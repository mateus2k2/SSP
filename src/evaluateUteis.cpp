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

vector<vector<int>> SSP::splitSolutionIntoMachines(const vector<int>& input, size_t n) {
    if (n <= 0) {
        throw invalid_argument("Number of parts must be greater than 0.");
    }

    size_t totalSize = input.size();
    size_t baseSize = totalSize / n;
    size_t remainder = totalSize % n;

    vector<vector<int>> result;
    auto it = input.begin();

    for (size_t i = 0; i < n; ++i) {
        size_t currentSize = baseSize + (i < remainder ? 1 : 0);
        vector<int> part(it, it + currentSize);
        result.push_back(move(part));
        it += currentSize;
    }

    return result;
}

double SSP::evaluateReport(solSSP& solution, fstream& solutionReportFile) {
    solutionReportFile << inputJobsFile << ";" << inputToolsetsFile << endl;
    solutionReportFile << planingHorizon << ";" << unsupervised << ";" << DAY << endl;

    int fineshedJobsCountTotal = 0;
    int switchsTotal = 0;
    int switchsInstancesTotal = 0;
    int unfineshedPriorityCountTotal = numberOfPriorityJobs;
    int totalUnfineshed = numberJobsUngrouped;

    solSSP sol = expandSolution(solution);

    // int startIndex = 0;
    // for (int i = 0; i < numberMachines; i++) {
    //     auto [fineshedJobsCount, switchs, switchsInstances, fineshedPriorityCount, curStartIndex] = KTNSReport(sol.sol, startIndex, solutionReportFile, i);
    //     startIndex = curStartIndex;
    //     switchsTotal += switchs;
    //     switchsInstancesTotal += switchsInstances;
    //     unfineshedPriorityCountTotal -= fineshedPriorityCount;
    //     totalUnfineshed -= fineshedJobsCount;
    //     fineshedJobsCountTotal += fineshedJobsCount;
    // }
    vector<vector<int>> machines = splitSolutionIntoMachines(sol.sol, numberMachines);
    for (size_t i = 0; i < machines.size(); i++) {
        auto [fineshedJobsCount, switchs, switchsInstances, fineshedPriorityCount, _] = KTNSReport(machines[i], 0, solutionReportFile, i);
        fineshedJobsCountTotal += fineshedJobsCount;
        switchsTotal += switchs;
        switchsInstancesTotal += switchsInstances;
        unfineshedPriorityCountTotal -= fineshedPriorityCount;
        totalUnfineshed -= fineshedJobsCount;
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
    vector<bool> toolUsedUnsupervided(numberToolsReal, false);
    vector<bool> magazineL(numberToolsReal, true);
    vector<bool> magazineBKP(numberToolsReal, true);
    unsigned int switchs = 0;
    int numberJobsSol = s.size();
    int jL;

    int switchsInstances = 0;
    int currantSwitchs = 0;
    int fineshedJobsCount = 0;
    int fineshedPriorityCount = 0;

    bool unsupervisedSwitch = false;

    int inicioJob = 0;
    int fimJob = 0;

    solutionReportFile << "Machine: " << machine << std::endl;

    // for (jL = startIndex; jL < numberJobsSol; ++jL) {
    //     const auto &job = originalJobs[s[jL]];
    //     cout << "Job: " << job.indexJob << ", Operation: " << job.indexOperation << endl;
    // }


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
        
        // Aramzenar o magazine antes de entrar no periodo sem supervisao
        if(inicioJob % (DAY) < unsupervised && fimJob % (DAY) > unsupervised && fimJob < (planingHorizon * DAY)) {
            cout << "Job " << originalJobs[s[jL]].indexJob << " entra no periodo nao supervisionado." << "fimJob % (DAY): " << fimJob % (DAY) << " unsupervised: " << unsupervised << " (planingHorizon * DAY): " << (planingHorizon * DAY) << endl;
            magazineBKP = magazineL;
            unsupervisedSwitch = false;
            toolUsedUnsupervided.assign(numberToolsReal, false);
        }

        // Ferramentas q nao podem ser subistituidas no periodo sem supervisao
        if(inicioJob % (DAY) < unsupervised && fimJob % (DAY) > unsupervised && fimJob < (planingHorizon * DAY) || ((inicioJob % DAY) >= unsupervised)) {
            for (auto it = originalJobs[s[jL]].toolSet.tools.begin(); it != originalJobs[s[jL]].toolSet.tools.end(); ++it) {
                toolUsedUnsupervided[*it] = true;  
            }
        }


        //verificacoes enquanto estiver no periodo sem supervisao
        if ((inicioJob % DAY) >= unsupervised) {
            cout << "Job " << originalJobs[s[jL]].indexJob << " is in unsupervised period." << " CurrantSwitches: " << currantSwitchs << endl;
            // for (int t = 0; t < numberToolsReal; ++t) {
            //     if (magazineL[t] && !magazineBKP[t]) {
            //         cout << "Tool " << t << " was not in magazine before unsupervised period." << endl;
            //     }
            // }
            // for (int t = 0; t < numberToolsReal; ++t) {
            //     if (!magazineL[t] && magazineBKP[t]) {
            //         cout << "Tool " << t << " was in magazine before unsupervised period." << endl;
            //     }
            // }
            // cout << endl << endl << endl;
            
            //itera pelo magaizine atual
            for (int t = 0; t < numberToolsReal; ++t) {
                if (!magazineL[t] && toolUsedUnsupervided[t]) {
                    cout << "Tool " << t << " é usado por alguma tarefa no periodo nao supervisionado e foi substituida." << endl;
                }
            }
            cout << endl;

            // se esta em um periodo sem supervisao e houve troca de ferramenta
            // pegar o magazine antes de entrar no periodo sem supervisao e fazer as trocas nele, 
            // modificar esse magazine trocando apenas as ferramentas q ja estavam
            // se for necessario trocar uma ferramenta que nao estava no magazine, mandar a tarefa para o proximo dia
        }
        
        if (((inicioJob % DAY) >= unsupervised && (currantSwitchs > 0))) { // verificar se o job excede o horizonte de planejamento
            inicioJob += DAY - (inicioJob % DAY);
            fimJob = inicioJob + originalJobs[s[jL]].processingTime;
        }


        if (fimJob > (planingHorizon * DAY)) {
            break;
        }

        inicioJob = fimJob;

        // ---------------------------------------------------------------------------
        // COSTS
        // ---------------------------------------------------------------------------

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
