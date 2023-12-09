#include <vector>
#include <iostream>
#include <atomic>
#include <thread>
#include <fstream>
#include <sstream>
#include <string>
#include <algorithm> 
#include <fstream>

#include "headers/GlobalVars.h"
#include "headers/loadData.h"
#include "headers/cost.h"

using namespace std;

// ------------------------------------------------------------------------------------------------------------------------------------------------------
// KTNS
// ------------------------------------------------------------------------------------------------------------------------------------------------------

unsigned int KTNSReport(vector<int> s, fstream& solutionReportFile){
    // Variaveis
	vector<bool> magazineL(numberTools, true);	
	unsigned int switchs = 0; 
	int jL;

	int switchsInstances = 0; 		// Conta quantas trocas de instancia foram feitas, quando pelo menos uma troca de ferramenta foi trocada do magazine
	int currantSwitchs = 0; 		// Conta quantas trocas de ferramenta foram feitas, no job atual
	int currantProcessingTime = 0; 

	int inicioJob = 0; 				// Conta quantas horas ja foram usadas no dia atual  		       
	int fimJob = 0; 				// Conta quantos dias ja foram usados no horizonte de planejamento 

	int fineshedPriorityCount = 0;
	int unfineshedPriorityCount = 0;

	int countCreated = 0;
	int expanded = true;

	int numberJobsSol = s.size();

	// Percorre todas as ferramentas
	for(jL= 0; jL < numberJobsSol; ++jL){
		
		currantSwitchs = 0;

		vector<bool> magazineCL(numberTools);		
		int left = jL;
		int cmL = 0;

		while((cmL < capacityMagazine) && (left < numberJobsSol)){
			for (auto it=originalToolSets[superToolSet[superJobs[s[left]].indexSuperToolSet].indexOriginalToolSet].tools.begin(); ((it!=originalToolSets[superToolSet[superJobs[s[left]].indexSuperToolSet].indexOriginalToolSet].tools.end()) && (cmL < capacityMagazine)); ++it){
				
				if((magazineL[*it]) && (!magazineCL[*it])){
					magazineCL[*it] = true;
					++cmL;
				}else if((jL == left) && (!magazineCL[*it])){
					magazineCL[*it] = true;
					++cmL;
					++currantSwitchs;
				}
			}
			++left;
		}

		
		for(int t=0; ((t < numberTools) && (cmL < capacityMagazine)); t++){
			if((magazineL[t]) && (!magazineCL[t])){
				magazineCL[t] = true;
				++cmL;			
			}
		}

		magazineL = magazineCL;

		if (jL == 0) currantSwitchs = capacityMagazine;

		// ---------------------------------------------------------------------------

		currantProcessingTime = superJobs[s[jL]].processingTimeSum;
		fimJob = inicioJob + currantProcessingTime;

		// verifica se a hora é sem supervisao e se houve troca de ferramenta
		if ((inicioJob % TIMESCALE >= unsupervised) && (currantSwitchs > 0)){
			
			//Verificar de consigo acabar essa tarefa antes de exceder o horizonte de planejamento
			if(fimJob + (TIMESCALE - (fimJob % TIMESCALE)) >= planingHorizon * TIMESCALE){
				
				if(expanded == false){
					for(unsigned int v = jL+1; v < numberJobsSol; ++v){
						unfineshedPriorityCount += superJobs[s[v]].prioritySum;
					}

					int lastJob = s[s.size()-1];
					s.clear();

					for (auto it = superJobs[lastJob].originalJobs.begin(); it != superJobs[lastJob].originalJobs.end(); ++it){
						SuperJob SuperJobTmp;

						SuperJobTmp.indexSuperToolSet = superJobs[lastJob].indexSuperToolSet;
						SuperJobTmp.processingTimeSum = originalJobs[*it].processingTime;
						SuperJobTmp.prioritySum = originalJobs[*it].priority;
						SuperJobTmp.originalJobs.push_back(*it);

						superJobs.push_back(SuperJobTmp);
						countCreated++;

						s.push_back(superJobs.size()-1);
					}
					expanded = true;
					numberJobsSol = s.size();
					jL = 0;

					continue;
				}

				else{
					// Contar quantar tarefas prioritarias faltaram
					for(unsigned int v = jL; v < numberJobsSol; ++v){
						unfineshedPriorityCount += superJobs[s[v]].prioritySum;
					}
					// Pode sair do loop 
					break;
				}
				
			}
			else{
				inicioJob += TIMESCALE - (inicioJob % TIMESCALE);
				fimJob = inicioJob + currantProcessingTime;
			}

		}

		//Tarefa vai vazar para o proximo dia
		if ((inicioJob % TIMESCALE) + currantProcessingTime >= TIMESCALE){
			
			//Verificar de consigo acabar essa tarefa antes de exceder o horizonte de planejamento
			if(fimJob >= planingHorizon * TIMESCALE){
				
				if(expanded == false){
					for(unsigned int v = jL+1; v < numberJobsSol; ++v){
						unfineshedPriorityCount += superJobs[s[v]].prioritySum;
					}

					int lastJob = s[s.size()-1];
					s.clear();

					for (auto it = superJobs[lastJob].originalJobs.begin(); it != superJobs[lastJob].originalJobs.end(); ++it){
						SuperJob SuperJobTmp;

						SuperJobTmp.indexSuperToolSet = superJobs[lastJob].indexSuperToolSet;
						SuperJobTmp.processingTimeSum = originalJobs[*it].processingTime;
						SuperJobTmp.prioritySum = originalJobs[*it].priority;
						SuperJobTmp.originalJobs.push_back(*it);

						superJobs.push_back(SuperJobTmp);
						countCreated++;

						s.push_back(superJobs.size()-1);
					}
					expanded = true;
					numberJobsSol = s.size();
					jL = 0;

					continue;
				}
				else{
					// Contar quantar tarefas prioritarias faltaram
					for(unsigned int v = jL; v < numberJobsSol; ++v){
						unfineshedPriorityCount += superJobs[s[v]].prioritySum;
					}
					// Pode sair do loop 
					break;
					
				}
				
			}
			
		}
		
		inicioJob = fimJob;

		// ---------------------------------------------------------------------------
		
		int inicioTmp = fimJob - currantProcessingTime;
		int fimTmp = fimJob;

		for (auto it = superJobs[s[jL]].originalJobs.begin(); it != superJobs[s[jL]].originalJobs.end(); ++it){

			fimTmp = inicioTmp + originalJobs[*it].processingTime;

			solutionReportFile << originalJobs[*it].indexJob << ";";
			solutionReportFile << originalJobs[*it].indexOperation << ";";
			solutionReportFile << inicioTmp << ";";
			solutionReportFile << fimTmp << ";";
			solutionReportFile << originalJobs[*it].priority << ";";

			for(unsigned int t = 0; t < magazineCL.size(); ++t){
				if(magazineCL[t]){ 
					solutionReportFile << t << ",";
				}
			}
			solutionReportFile << "\n";

			inicioTmp = fimTmp;

		}
		
		// ---------------------------------------------------------------------------

		switchs += currantSwitchs;
		if(currantSwitchs > 0) ++switchsInstances;
		fineshedPriorityCount += superJobs[s[jL]].prioritySum;

		// ---------------------------------------------------------------------------

	}
	superJobs.erase(superJobs.end() - countCreated, superJobs.end());


	int cost = (PROFITYPRIORITY * fineshedPriorityCount) - (COSTSWITCH * switchs) - (COSTSWITCHINSTANCE * switchsInstances) - (COSTPRIORITY * unfineshedPriorityCount);
	solutionReportFile << "END;";
	solutionReportFile << fineshedPriorityCount << ";";
	solutionReportFile << switchs << ";";
	solutionReportFile << switchsInstances << ";";
	solutionReportFile << unfineshedPriorityCount << ";";
	solutionReportFile << cost << "\n";

	return cost;
}

unsigned int KTNS(vector<int> s){
	return 0;
}

// ------------------------------------------------------------------------------------------------------------------------------------------------------
// COST
// ------------------------------------------------------------------------------------------------------------------------------------------------------

unsigned int costReport(vector<int> sol, vector<int> machine, fstream& solutionReportFile){
	
	vector<vector<int>> sols(numberMachines);
	int totalCost = 0;
	
	solutionReportFile << planingHorizon << ";" << unsupervised << ";" << TIMESCALE << endl;

    for (int i = 0; i < sol.size(); ++i){
        sols[machine[sol[i]]].push_back(sol[i]);
    }

    for (int i = 0; i < sols.size(); ++i) {

		// ------------------------------------------------------------------------------------------------
		// int countCreated = 0;

		// int lastJob = sols[i][sols[i].size()-1];
		// sols[i].erase(sols[i].end()-1, sols[i].end());

		// for (auto it = superJobs[lastJob].originalJobs.begin(); it != superJobs[lastJob].originalJobs.end(); ++it){
		// 	SuperJob SuperJobTmp;

		// 	SuperJobTmp.indexSuperToolSet = superJobs[lastJob].indexSuperToolSet;
		// 	SuperJobTmp.processingTimeSum = originalJobs[*it].processingTime;
		// 	SuperJobTmp.prioritySum = originalJobs[*it].priority;
		// 	SuperJobTmp.originalJobs.push_back(*it);

		// 	superJobs.push_back(SuperJobTmp);
		// 	countCreated++;

		// 	sols[i].push_back(superJobs.size()-1);
		// }

		// ------------------------------------------------------------------------------------------------

        solutionReportFile << "Machine: " << i << " = ";
        for (int j = 0; j < sols[i].size(); ++j)
			for (auto it = superJobs[sols[i][j]].originalJobs.begin(); it != superJobs[sols[i][j]].originalJobs.end(); ++it){
				solutionReportFile << "(" << originalJobs[*it].indexJob << "," << originalJobs[*it].indexOperation << ");";
			}
		solutionReportFile << "\n";

        totalCost += KTNSReport(sols[i], solutionReportFile);

		// ------------------------------------------------------------------------------------------------

		// superJobs.erase(superJobs.end() - countCreated, superJobs.end());

    }

	return totalCost;

}

unsigned int cost(vector<int> sol){
	return 0;
}