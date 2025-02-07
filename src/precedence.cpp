#include "headers/SSP.h"

#ifdef DEBUG
#include <fmt/core.h>
#include <fmt/ranges.h>
#endif

solSSP SSP::ajustFinalSolution(solSSP sol) {
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
    int logicalMachine = 0;

    sol.releaseDates = vector<int>(numberJobs, INT_MAX);
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
        cout << "Index: " << s[jL] << " Job: " << originalJobs[s[jL]].indexJob << " Operaiton: " << originalJobs[s[jL]].indexOperation << " Start: " << startTMP << " End: " << endTMP << endl;

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

    cout << "" << endl;
    //print each  solution and the release and due dates
    for (size_t i = 0; i < solAjusted.sol.size(); i++){
        int jobNessaPoss = solAjusted.sol[i];
        cout << "Index: " << jobNessaPoss << " job: " << originalJobs[jobNessaPoss].indexJob << " Operation: " << originalJobs[jobNessaPoss].indexOperation << " release: " << sol.releaseDates[originalJobs[jobNessaPoss].indexJob] << " due: " << sol.dueDates[originalJobs[jobNessaPoss].indexJob] << endl;
    }

    int cost = (PROFITYFINISHED * fineshedJobsCount) - (COSTSWITCH * switchs) - (COSTSWITCHINSTANCE * switchsInstances) - (COSTPRIORITY * unfineshedPriorityCount);

    return solAjusted;
}