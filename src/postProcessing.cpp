#include "headers/SSP.h"

#ifdef DEBUG
#include <fmt/core.h>
#include <fmt/ranges.h>
#endif

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

solSSP SSP::postProcessDifferent(solSSP& sol) {
    vector<int> s = sol.sol;
    solSSP solAjusted;
    // solAjusted.sol = sol.sol;

    vector<bool> magazineL(numberToolsReal, true);
    unsigned int switchs = 0;
    int numberJobsSol = s.size();
    int jL;
    int currantJob = 0;

    int switchsInstances = 0;
    int currantSwitchs = 0;
    int fineshedJobsCount = 0;
    int unfineshedPriorityCount = numberOfPriorityJobs;

    int inicioJob = 0;
    int fimJob = 0;
    int extendedPlaningHorizon = (planingHorizon * numberMachines) * DAY;
    int isFirstJobOfMachine = 1;

    sol.releaseDates = vector<int>(numberJobs, -INT_MAX);
    sol.dueDates = vector<int>(numberJobs, INT_MAX);

    while(!s.empty()){
    // for (jL = 0; jL < numberJobsSol; ++jL) {

        // ---------------------------------------------------------------------------
        // switchs
        // ---------------------------------------------------------------------------
        jL = currantJob;
        numberJobsSol = s.size();

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

        // ---------------------------------------------------------------------------
        // TIME VERIFICATIONS
        // ---------------------------------------------------------------------------
        int fimJobBKP = fimJob;
        int inicioJobBKP = inicioJob;

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
        // VERIFICAR DOE E RELEASE
        // ---------------------------------------------------------------------------

        int startTMP = (fimJob - originalJobs[s[jL]].processingTime) % (planingHorizon * DAY);
        int endTMP = ((fimJob - 1) % (planingHorizon * DAY)) + 1;

        //verficar due e release
        if(originalJobs[s[jL]].indexOperation == 0){
            if(endTMP > sol.dueDates[originalJobs[s[jL]].indexJob]){
                inicioJob = inicioJobBKP;
                fimJob = fimJobBKP;
                currantJob++;
                continue;
            }
        }
        if(originalJobs[s[jL]].indexOperation == 1){
            if(startTMP < sol.releaseDates[originalJobs[s[jL]].indexJob]){
                inicioJob = inicioJobBKP;
                fimJob = fimJobBKP;
                currantJob++;
                continue;
            }
        }

        //setar release e due
        if(originalJobs[s[jL]].indexOperation == 0){
            sol.releaseDates[originalJobs[s[jL]].indexJob] = endTMP;
        }
        else{
            sol.dueDates[originalJobs[s[jL]].indexJob] = startTMP;
        }

        solAjusted.sol.push_back(s[jL]);

        // s.erase(s.begin() + jL);
        // currantJob = 0;
        // cout << "Index: " << s[jL] << " Job: " << originalJobs[s[jL]].indexJob << " Operaiton: " << originalJobs[s[jL]].indexOperation << " Start: " << startTMP << " End: " << endTMP << endl;

        s.erase(s.begin() + jL);
        currantJob = 0;
        //oficializar a troca de ferramentas
        magazineL = magazineCL;

        // ---------------------------------------------------------------------------
        // COSTS
        // ---------------------------------------------------------------------------

        if (isFirstJobOfMachine) currantSwitchs = 0;
        switchs += currantSwitchs;
        if (currantSwitchs > 0) ++switchsInstances;

        fineshedJobsCount += originalJobs[s[jL]].isGrouped ? 2 : 1;
        if (originalJobs[s[jL]].priority) unfineshedPriorityCount -= originalJobs[s[jL]].isGrouped ? 2 : 1;

    }

    // cout << "" << endl;
    //print each  solution and the release and due dates
    // for (size_t i = 0; i < solAjusted.sol.size(); i++){
        // int jobNessaPoss = solAjusted.sol[i];
        // cout << "Index: " << jobNessaPoss << " job: " << originalJobs[jobNessaPoss].indexJob << " Operation: " << originalJobs[jobNessaPoss].indexOperation << " release: " << sol.releaseDates[originalJobs[jobNessaPoss].indexJob] << " due: " << sol.dueDates[originalJobs[jobNessaPoss].indexJob] << endl;
    // }

    // int cost = (PROFITYFINISHED * fineshedJobsCount) - (COSTSWITCH * switchs) - (COSTSWITCHINSTANCE * switchsInstances) - (COSTPRIORITY * unfineshedPriorityCount);

    return solAjusted;
}

double SSP::evaluateReportKTNS(solSSP& solution, string filenameJobs, string filenameTools, fstream& solutionReportFile) {
    solutionReportFile << filenameJobs << ";" << filenameTools << endl;
    solutionReportFile << planingHorizon << ";" << unsupervised << ";" << DAY << endl;

    int fineshedJobsCountTotal = 0;
    int switchsTotal = 0;
    int switchsInstancesTotal = 0;
    int unfineshedPriorityCountTotal = 0;
    int totalUnfineshed = numberJobsUngrouped;

    vector<vector<int>> machines = splitSolutionIntoMachines(solution.sol, numberMachines);
    for (size_t i = 0; i < machines.size(); i++) {
        auto [fineshedJobsCount, switchs, switchsInstances, unfineshedPriorityCount] = evaluateReportKTNSMachine(machines[i], solutionReportFile, i);
        fineshedJobsCountTotal += fineshedJobsCount;
        switchsTotal += switchs;
        switchsInstancesTotal += switchsInstances;
        unfineshedPriorityCountTotal += unfineshedPriorityCount;
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

    // vector<int> s = solution.sol;

    // vector<bool> magazineL(numberToolsReal, true);
    // unsigned int switchs = 0;
    // int numberJobsSol = s.size();
    // int jL;

    // int switchsInstances = 0;
    // int currantSwitchs = 0;
    // int fineshedJobsCount = 0;
    // int unfineshedPriorityCount = numberOfPriorityJobs;

    // int inicioJob = 0;
    // int fimJob = 0;
    // int extendedPlaningHorizon = (planingHorizon * numberMachines) * DAY;
    // int isFirstJobOfMachine = 1;

    // int logicalMachine = 0;
    // solutionReportFile << filenameJobs << ";" << filenameTools << endl;
    // solutionReportFile << planingHorizon << ";" << unsupervised << ";" << DAY << endl;

    // solution.releaseDates = vector<int>(numberJobs, -INT_MAX);
    // solution.dueDates = vector<int>(numberJobs, INT_MAX);

    // for (jL = 0; jL < numberJobsSol; ++jL) {
    //     // ---------------------------------------------------------------------------
    //     // switchs
    //     // ---------------------------------------------------------------------------

    //     currantSwitchs = 0;
    //     vector<bool> magazineCL(numberToolsReal);
    //     int left = jL;
    //     int cmL = 0;

    //     while ((cmL < capacityMagazine) && (left < numberJobsSol)) {
    //         for (auto it = originalJobs[s[left]].toolSet.tools.begin(); ((it != originalJobs[s[left]].toolSet.tools.end()) && (cmL < capacityMagazine)); ++it) {
    //             if ((magazineL[*it]) && (!magazineCL[*it])) {
    //                 magazineCL[*it] = true;
    //                 ++cmL;
    //             } else if ((jL == left) && (!magazineCL[*it])) {
    //                 magazineCL[*it] = true;
    //                 ++cmL;
    //                 ++currantSwitchs;
    //             }
    //         }
    //         ++left;
    //     }

    //     for (int t = 0; ((t < numberToolsReal) && (cmL < capacityMagazine)); t++) {
    //         if ((magazineL[t]) && (!magazineCL[t])) {
    //             magazineCL[t] = true;
    //             ++cmL;
    //         }
    //     }
    //     magazineL = magazineCL;

    //     // ---------------------------------------------------------------------------
    //     // TIME VERIFICATIONS
    //     // ---------------------------------------------------------------------------

    //     fimJob = inicioJob + originalJobs[s[jL]].processingTime;

    //     if (((inicioJob % DAY) >= unsupervised && (currantSwitchs > 0)) ||                                           // verificar se estou em um periodo sem supervisao e houve troca de ferramenta
    //         (inicioJob % (planingHorizon * DAY) + (originalJobs[s[jL]].processingTime) > (planingHorizon * DAY))) {  // verificar se o job excede o horizonte de planejamento unico (iria extender de uma maquina para outra)
    //         inicioJob += DAY - (inicioJob % DAY);
    //         fimJob = inicioJob + originalJobs[s[jL]].processingTime;
    //     }

    //     if ((inicioJob % (planingHorizon * DAY) == 0))
    //         isFirstJobOfMachine = 1;
    //     else
    //         isFirstJobOfMachine = 0;

    //     if (fimJob > extendedPlaningHorizon) break;

    //     inicioJob = fimJob;

    //     // ---------------------------------------------------------------------------
    //     // COSTS
    //     // ---------------------------------------------------------------------------

    //     if (isFirstJobOfMachine) currantSwitchs = 0;
    //     switchs += currantSwitchs;
    //     if (currantSwitchs > 0) ++switchsInstances;

    //     fineshedJobsCount += originalJobs[s[jL]].isGrouped ? 2 : 1;
    //     if (originalJobs[s[jL]].priority) unfineshedPriorityCount -= originalJobs[s[jL]].isGrouped ? 2 : 1;

    //     // ---------------------------------------------------------------------------
    //     // PRINTS
    //     // ---------------------------------------------------------------------------

    //     int startTMP = (fimJob - originalJobs[s[jL]].processingTime) % (planingHorizon * DAY);
    //     int endTMP = ((fimJob - 1) % (planingHorizon * DAY)) + 1;

    //     if (startTMP % (planingHorizon * DAY) == 0) {
    //         solutionReportFile << "Machine: " << logicalMachine << std::endl;
    //         ++logicalMachine;
    //     }

    //     const auto &job = originalJobs[s[jL]];
    //     bool isGrouped = job.isGrouped;
    //     int loops = isGrouped ? 2 : 1;

    //     auto writeJobDetails = [&](int start, int end, int operation) {
    //         solutionReportFile << job.indexJob << ";" << operation << ";" << start << ";" << end << ";" << job.priority << ";";
    //         for (size_t t = 0; t < magazineCL.size(); ++t) {
    //             if (magazineCL[t]) {
    //                 solutionReportFile << t << ",";
    //             }
    //         }
    //         solutionReportFile << "\n";
    //     };

    //     for (int i = 0; i < loops; ++i) {
    //         if (isGrouped && i == 0) {
    //             writeJobDetails(startTMP, startTMP + job.processingTimes[0], 0);
    //         } else if (isGrouped && i == 1) {
    //             writeJobDetails(startTMP + job.processingTimes[0], endTMP, 1);
    //         } else {
    //             writeJobDetails(startTMP, endTMP, job.indexOperation);
    //         }
    //     }
    // }

    // int cost = (PROFITYFINISHED * fineshedJobsCount) - (COSTSWITCH * switchs) - (COSTSWITCHINSTANCE * switchsInstances) - (COSTPRIORITY * unfineshedPriorityCount);

    // solutionReportFile << "END" << endl;
    // solutionReportFile << "fineshedJobsCount: " << fineshedJobsCount << endl;
    // solutionReportFile << "switchs: " << switchs << endl;
    // solutionReportFile << "switchsInstances: " << switchsInstances << endl;
    // solutionReportFile << "unfineshedPriorityCount: " << unfineshedPriorityCount << endl;

    // return cost;
}

tuple<int, int, int, int>  SSP::evaluateReportKTNSMachine(vector<int> s, fstream& solutionReportFile, int machine) {
    vector<bool> magazineL(numberToolsReal, true);
    unsigned int switchs = 0;
    int numberJobsSol = s.size();
    int jL;

    int switchsInstances = 0;
    int currantSwitchs = 0;
    int fineshedJobsCount = 0;
    int unfineshedPriorityCount = 0;

    for (int i = 0; i < numberJobsSol; ++i) {
        if (originalJobs[s[i]].priority) {
            unfineshedPriorityCount += originalJobs[s[i]].isGrouped ? 2 : 1;
        }
    }

    int inicioJob = 0;
    int fimJob = 0;
    int extendedPlaningHorizon = (planingHorizon * 1) * DAY;
    int isFirstJobOfMachine = 1;

    solutionReportFile << "Machine: " << machine << std::endl;

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

    return {fineshedJobsCount, switchs, switchsInstances, unfineshedPriorityCount};
}

