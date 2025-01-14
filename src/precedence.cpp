#include "headers/SSP.h"

void SSP::setupPermutations() {
    for (int i = 0; i < numberJobs; i++) {
        for (int j = i + 1; j < numberJobs; j++) {
            switchPermutation.push_back(std::make_tuple(i, j));
        }
    }
}

solSSP SSP::ajustFinalSolution(solSSP sol) {
    cout << "TESTE" << endl;
    return sol;

    vector<int> s = sol.sol;

    fstream solutionReportFile;
    solutionReportFile.open("", ios::out);

    vector<bool> magazineL(numberToolsReal, true);
    unsigned int switchs = 0;
    int numberJobsSol = s.size();
    int jL;

    int switchsInstances = 0;
    int currantSwitchs = 0;
    int fineshedJobsCount = 0;
    int unfineshedPriorityCount = numberOfPriorityJobs;

    int inicioJob = 0;
    int fimJob = 0;
    int extendedPlaningHorizon = (planingHorizon * numberMachines) * DAY;
    int isFirstJobOfMachine = 1;

    int logicalMachine = 0;

    for (jL = 0; jL < numberJobsSol; ++jL) {
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

        if (originalJobs[s[jL]].indexJob == 1012 && originalJobs[s[jL]].indexOperation == 0) {
            int countTrue = 0;
            for (int i = 0; i < numberToolsReal; i++) {
                if (magazineCL[i]) {
                    countTrue++;
                }
            }
            cout << countTrue << endl;
            cout << numberToolsReal << endl;
        }

        fimJob = inicioJob + originalJobs[s[jL]].processingTime;

        if (((inicioJob % DAY) >= unsupervised && (currantSwitchs > 0)) ||                                           // verificar se estou em um periodo sem supervisao e houve troca de ferramenta
            (inicioJob % (planingHorizon * DAY) + (originalJobs[s[jL]].processingTime) > (planingHorizon * DAY))) {  // verificar se o job excede o horizonte de planejamento unico (iria extender de uma maquina para outra)
            inicioJob += DAY - (inicioJob % DAY);
            fimJob = inicioJob + originalJobs[s[jL]].processingTime;
        }

        if ((inicioJob % (planingHorizon * DAY) == 0))
            isFirstJobOfMachine = 1;
        else
            isFirstJobOfMachine = 0;

        if (fimJob > extendedPlaningHorizon) break;

        inicioJob = fimJob;

        // ---------------------------------------------------------------------------
        // COSTS
        // ---------------------------------------------------------------------------

        if (isFirstJobOfMachine) currantSwitchs = 0;
        switchs += currantSwitchs;
        if (currantSwitchs > 0) ++switchsInstances;

        fineshedJobsCount += originalJobs[s[jL]].isGrouped ? 2 : 1;
        if (originalJobs[s[jL]].priority) unfineshedPriorityCount -= originalJobs[s[jL]].isGrouped ? 2 : 1;

        // ---------------------------------------------------------------------------
        // PRINTS
        // ---------------------------------------------------------------------------

        int startTMP = (fimJob - originalJobs[s[jL]].processingTime) % (planingHorizon * DAY);
        int endTMP = ((fimJob - 1) % (planingHorizon * DAY)) + 1;

        if (startTMP % (planingHorizon * DAY) == 0) {
            solutionReportFile << "Machine: " << logicalMachine << std::endl;
            ++logicalMachine;
        }

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

    int cost = (PROFITYFINISHED * fineshedJobsCount) - (COSTSWITCH * switchs) - (COSTSWITCHINSTANCE * switchsInstances) - (COSTPRIORITY * unfineshedPriorityCount);

    solutionReportFile << "END;";
    solutionReportFile << fineshedJobsCount << ";";
    solutionReportFile << switchs << ";";
    solutionReportFile << switchsInstances << ";";
    solutionReportFile << unfineshedPriorityCount << ";";
    solutionReportFile << cost << "\n";

    solutionReportFile << "TIME;" << time << endl;

    solutionReportFile.close();

    // cout << "-----------INICIO-----------" << endl;
    // for (size_t i = 0; i < solution.sol.size(); ++i) {
    //     int index = solution.sol[i];
    //     Job job = originalJobs[index];
    //     cout << i << ": (" << job.indexJob << " " << job.indexOperation << ") = " << solution.releaseDates[originalJobs[solution.sol[i]].indexJob] << endl;
    // }
    // cout << "-----------FIM-----------" << endl;

    return sol;
}