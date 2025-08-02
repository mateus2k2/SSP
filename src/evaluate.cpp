#include <algorithm>
#include <atomic>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <thread>
#include <vector>
#include <numeric>

#include "headers/GlobalVars.h"
#include "headers/SSP.h"

#ifdef FMT
#include <fmt/core.h>
#include <fmt/ranges.h>
#endif

using namespace std;

double SSP::evaluate(solSSP& s) {
    int fineshedJobsCountTotal = 0;
    int switchsTotal = 0;
    int switchsInstancesTotal = 0;
    int unFineshedPriorityCountTotal = numberOfPriorityJobs;
    int totalUnfineshed = numberJobsUngrouped;

    solSSP sol = expandSolution(s);

    // int startIndex = 0;
    // for (int i = 0; i < numberMachines; i++) {
    //     auto [fineshedJobsCount, switchs, switchsInstances, fineshedPriorityCount, curStartIndex] = KTNS(sol.sol, startIndex);
    //     startIndex = curStartIndex;
    //     switchsTotal += switchs;
    //     switchsInstancesTotal += switchsInstances;
    //     unFineshedPriorityCountTotal -= fineshedPriorityCount;
    //     totalUnfineshed -= fineshedJobsCount;
    //     fineshedJobsCountTotal += fineshedJobsCount;
    // }
    vector<vector<int>> machines = splitSolutionIntoMachines(sol.sol, numberMachines);
    for (size_t i = 0; i < machines.size(); i++) {
        auto [fineshedJobsCount, switchs, switchsInstances, fineshedPriorityCount, trash] = KTNS(machines[i], 0);
        fineshedJobsCountTotal += fineshedJobsCount;
        switchsTotal += switchs;
        switchsInstancesTotal += switchsInstances;
        unFineshedPriorityCountTotal -= fineshedPriorityCount;
        totalUnfineshed -= fineshedJobsCount;
    }

    return -((PROFITYFINISHED * fineshedJobsCountTotal) - (COSTSWITCH * switchsTotal) - (COSTSWITCHINSTANCE * switchsInstancesTotal) - (COSTPRIORITY * unFineshedPriorityCountTotal));
}

tuple<int, int, int, int, int> SSP::KTNS(vector<int> s, int startIndex) {
    
    // int currNumberJobs = numberJobs;
    vector<Job> originalJobsCopy = originalJobs;
    int currNumberJobs = s.size();

    vector<bool> magazineL(numberTools, true);
    unsigned int switchs = 0;
    int jL;

    int switchsInstances = 0;
    int currantSwitchs = 0;
    int fineshedJobsCount = 0;
    int fineshedPriorityCount = 0;

    int inicioJob = 0;
    int fimJob = 0;

    for (jL = startIndex; jL < currNumberJobs; ++jL) {
        // ---------------------------------------------------------------------------
        // UNSUPERVISED PERIOD FIX
        // ---------------------------------------------------------------------------

        int processingTimeSum = originalJobsCopy[s[jL]].processingTime;
        if((originalJobsCopy[s[jL]].isReentrant && !originalJobsCopy[s[jL]].isGrouped) && originalJobsCopy[s[jL]].indexOperation == 0) processingTimeSum = std::accumulate(originalJobsCopy[s[jL]].processingTimes.begin(), originalJobsCopy[s[jL]].processingTimes.end(), 0);
        fimJob = inicioJob + originalJobsCopy[s[jL]].processingTime;

        fimJob = inicioJob + originalJobsCopy[s[jL]].processingTime;

        // Estou no periodo de supervisao e entrando no periodo sem supervisao
        if (inicioJob % (DAY) < unsupervised && fimJob % (DAY) > unsupervised && fimJob < (planingHorizon * DAY)) {
            vector<bool> magazineAntes = magazineL;
            set<int> unsupervisedMagazine;
            int inicioUnsupervised = inicioJob;
            int fimUnsupervised = fimJob;
            
            int k;
            for(k = jL; k < currNumberJobs; ++k) {
                // verificacao de ferramentas
                vector<int> newTools;
                bool breakLoop = false;
                for (const auto& tool : originalJobsCopy[s[k]].toolSetNormalized.tools) {
                    if (unsupervisedMagazine.find(tool) == unsupervisedMagazine.end()) {
                        newTools.push_back(tool);
                        unsupervisedMagazine.insert(tool);
                    }
                    if((int)unsupervisedMagazine.size() >= capacityMagazine) {
                        for (const auto& newTool : newTools) unsupervisedMagazine.erase(newTool);
                        breakLoop = true;
                        break;
                    }
                }
                if (breakLoop) break;

                // verificacao de tempo
                if (((inicioUnsupervised % DAY) >= unsupervised) && (fimUnsupervised % DAY) < unsupervised) {
                    break;
                }
                
                // passando o tempo
                inicioUnsupervised += originalJobsCopy[s[k]].processingTime;
                fimUnsupervised = inicioUnsupervised + originalJobsCopy[s[k]].processingTime;
            }
            // create a new toolset with the tools used in the unsupervised period and put it in all the jobs in the unsupervised period
            ToolSet unsupervisedToolSet;
            unsupervisedToolSet.indexToolSet = -1; // -1 to indicate that this is a temporary toolset
            unsupervisedToolSet.tools.assign(unsupervisedMagazine.begin(), unsupervisedMagazine.end());
            for (int l = jL; l < k; ++l) {
                originalJobsCopy[s[l]].toolSetNormalized = unsupervisedToolSet;
                originalJobsCopy[s[l]].toolSet = unsupervisedToolSet;
            }
        }

        // ---------------------------------------------------------------------------
        // switchs
        // ---------------------------------------------------------------------------

        currantSwitchs = 0;
        vector<bool> magazineCL(numberTools);
        int left = jL;
        int cmL = 0;

        while ((cmL < capacityMagazine) && (left < currNumberJobs)) {
            for (auto it = originalJobsCopy[s[left]].toolSetNormalized.tools.begin(); ((it != originalJobsCopy[s[left]].toolSetNormalized.tools.end()) && (cmL < capacityMagazine)); ++it) {
                if ((magazineL[*it]) && (!magazineCL[*it])) {
                    magazineCL[*it] = true;
                    ++cmL;
                } else if ((jL == left) && (!magazineCL[*it])) {
                    magazineCL[*it] = true;
                    ++cmL;
                    ++currantSwitchs;
                }
            }
            ++left;
        }

        for (int t = 0; ((t < numberTools) && (cmL < capacityMagazine)); t++) {
            if ((magazineL[t]) && (!magazineCL[t])) {
                magazineCL[t] = true;
                ++cmL;
            }
        }
        magazineL = magazineCL;

        // ---------------------------------------------------------------------------
        // TIME VERIFICATIONS
        // ---------------------------------------------------------------------------

        if (((inicioJob % DAY) >= unsupervised && (currantSwitchs > 0)) ||                          // verificar se estou em um periodo semsupervisao e houve troca de ferramenta
            (inicioJob % (planingHorizon * DAY) + (processingTimeSum) > (planingHorizon * DAY))) {  // verificar se o job excede o horizonte de planejamento unico (iria extender de uma maquina para outra)
            inicioJob += DAY - (inicioJob % DAY);
            fimJob = inicioJob + originalJobsCopy[s[jL]].processingTime;
        }

        if (fimJob > (planingHorizon * DAY)) break;

        inicioJob = fimJob;

        // ---------------------------------------------------------------------------
        // COSTS
        // ---------------------------------------------------------------------------

        switchs += currantSwitchs;
        if (currantSwitchs > 0) ++switchsInstances;
        fineshedJobsCount += originalJobsCopy[s[jL]].isGrouped ? 2 : 1;
        if (originalJobsCopy[s[jL]].priority) fineshedPriorityCount += originalJobsCopy[s[jL]].isGrouped ? 2 : 1;
    }

    return {fineshedJobsCount, switchs, switchsInstances, fineshedPriorityCount, jL};
}
