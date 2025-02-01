#include "headers/SSP.h"

#ifdef DEBUG
#include <fmt/core.h>
#include <fmt/ranges.h>
#endif

solSSP SSP::ajustFinalSolution(solSSP sol) {
    solSSP semLugars;
    solSSP newSol = sol;

    newSol.jobsTime.clear();
    newSol.sol.clear();
    
    for (size_t i = 0; i < sol.jobsTime.size(); i++){
        int jobIndex = sol.jobsTime[i].job.indexJob;
        int indexOperation = sol.jobsTime[i].job.indexOperation;
        cout << fmt::format("Job: {0}, Operation: {1}, Machine: {2}, Start: {3}, End: {4}, Release: {5}, Due: {6}", jobIndex, indexOperation, sol.jobsTime[i].machine, sol.jobsTime[i].start, sol.jobsTime[i].end, sol.releaseDates[jobIndex], sol.dueDates[jobIndex]) << endl;
        if(indexOperation == 0){
            if(sol.jobsTime[i].end > sol.dueDates[jobIndex]){
                cout << "é 0 e nao estou fazendo na hora certa" << endl;
                semLugars.jobsTime.push_back(sol.jobsTime[i]);
                semLugars.sol.push_back(sol.sol[i]);
            }
            else{
                newSol.jobsTime.push_back(sol.jobsTime[i]);
                newSol.sol.push_back(sol.sol[i]);
            }
        }
        if(indexOperation == 1){
            if(sol.jobsTime[i].start < sol.releaseDates[jobIndex]){
                cout << "é 1 e nao estou fazendo na hora certa" << endl;
                semLugars.jobsTime.push_back(sol.jobsTime[i]);
                semLugars.sol.push_back(sol.sol[i]);
            }
            else{
                newSol.jobsTime.push_back(sol.jobsTime[i]);
                newSol.sol.push_back(sol.sol[i]);
            }
        }

        for (size_t j = 0; j < semLugars.sol.size(); j++){
            int jobIndexSemLugar = semLugars.jobsTime[j].job.indexJob;
            int indexOperationSemLugar = semLugars.jobsTime[j].job.indexOperation;

            if(indexOperationSemLugar == 0){
                if(sol.jobsTime[i].end <= sol.dueDates[jobIndexSemLugar]){
                    cout << "é 0 e estou fazendo na hora certa" << endl;
                    newSol.jobsTime.push_back(semLugars.jobsTime[j]);
                    newSol.sol.push_back(semLugars.sol[j]);
                    semLugars.jobsTime.erase(semLugars.jobsTime.begin() + j);
                    semLugars.sol.erase(semLugars.sol.begin() + j);
                }
            }
            if(indexOperationSemLugar == 1){
                if(sol.jobsTime[i].start >= sol.releaseDates[jobIndexSemLugar]){
                    cout << "é 1 e estou fazendo na hora certa" << endl;
                    newSol.jobsTime.push_back(semLugars.jobsTime[j]);
                    newSol.sol.push_back(semLugars.sol[j]);
                    semLugars.jobsTime.erase(semLugars.jobsTime.begin() + j);
                    semLugars.sol.erase(semLugars.sol.begin() + j);
                }
            }
        }
        cout << "\n" << endl;
    }

    //print the new solution length
    cout << "Old solution length: " << sol.sol.size() << endl;
    cout << "New solution length: " << newSol.sol.size() << endl;
    cout << "Sem Lugar length: " << semLugars.sol.size() << endl;

    return newSol;
}