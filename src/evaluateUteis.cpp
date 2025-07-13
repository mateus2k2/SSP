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

tuple<int, int, int, int, int> SSP::KTNSReport(vector<int> s, int startIndex, fstream& solutionReportFile, int machine) {
    vector<bool> toolUsedUnsupervided(numberToolsReal, false);
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

    set<int> unsupervisedMagazine;
    bool leavingUnsupervised = false;
    int unsuperviedJobsCount = 0;
    int unsupervisedJobStartIndex = 0;
    map<tuple<int, int>, Interval> unsupervisedIntervals;

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

        // int processingTimeSum = originalJobs[s[jL]].processingTime;
        // if ((originalJobs[s[jL]].isReentrant && !originalJobs[s[jL]].isGrouped) && originalJobs[s[jL]].indexOperation == 0) processingTimeSum = std::accumulate(originalJobs[s[jL]].processingTimes.begin(), originalJobs[s[jL]].processingTimes.end(), 0);

        fimJob = inicioJob + originalJobs[s[jL]].processingTime;

        // Entrou no periodo de supervisao e entrando no periodo sem supervisao
        if (inicioJob % (DAY) < unsupervised && fimJob % (DAY) > unsupervised && fimJob < (planingHorizon * DAY)) {
            leavingUnsupervised = false;
            unsupervisedIntervals.clear();
            unsupervisedMagazine.clear();
            unsuperviedJobsCount = 0;
        }

        // estou no començo do periodo sem supervisao ou dentro do periodo sem supervisao
        if (((inicioJob % (DAY) < unsupervised && fimJob % (DAY) > unsupervised && fimJob < (planingHorizon * DAY)) || ((inicioJob % DAY) >= unsupervised)) && fimJob < (planingHorizon * DAY)) {
            // cout << "Job: " << originalJobs[s[jL]].indexJob << endl;
            unsuperviedJobsCount++;
            ToolSet currentToolSet = originalJobs[s[jL]].toolSet;
            if (currentToolSet.tools.size() + unsupervisedMagazine.size() > (size_t)capacityMagazine) {
                // cout << "Não consigo colocar o job: " << originalJobs[s[jL]].indexJob << " no periodo sem supervisao" << " inicioJob: " << inicioJob << endl;
                currantSwitchs = unsupervisedMagazine.size();
            } else {
                // cout << "Colocando o job: " << originalJobs[s[jL]].indexJob << " no periodo sem supervisao" << " inicioJob: " << inicioJob << endl;
                currantSwitchs = 0;
                for (auto it = currentToolSet.tools.begin(); it != currentToolSet.tools.end(); ++it) unsupervisedMagazine.insert(*it);
            }
            cout << endl;
        }

        // estou saindo do periodo sem supervisao
        if (((inicioJob % DAY) >= unsupervised) && (fimJob % DAY) < unsupervised) {
            cout << "saindo do periodo sem supervisao: " << originalJobs[s[jL]].indexJob << " inicioJob: " << inicioJob << endl << endl;
            leavingUnsupervised = true;
            for (size_t t = 0; t < magazineL.size(); ++t) {
                magazineL[t] = false;
            }
            for (auto it = unsupervisedMagazine.begin(); it != unsupervisedMagazine.end(); ++it) {
                magazineL[*it] = true;
            }
            magazineCL = magazineL;
        }

        if (((inicioJob % DAY) >= unsupervised && (currantSwitchs > 0))) {  // verificar se o job excede o horizonte de planejamento
            inicioJob += DAY - (inicioJob % DAY);
            fimJob = inicioJob + originalJobs[s[jL]].processingTime;
            leavingUnsupervised = true;
        }

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

        const auto& job = originalJobs[s[jL]];
        bool isGrouped = job.isGrouped;
        int loops = isGrouped ? 2 : 1;

        Interval interval = {startTMP, endTMP};
        unsupervisedIntervals[std::make_tuple(job.indexJob, job.indexOperation)] = interval;

        cout << "Job: " << job.indexJob << ", Operation: " << job.indexOperation << ", Start: " << startTMP << ", End: " << endTMP << " unsuperviedJobsCount: " << unsuperviedJobsCount << " leavingUnsupervised: " << leavingUnsupervised << endl;

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
            for (auto t : unsupervisedMagazine) {
                solutionReportFile << t << ",";
            }
            solutionReportFile << "\n";
        };

        if (leavingUnsupervised) {
            // iterate from unsupervisedJobStartIndex to jL
            for (int k = unsupervisedJobStartIndex; k <= jL; ++k) {
                const auto& curJob = originalJobs[s[k]];
                bool curIsGrouped = curJob.isGrouped;
                int curLoops = curIsGrouped ? 2 : 1;
                
                for (int i = 0; i < curLoops; ++i) {
                    int curStart = unsupervisedIntervals[std::make_tuple(curJob.indexJob, i)].start;
                    int curEnd = unsupervisedIntervals[std::make_tuple(curJob.indexJob, i)].end;
                    writeJobDetailsUnsupervised(curJob.indexJob, curStart, curEnd, i, curJob.priority);
                }
            }
            leavingUnsupervised = false;
            unsupervisedIntervals.clear();
            unsupervisedMagazine.clear();
            unsuperviedJobsCount = 0;
        } else if(unsuperviedJobsCount == 0) {
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
