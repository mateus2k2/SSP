#include "headers/SSP.h"

#ifdef FMT
#include <fmt/core.h>
#include <fmt/ranges.h>
#endif

struct Interval {
    int start;
    int end;
};

solSSP SSP::expandSolution(solSSP& s) {
    solSSP expandedSol;

    for (size_t i = 0; i < s.sol.size(); ++i) {
        Job jobGrouped = groupedJobs[s.sol[i]];
        auto indexOperation1 = mapJobsToOriginalIndex.find(make_tuple(jobGrouped.indexJob, 0));
        if (indexOperation1 != mapJobsToOriginalIndex.end()) {
            expandedSol.sol.push_back(indexOperation1->second);
        }
        auto indexOperation2 = mapJobsToOriginalIndex.find(make_tuple(jobGrouped.indexJob, 1));
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

tuple<int, int, int, int, int> SSP::KTNSReport(vector<int> s, int startIndex, fstream& solutionReportFile, int machine) {
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

    bool horizonteExcedido = false; 
    int atualIndex = 0;
    bool tamanhoMagazineExcedido = false;
    set<int> unsupervisedMagazine;
    bool leavingUnsupervised = false;
    int unsuperviedJobsCount = 0;
    int unsupervisedJobStartIndex = 0;
    int unsupervisedJobEndIndex = 0;
    map<tuple<int, int>, Interval> unsupervisedIntervals;
    vector<bool> toolUsedUnsupervided(numberToolsReal, false);
    vector<bool> lastMag(numberToolsReal, true);

    solutionReportFile << "Machine: " << machine << endl;

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

        fimJob = inicioJob + originalJobs[s[jL]].processingTime;

        // Entrou no periodo de supervisao e entrando no periodo sem supervisao
        if (inicioJob % (DAY) < unsupervised && fimJob % (DAY) > unsupervised && fimJob < (planingHorizon * DAY)) {
            leavingUnsupervised = false;
            unsupervisedIntervals.clear();
            unsupervisedMagazine.clear();
            unsuperviedJobsCount = 0;
            unsupervisedJobStartIndex = jL;
            lastMag = magazineL;
        }

        // estou no començo do periodo sem supervisao ou dentro do periodo sem supervisao
        if (((inicioJob % (DAY) < unsupervised && fimJob % (DAY) > unsupervised && fimJob < (planingHorizon * DAY)) || ((inicioJob % DAY) >= unsupervised)) && fimJob < (planingHorizon * DAY)) {
            ToolSet currentToolSet = originalJobs[s[jL]].toolSet;
            ToolSet nextToolset = originalJobs[s[jL+1]].toolSet;
            unsuperviedJobsCount++;
            currantSwitchs = 0;
            for (auto it = currentToolSet.tools.begin(); it != currentToolSet.tools.end(); ++it) unsupervisedMagazine.insert(*it);
            
            if (nextToolset.tools.size() + unsupervisedMagazine.size() > (size_t)capacityMagazine) {
                vector<int> unusedTools;
                for (int t = 0; t < numberToolsReal; ++t) {
                    if (unsupervisedMagazine.find(t) == unsupervisedMagazine.end() && lastMag[t] == true) {
                        unusedTools.push_back(t);
                    }
                }
                int removeIndex = 0;
                for (int t = 0; t < numberToolsReal; ++t) {
                    if (unsupervisedMagazine.find(t) != unsupervisedMagazine.end() && lastMag[t] == false) {
                        ++currantSwitchs;
                        lastMag[t] = true;
                        if (removeIndex < unusedTools.size()) {
                            lastMag[unusedTools[removeIndex]] = false;
                            ++removeIndex;
                        }
                    }
                }
                tamanhoMagazineExcedido = true;
                atualIndex = jL;
                leavingUnsupervised = true;
                unsupervisedJobEndIndex = jL;
                magazineCL = lastMag;
                magazineL = magazineCL;
            }
        }

        cout << "TESTE: " << originalJobs[s[jL]].indexJob << " Tools: " << originalJobs[s[jL]].toolSet.tools.size() << endl;


        // estou saindo do periodo sem supervisao
        if (((inicioJob % DAY) >= unsupervised) && (fimJob % DAY) < unsupervised) {
            cout << "Leaving unsupervised period at job: " << originalJobs[s[jL]].indexJob << " inicioJob: " << inicioJob % DAY << " unsupervised: " << unsupervised << endl;
            leavingUnsupervised = true;
            unsupervisedJobEndIndex = jL;
            vector<int> unusedTools;
            for (int t = 0; t < numberToolsReal; ++t) {
                if (unsupervisedMagazine.find(t) == unsupervisedMagazine.end() && lastMag[t] == true) {
                    unusedTools.push_back(t);
                }
            }
            int removeIndex = 0;
            for (int t = 0; t < numberToolsReal; ++t) {
                if (unsupervisedMagazine.find(t) != unsupervisedMagazine.end() && lastMag[t] == false) {
                    ++currantSwitchs;
                    lastMag[t] = true;
                    if (removeIndex < unusedTools.size()) {
                        lastMag[unusedTools[removeIndex]] = false;
                        ++removeIndex;
                    }
                }
            }
            magazineCL = lastMag;
            magazineL = magazineCL;
        }

        if((tamanhoMagazineExcedido && jL == atualIndex + 1)) unsuperviedJobsCount = 0;
        if (((inicioJob % DAY) >= unsupervised) && (tamanhoMagazineExcedido && jL == atualIndex + 1)) {  // verificar se o job excede o horizonte de planejamento
            atualIndex = jL;
            tamanhoMagazineExcedido = false;
            inicioJob += DAY - (inicioJob % DAY);
            fimJob = inicioJob + originalJobs[s[jL]].processingTime;
            leavingUnsupervised = true;
            unsupervisedJobEndIndex = jL;
        }

        
        // ---------------------------------------------------------------------------
        // COSTS
        // ---------------------------------------------------------------------------
        
        if (fimJob > (planingHorizon * DAY)) {
            unsupervisedJobEndIndex = jL - 1;
            currantSwitchs = 0;
        }

        else{
            switchs += currantSwitchs;
            if (currantSwitchs > 0) ++switchsInstances;
    
            fineshedJobsCount += originalJobs[s[jL]].isGrouped ? 2 : 1;
            if (originalJobs[s[jL]].priority) fineshedPriorityCount += originalJobs[s[jL]].isGrouped ? 2 : 1;
        }
        cout << "Job: " << originalJobs[s[jL]].indexJob << " | Operation: " << originalJobs[s[jL]].indexOperation << " | Start: " << inicioJob << " | End: " << fimJob << " | Switches: " << currantSwitchs << endl;

        // ---------------------------------------------------------------------------
        // PRINTS
        // ---------------------------------------------------------------------------

        int startTMP = (fimJob - originalJobs[s[jL]].processingTime) % (planingHorizon * DAY);
        int endTMP = ((fimJob - 1) % (planingHorizon * DAY)) + 1;

        const auto& job = originalJobs[s[jL]];
        bool isGrouped = job.isGrouped;
        int loops = isGrouped ? 2 : 1;

        Interval interval = {startTMP, endTMP};
        unsupervisedIntervals[make_tuple(job.indexJob, job.indexOperation)] = interval;

        auto writeJobDetailsMagazine = [&](int job_, int start, int end, int operation, int priority_) {
            solutionReportFile << job_ << ";" << operation << ";" << start << ";" << end << ";" << priority_ << ";";
            for (size_t t = 0; t < magazineCL.size(); ++t) {
                if (magazineCL[t]) {
                    solutionReportFile << t << ",";
                }
            }
            solutionReportFile << "\n";
        };

        auto writeJobDetailsUnsupervised = [&](int job_, int start, int end, int operation, int priority_) {
            solutionReportFile << job_ << ";" << operation << ";" << start << ";" << end << ";" << priority_ << ";";
            for (size_t t = 0; t < lastMag.size(); ++t) {
                if (lastMag[t]) {
                    solutionReportFile << t << ",";
                }
            }
            solutionReportFile << "\n";
        };

        if(unsuperviedJobsCount == 0) {
            for (int i = 0; i < loops; ++i) {
                if (isGrouped && i == 0) {
                    writeJobDetailsMagazine(job.indexJob, startTMP, startTMP + job.processingTimes[0], 0, job.priority);
                } else if (isGrouped && i == 1) {
                    writeJobDetailsMagazine(job.indexJob, startTMP + job.processingTimes[0], endTMP, 1, job.priority);
                } else {
                    writeJobDetailsMagazine(job.indexJob, startTMP, endTMP, job.indexOperation, job.priority);
                }
            }
        } 
        else if (leavingUnsupervised) {
            for (int k = unsupervisedJobStartIndex; k <= unsupervisedJobEndIndex; ++k) {
                const auto& curJob = originalJobs[s[k]];
                bool curIsGrouped = curJob.isGrouped;
                int curLoops = curIsGrouped ? 2 : 1;
                
                for (int i = 0; i < curLoops; ++i) {
                    int curStart = unsupervisedIntervals[make_tuple(curJob.indexJob, i)].start;
                    int curEnd = unsupervisedIntervals[make_tuple(curJob.indexJob, i)].end;
                    writeJobDetailsUnsupervised(curJob.indexJob, curStart, curEnd, i, curJob.priority);
                }
            }
            leavingUnsupervised = false;
            unsupervisedIntervals.clear();
            unsupervisedMagazine.clear();
            unsuperviedJobsCount = 0;
        }
        
        // ---------------------------------------------------------------------------
        // TIME VERIFICATIONS
        // ---------------------------------------------------------------------------

        if (fimJob > (planingHorizon * DAY)) {
            break;
        }

        inicioJob = fimJob;
    }

    return {fineshedJobsCount, switchs, switchsInstances, fineshedPriorityCount, jL};
}
