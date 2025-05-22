#include <algorithm>
#include <atomic>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <thread>
#include <vector>

#include "headers/GlobalVars.h"
#include "headers/SSP.h"

#ifdef DEBUG
#include <fmt/core.h>
#include <fmt/ranges.h>
#endif

using namespace std;

double SSP::evaluate(solSSP& s) {
    int fineshedJobsCountTotal = 0;
    int switchsTotal = 0;
    int switchsInstancesTotal = 0;
    int unfineshedPriorityCountTotal = 0;
    int totalUnfineshed = numberJobsUngrouped;

    vector<vector<int>> machines = splitSolutionIntoMachines(s.sol, numberMachines);
    for (size_t i = 0; i < machines.size(); i++) {
        auto [fineshedJobsCount, switchs, switchsInstances, unfineshedPriorityCount] = KTNS(machines[i]);
        fineshedJobsCountTotal += fineshedJobsCount;
        switchsTotal += switchs;
        switchsInstancesTotal += switchsInstances;
        unfineshedPriorityCountTotal += unfineshedPriorityCount;
        totalUnfineshed -= fineshedJobsCount;

        // cout << "Machine: " << i << endl;
        // cout << "fineshedJobsCount: " << fineshedJobsCount << endl;
        // cout << "switchs: " << switchs << endl;
        // cout << "switchsInstances: " << switchsInstances << endl;
        // cout << "unfineshedPriorityCount: " << unfineshedPriorityCount << endl;
        // cout << endl;
    }

    // cout << "fineshedJobsCountTotal: " << fineshedJobsCountTotal << endl;
    // cout << "switchsTotal: " << switchsTotal << endl;
    // cout << "switchsInstancesTotal: " << switchsInstancesTotal << endl;
    // cout << "unfineshedPriorityCountTotal: " << unfineshedPriorityCountTotal << endl;
    // cout << "totalUnfineshed: " << totalUnfineshed << endl;
    // cout << "Conta: " << -((PROFITYFINISHED * fineshedJobsCountTotal) - (COSTSWITCH * switchsTotal) - (COSTSWITCHINSTANCE * switchsInstancesTotal) - (COSTPRIORITY * unfineshedPriorityCountTotal)) << endl;
    // cout << endl << endl << endl;

    return -((PROFITYFINISHED * fineshedJobsCountTotal) - (COSTSWITCH * switchsTotal) - (COSTSWITCHINSTANCE * switchsInstancesTotal) - (COSTPRIORITY * unfineshedPriorityCountTotal));
}

tuple<int, int, int, int> SSP::KTNS(vector<int> s) {
    int currNumberJobs = s.size();
    vector<bool> magazineL(numberTools, true);
    unsigned int switchs = 0;
    int jL;

    int switchsInstances = 0;
    int currantSwitchs = 0;
    int fineshedJobsCount = 0;
    int unfineshedPriorityCount = 0;

    for (int i = 0; i < currNumberJobs; ++i) {
        if (originalJobs[s[i]].priority) {
            unfineshedPriorityCount += originalJobs[s[i]].isGrouped ? 2 : 1;
        }
    }

    int inicioJob = 0;
    int fimJob = 0;
    int isFirstJobOfMachine = 1;
    int extendedPlaningHorizon = (planingHorizon * 1) * DAY;

    for (jL = 0; jL < currNumberJobs; ++jL) {
        // ---------------------------------------------------------------------------
        // switchs
        // ---------------------------------------------------------------------------

        currantSwitchs = 0;
        vector<bool> magazineCL(numberTools);
        int left = jL;
        int cmL = 0;

        while ((cmL < capacityMagazine) && (left < currNumberJobs)) {
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

        fimJob = inicioJob + originalJobs[s[jL]].processingTime;

        if (((inicioJob % DAY) >= unsupervised && (currantSwitchs > 0)) ||                                               // verificar se estou em um periodo semsupervisao e houve troca de ferramenta
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
    }

    return {fineshedJobsCount, switchs, switchsInstances, unfineshedPriorityCount};
}






// #include <algorithm>
// #include <atomic>
// #include <fstream>
// #include <iostream>
// #include <sstream>
// #include <string>
// #include <thread>
// #include <vector>

// #include "headers/GlobalVars.h"
// #include "headers/SSP.h"

// #ifdef DEBUG
// #include <fmt/core.h>
// #include <fmt/ranges.h>
// #endif

// using namespace std;

// double SSP::evaluate(solSSP& s) {
//     vector<bool> magazineL(numberTools, true);
//     unsigned int switchs = 0;
//     int jL;

//     int switchsInstances = 0;
//     int currantSwitchs = 0;
//     int fineshedJobsCount = 0;
//     int unfineshedPriorityCount = numberOfPriorityJobs;

//     int inicioJob = 0;
//     int fimJob = 0;
//     int isFirstJobOfMachine = 1;
//     int extendedPlaningHorizon = (planingHorizon * numberMachines) * DAY;

//     for (jL = 0; jL < numberJobs; ++jL) {
//         // ---------------------------------------------------------------------------
//         // switchs
//         // ---------------------------------------------------------------------------

//         currantSwitchs = 0;
//         vector<bool> magazineCL(numberTools);
//         int left = jL;
//         int cmL = 0;

//         while ((cmL < capacityMagazine) && (left < numberJobs)) {
//             for (auto it = originalJobs[s.sol[left]].toolSetNormalized.tools.begin(); ((it != originalJobs[s.sol[left]].toolSetNormalized.tools.end()) && (cmL < capacityMagazine)); ++it) {
//                 if ((magazineL[*it]) && (!magazineCL[*it])) {
//                     magazineCL[*it] = true;
//                     ++cmL;
//                 } else if ((jL == left) && (!magazineCL[*it])) {
//                     magazineCL[*it] = true;
//                     ++cmL;
//                     ++currantSwitchs;
//                 }
//             }
//             ++left;
//         }

//         for (int t = 0; ((t < numberTools) && (cmL < capacityMagazine)); t++) {
//             if ((magazineL[t]) && (!magazineCL[t])) {
//                 magazineCL[t] = true;
//                 ++cmL;
//             }
//         }
//         magazineL = magazineCL;

//         // ---------------------------------------------------------------------------
//         // TIME VERIFICATIONS
//         // ---------------------------------------------------------------------------

//         fimJob = inicioJob + originalJobs[s.sol[jL]].processingTime;

//         if (((inicioJob % DAY) >= unsupervised && (currantSwitchs > 0)) ||                                               // verificar se estou em um periodo semsupervisao e houve troca de ferramenta
//             (inicioJob % (planingHorizon * DAY) + (originalJobs[s.sol[jL]].processingTime) > (planingHorizon * DAY))) {  // verificar se o job excede o horizonte de planejamento unico (iria extender de uma maquina para outra)
//             inicioJob += DAY - (inicioJob % DAY);
//             fimJob = inicioJob + originalJobs[s.sol[jL]].processingTime;
//         }

//         if (fimJob > extendedPlaningHorizon) break;

//         if ((inicioJob % (planingHorizon * DAY) == 0))
//             isFirstJobOfMachine = 1;
//         else
//             isFirstJobOfMachine = 0;

//         inicioJob = fimJob;

//         // ---------------------------------------------------------------------------
//         // COSTS
//         // ---------------------------------------------------------------------------

//         if (isFirstJobOfMachine) currantSwitchs = 0;
//         switchs += currantSwitchs;
//         if (currantSwitchs > 0) ++switchsInstances;
//         fineshedJobsCount += originalJobs[s.sol[jL]].isGrouped ? 2 : 1;
//         if (originalJobs[s.sol[jL]].priority) unfineshedPriorityCount -= originalJobs[s.sol[jL]].isGrouped ? 2 : 1;
//     }

//     int cost = (PROFITYFINISHED * fineshedJobsCount) - (COSTSWITCH * switchs) - (COSTSWITCHINSTANCE * switchsInstances) - (COSTPRIORITY * unfineshedPriorityCount);

//     return (-1) * cost;
// }