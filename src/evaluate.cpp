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

    int startIndex = 0;
    for (int i = 0; i < numberMachines; i++) {
        auto [fineshedJobsCount, switchs, switchsInstances, fineshedPriorityCount, curStartIndex] = KTNS(sol.sol, startIndex);
        startIndex = curStartIndex;
        switchsTotal += switchs;
        switchsInstancesTotal += switchsInstances;
        unFineshedPriorityCountTotal -= fineshedPriorityCount;
        totalUnfineshed -= fineshedJobsCount;
        fineshedJobsCountTotal += fineshedJobsCount;
    }

    return -((PROFITYFINISHED * fineshedJobsCountTotal) - (COSTSWITCH * switchsTotal) - (COSTSWITCHINSTANCE * switchsInstancesTotal) - (COSTPRIORITY * unFineshedPriorityCountTotal));
}

tuple<int, int, int, int, int> SSP::KTNS(vector<int> s, int startIndex) {
    vector<bool> magazineL(numberTools, true);
    unsigned int switchs = 0;
    int jL;

    int switchsInstances = 0;
    int currantSwitchs = 0;
    int fineshedJobsCount = 0;
    int fineshedPriorityCount = 0;

    int inicioJob = 0;
    int fimJob = 0;

    for (jL = startIndex; jL < numberJobs; ++jL) {
        // ---------------------------------------------------------------------------
        // switchs
        // ---------------------------------------------------------------------------

        currantSwitchs = 0;
        vector<bool> magazineCL(numberTools);
        int left = jL;
        int cmL = 0;

        while ((cmL < capacityMagazine) && (left < numberJobs)) {
            for (auto it = originalJobs[s[left]].toolSetNormalized.tools.begin(); ((it != originalJobs[s[left]].toolSetNormalized.tools.end()) && (cmL < capacityMagazine)); ++it) {
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

        int processingTimeSum = originalJobs[s[jL]].processingTime;
        if((originalJobs[s[jL]].isReentrant && !originalJobs[s[jL]].isGrouped) && originalJobs[s[jL]].indexOperation == 0) processingTimeSum = std::accumulate(originalJobs[s[jL]].processingTimes.begin(), originalJobs[s[jL]].processingTimes.end(), 0);

        fimJob = inicioJob + originalJobs[s[jL]].processingTime;

        if (((inicioJob % DAY) >= unsupervised && (currantSwitchs > 0)) ||                          // verificar se estou em um periodo semsupervisao e houve troca de ferramenta
            (inicioJob % (planingHorizon * DAY) + (processingTimeSum) > (planingHorizon * DAY))) {  // verificar se o job excede o horizonte de planejamento unico (iria extender de uma maquina para outra)
            inicioJob += DAY - (inicioJob % DAY);
            fimJob = inicioJob + originalJobs[s[jL]].processingTime;
        }

        if (fimJob > (planingHorizon * DAY)) break;

        inicioJob = fimJob;

        // ---------------------------------------------------------------------------
        // COSTS
        // ---------------------------------------------------------------------------

        switchs += currantSwitchs;
        if (currantSwitchs > 0) ++switchsInstances;
        fineshedJobsCount += originalJobs[s[jL]].isGrouped ? 2 : 1;
        if (originalJobs[s[jL]].priority) fineshedPriorityCount += originalJobs[s[jL]].isGrouped ? 2 : 1;
    }

    return {fineshedJobsCount, switchs, switchsInstances, fineshedPriorityCount, jL};
}

