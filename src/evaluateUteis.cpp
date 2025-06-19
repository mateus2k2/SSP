#include "headers/SSP.h"

#ifdef FMT
#include <fmt/core.h>
#include <fmt/ranges.h>
#endif

solSSP SSP::expandSolution(solSSP& s) {
    solSSP expandedSol;

    for (size_t i = 0; i < s.sol.size(); ++i) {
        Job jobGrouped = groupedJobs[s.sol[i]];
        auto indexOperation1 = mapJobsToOriginalIndex.find(std::make_tuple(jobGrouped.indexJob, 0));
        if (indexOperation1 != mapJobsToOriginalIndex.end()) {
            expandedSol.sol.push_back(indexOperation1->second);
        }
        auto indexOperation2 = mapJobsToOriginalIndex.find(std::make_tuple(jobGrouped.indexJob, 1));
        if (indexOperation2 != mapJobsToOriginalIndex.end()) {
            expandedSol.sol.push_back(indexOperation2->second);
        }
        expandedSol.evalSol = s.evalSol;
    }

    return expandedSol;
}

double SSP::evaluateReport(solSSP& solution, string filenameJobs, string filenameTools, fstream& solutionReportFile) {
    solutionReportFile << filenameJobs << ";" << filenameTools << endl;
    solutionReportFile << planingHorizon << ";" << unsupervised << ";" << DAY << endl;

    int fineshedJobsCountTotal = 0;
    int switchsTotal = 0;
    int switchsInstancesTotal = 0;
    int unfineshedPriorityCountTotal = numberOfPriorityJobs;
    int totalUnfineshed = numberJobsUngrouped;

    solSSP sol = expandSolution(solution);

    int startIndex = 0;
    for (int i = 0; i < numberMachines; i++) {
        auto [fineshedJobsCount, switchs, switchsInstances, fineshedPriorityCount, curStartIndex] = KTNSReport(sol.sol, startIndex, solutionReportFile, i);
        startIndex = curStartIndex;
        switchsTotal += switchs;
        switchsInstancesTotal += switchsInstances;
        unfineshedPriorityCountTotal -= fineshedPriorityCount;
        totalUnfineshed -= fineshedJobsCount;
        fineshedJobsCountTotal += fineshedJobsCount;
    }

    solutionReportFile << "END" << endl;
    solutionReportFile << "fineshedJobsCount: " << fineshedJobsCountTotal << endl;
    solutionReportFile << "switchs: " << switchsTotal << endl;
    solutionReportFile << "switchsInstances: " << switchsInstancesTotal << endl;
    solutionReportFile << "unfineshedPriorityCount: " << unfineshedPriorityCountTotal << endl;
    solutionReportFile << "totalUnfineshed: " << totalUnfineshed << endl;

    int cost = (PROFITYFINISHED * fineshedJobsCountTotal) - (COSTSWITCH * switchsTotal) - (COSTSWITCHINSTANCE * switchsInstancesTotal) - (COSTPRIORITY * unfineshedPriorityCountTotal);
    
    return cost;
}

tuple<int, int, int, int, int>  SSP::KTNSReport(vector<int> s, int startIndex, fstream& solutionReportFile, int machine) {
    vector<bool> magazineL(numberToolsReal, true);
    unsigned int switchs = 0;
    int numberJobsSol = s.size();
    int jL;

    int switchsInstances = 0;
    int currantSwitchs = 0;
    int fineshedJobsCount = 0;
    int fineshedPriorityCount = 0;

    int inicioJob = 0;
    int fimJob = 0;
    int extendedPlaningHorizon = (planingHorizon * 1) * DAY;
    int isFirstJobOfMachine = 1;

    solutionReportFile << "Machine: " << machine << std::endl;

    for (jL = startIndex; jL < numberJobsSol; ++jL) {
        // ---------------------------------------------------------------------------
        // switchs
        // ---------------------------------------------------------------------------

        currantSwitchs = 0;
        vector<bool> magazineCL(numberToolsReal);
        int left = jL;
        int cmL = 0;

        while ((cmL < capacityMagazine) && (left < numberJobsSol)) {
            for (auto it = originalJobs[s[left]].toolSet.tools.begin(); ((it != originalJobs[s[left]].toolSet.tools.end()) && (cmL < capacityMagazine)); ++it) {
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

        for (int t = 0; ((t < numberToolsReal) && (cmL < capacityMagazine)); t++) {
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

        if (((inicioJob % DAY) >= unsupervised && (currantSwitchs > 0)) ||                          // verificar se estou em um periodo sem supervisao e houve troca de ferramenta
            (inicioJob % (planingHorizon * DAY) + (processingTimeSum) > (planingHorizon * DAY))) {  // verificar se o job excede o horizonte de planejamento unico (iria extender de uma maquina para outra)
            inicioJob += DAY - (inicioJob % DAY);
            fimJob = inicioJob + originalJobs[s[jL]].processingTime;
        }

        if ((inicioJob % (planingHorizon * DAY) == 0))
            isFirstJobOfMachine = 1;
        else
            isFirstJobOfMachine = 0;

        if (fimJob > extendedPlaningHorizon) {
            cout << "Job " << originalJobs[s[jL]].indexJob << " exceeds the planning horizon." << endl;
            cout << "Current job end time: " << fimJob << ", Planning horizon: " << extendedPlaningHorizon << endl;
            cout << "Job processing time: " << originalJobs[s[jL]].processingTime << endl;
            cout << "Job index: " << jL << endl;
            cout << "Job start time: " << inicioJob << endl;
            break;
        }

        inicioJob = fimJob;

        // ---------------------------------------------------------------------------
        // COSTS
        // ---------------------------------------------------------------------------

        if (isFirstJobOfMachine) currantSwitchs = 0;
        switchs += currantSwitchs;
        if (currantSwitchs > 0) ++switchsInstances;

        fineshedJobsCount += originalJobs[s[jL]].isGrouped ? 2 : 1;
        if (originalJobs[s[jL]].priority) fineshedPriorityCount += originalJobs[s[jL]].isGrouped ? 2 : 1;

        // ---------------------------------------------------------------------------
        // PRINTS
        // ---------------------------------------------------------------------------

        int startTMP = (fimJob - originalJobs[s[jL]].processingTime) % (planingHorizon * DAY);
        int endTMP = ((fimJob - 1) % (planingHorizon * DAY)) + 1;

        const auto &job = originalJobs[s[jL]];
        bool isGrouped = job.isGrouped;
        int loops = isGrouped ? 2 : 1;

        auto writeJobDetails = [&](int start, int end, int operation) {
            solutionReportFile << job.indexJob << ";" << operation << ";" << start << ";" << end << ";" << job.priority << ";";
            for (size_t t = 0; t < magazineCL.size(); ++t) {
                if (magazineCL[t]) {
                    solutionReportFile << t << ",";
                }
            }
            solutionReportFile << "\n";
        };

        for (int i = 0; i < loops; ++i) {
            if (isGrouped && i == 0) {
                writeJobDetails(startTMP, startTMP + job.processingTimes[0], 0);
            } else if (isGrouped && i == 1) {
                writeJobDetails(startTMP + job.processingTimes[0], endTMP, 1);
            } else {
                writeJobDetails(startTMP, endTMP, job.indexOperation);
            }
        }
    }

    return {fineshedJobsCount, switchs, switchsInstances, fineshedPriorityCount, jL};
}
