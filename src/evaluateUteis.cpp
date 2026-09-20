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

    // expandSolution keeps the two operations of a reentrant job adjacent; a cut
    // between them would put op 1 on the next machine and break the strong chain,
    // so such a cut moves one position forward (op 1 stays with op 0)
    auto cutsPair = [&](size_t cut) {
        const Job& before = originalJobs[input[cut - 1]];
        const Job& after = originalJobs[input[cut]];
        return before.indexJob == after.indexJob && before.indexOperation == 0 && after.indexOperation == 1;
    };

    vector<vector<int>> result;
    size_t begin = 0;

    for (size_t i = 0; i < n; ++i) {
        size_t end = (i == n - 1) ? totalSize : min(totalSize, begin + baseSize + (i < remainder ? 1 : 0));
        if (end > begin && end < totalSize && cutsPair(end)) ++end;
        result.emplace_back(input.begin() + begin, input.begin() + end);
        begin = end;
    }

    return result;
}

vector<vector<int>> SSP::splitSolutionIntoMachinesByTime(const vector<int>& input, size_t h) {
    
    // int processingTimeSum = originalJobsCopy[s[jL]].processingTime;
    
    vector<vector<int>> result;

    // iterate over the input and sum the processing times until it reaches h, then create a new vector for the next machine
    vector<int> currentMachine;
    int currentTime = 0;
    for (size_t i = 0; i < input.size(); ++i) {
        int processingTime = originalJobs[input[i]].processingTime;
        if (currentTime + processingTime > h) {
            result.push_back(move(currentMachine));
            currentMachine.clear();
            currentTime = 0;
        }
        currentMachine.push_back(input[i]);
        currentTime += processingTime;
    }
    if (!currentMachine.empty()) {
        result.push_back(move(currentMachine));
    }

    return result;
}

double SSP::evaluateReport(solSSP& solution, fstream& solutionReportFile) {
    solutionReportFile << inputJobsFile << ";" << inputToolsetsFile << endl;
    solutionReportFile << horizonMinutes / DAY << ";" << unsupervisedStart << ";" << DAY << endl;

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
    // vector<vector<int>> machines = splitSolutionIntoMachinesByTime(sol.sol, horizonMinutes);
    int criticalMachine = 0;
    int criticalMachineSwitchs = 0;

    for (size_t i = 0; i < machines.size(); i++) {
        auto [fineshedJobsCount, switchs, switchsInstances, fineshedPriorityCount, _, lastTime] = KTNSReport(machines[i], 0, solutionReportFile, i);
        fineshedJobsCountTotal += fineshedJobsCount;
        switchsTotal += switchs;
        switchsInstancesTotal += switchsInstances;
        unfineshedPriorityCountTotal -= fineshedPriorityCount;
        totalUnfineshed -= fineshedJobsCount;
        
        if (lastTime > criticalMachine) {
            criticalMachine = lastTime;
            criticalMachineSwitchs = switchs;
        }
    }

    solutionReportFile << "END" << endl;
    solutionReportFile << "fineshedJobsCount: " << fineshedJobsCountTotal << endl;
    solutionReportFile << "switchs: " << switchsTotal << endl;
    solutionReportFile << "switchsInstances: " << switchsInstancesTotal << endl;
    solutionReportFile << "unfineshedPriorityCount: " << unfineshedPriorityCountTotal << endl;
    solutionReportFile << "totalUnfineshed: " << totalUnfineshed << endl;
    solutionReportFile << "criticalMachineSwitchs: " << criticalMachineSwitchs << endl;
    solutionReportFile << "criticalMachineTime: " << criticalMachine << endl;

    // if(totalUnfineshed != 0) {
    //     cout << "Warning: totalUnfineshed (" << totalUnfineshed << ") is not equal to the size of the solution (" << solution.sol.size() << ")" << endl;
    // }

    int cost = (PROFITYFINISHED * fineshedJobsCountTotal) - (COSTSWITCH * switchsTotal) - (COSTSWITCHINSTANCE * switchsInstancesTotal) - (COSTPRIORITY * unfineshedPriorityCountTotal);
    
    return cost;
}

tuple<int, int, int, int, int, int>  SSP::KTNSReport(vector<int> s, int startIndex, fstream& solutionReportFile, int machine) {
    vector<Job> originalJobsCopy = originalJobs;
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
    int lastTime = 0;

    solutionReportFile << "Machine: " << machine << std::endl;

    // an operation needing more tools than the magazine holds can never be processed:
    // it is skipped (left unfinished) instead of being run with part of its tools
    auto fitsMagazine = [&](int k) { return (int)originalJobsCopy[s[k]].toolSet.tools.size() <= capacityMagazine; };

    for (jL = startIndex; jL < numberJobsSol; ++jL) {
        if (!fitsMagazine(jL)) continue;

        // ---------------------------------------------------------------------------
        // UNSUPERVISED PERIOD FIX
        // ---------------------------------------------------------------------------

        int processingTimeSum = originalJobsCopy[s[jL]].processingTime;
        if((originalJobsCopy[s[jL]].isReentrant && !originalJobsCopy[s[jL]].isGrouped) && originalJobsCopy[s[jL]].indexOperation == 0) processingTimeSum = std::accumulate(originalJobsCopy[s[jL]].processingTimes.begin(), originalJobsCopy[s[jL]].processingTimes.end(), 0);
        fimJob = inicioJob + originalJobsCopy[s[jL]].processingTime;

        // Estou no periodo de supervisao e entrando no periodo sem supervisao
        if (inicioJob % DAY < unsupervisedStart && fimJob % DAY > unsupervisedStart && fimJob < horizonMinutes) {
            vector<bool> magazineAntes = magazineL;
            set<int> unsupervisedMagazine;
            int inicioUnsupervised = inicioJob;
            int fimUnsupervised = fimJob;
            
            int k;
            for(k = jL; k < numberJobsSol; ++k) {
                // verificacao de ferramentas
                vector<int> newTools;
                bool breakLoop = false;
                for (const auto& tool : originalJobsCopy[s[k]].toolSet.tools) {
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
                // verificacao de tempo
                if ((((inicioUnsupervised % DAY) >= unsupervisedStart) && (fimUnsupervised % DAY) < unsupervisedStart) || (breakLoop))  {
                    if( (fimUnsupervised + (DAY - unsupervisedStart) >= horizonMinutes) && (originalJobsCopy[s[k]].indexOperation == 1)) {
                        originalJobsCopy[s[k-1]].flag = true; // flag para indicar que a tarefa foi interrompida
                    }
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
        vector<bool> magazineCL(numberToolsReal);
        int left = jL;
        int cmL = 0;

        while ((cmL < capacityMagazine) && (left < numberJobsSol)) {
            if (!fitsMagazine(left)) { ++left; continue; }
            for (auto it = originalJobsCopy[s[left]].toolSet.tools.begin(); ((it != originalJobsCopy[s[left]].toolSet.tools.end()) && (cmL < capacityMagazine)); ++it) {
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

        if (((inicioJob % DAY) >= unsupervisedStart && (currantSwitchs > 0)) ||                         // verificar se estou em um periodo sem supervisao e houve troca de ferramenta
            (inicioJob % horizonMinutes + (processingTimeSum) > horizonMinutes) || // verificar se o job excede o horizonte de planejamento
            (originalJobsCopy[s[jL]].flag == true)) {
            inicioJob += DAY - (inicioJob % DAY);
            fimJob = inicioJob + originalJobsCopy[s[jL]].processingTime;
        }

        if (fimJob > horizonMinutes) {
            break;
        }

        inicioJob = fimJob;

        // ---------------------------------------------------------------------------
        // COSTS
        // ---------------------------------------------------------------------------

        switchs += currantSwitchs;
        if (currantSwitchs > 0) ++switchsInstances;

        fineshedJobsCount += originalJobsCopy[s[jL]].isGrouped ? 2 : 1;
        if (originalJobsCopy[s[jL]].priority) fineshedPriorityCount += originalJobsCopy[s[jL]].isGrouped ? 2 : 1;

        // ---------------------------------------------------------------------------
        // PRINTS
        // ---------------------------------------------------------------------------

        int startTMP = (fimJob - originalJobsCopy[s[jL]].processingTime) % horizonMinutes;
        int endTMP = ((fimJob - 1) % horizonMinutes) + 1;
        lastTime = endTMP;

        const auto &job = originalJobsCopy[s[jL]];
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

    return {fineshedJobsCount, switchs, switchsInstances, fineshedPriorityCount, jL, lastTime};
}