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

	// ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

	int numberJobsSol = s.size();

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
			if(currantProcessingTime + (inicioJob + (TIMESCALE - (inicioJob % TIMESCALE))) >= planingHorizon * TIMESCALE){
			
				int lastJob = s[s.size()-1];
				s.clear();

				for (auto it = superJobs[lastJob].originalJobs.begin(); it != superJobs[lastJob].originalJobs.end(); ++it){
					s.push_back(*it);
				}
				numberJobsSol = s.size();
				jL = 0;
				break;
				
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
				
				int lastJob = s[s.size()-1];
				s.clear();

				for (auto it = superJobs[lastJob].originalJobs.begin(); it != superJobs[lastJob].originalJobs.end(); ++it){
					s.push_back(*it);
				}
				numberJobsSol = s.size();
				jL = 0;
				break;
				
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

	// ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

	numberJobsSol = s.size();

	for(jL= 0; jL < numberJobsSol; ++jL){
		
		currantSwitchs = 0;

		vector<bool> magazineCL(numberTools);		
		int left = jL;
		int cmL = 0;

		while((cmL < capacityMagazine) && (left < numberJobsSol)){
			for (auto it=originalToolSets[originalJobs[s[left]].indexToolSet].tools.begin(); ((it!=originalToolSets[originalJobs[s[left]].indexToolSet].tools.end()) && (cmL < capacityMagazine)); ++it){
				
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


		// ---------------------------------------------------------------------------

		currantProcessingTime = originalJobs[s[jL]].processingTime;
		fimJob = inicioJob + currantProcessingTime;

		// verifica se a hora é sem supervisao e se houve troca de ferramenta
		if ((inicioJob % TIMESCALE >= unsupervised) && (currantSwitchs > 0)){
			
			//Verificar de consigo acabar essa tarefa antes de exceder o horizonte de planejamento
			if(currantProcessingTime + (inicioJob + (TIMESCALE - (inicioJob % TIMESCALE))) >= planingHorizon * TIMESCALE){
				
				// Contar quantar tarefas prioritarias faltaram
				for(unsigned int v = jL; v < numberJobsSol; ++v){
					unfineshedPriorityCount += originalJobs[s[v]].priority;
				}
				// Pode sair do loop 
				break;
				
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
				
				// Contar quantar tarefas prioritarias faltaram
				for(unsigned int v = jL; v < numberJobsSol; ++v){
					unfineshedPriorityCount += originalJobs[s[v]].priority;
				}
				// Pode sair do loop 
				break;
				
			}
			
		}
		
		inicioJob = fimJob;

		// ---------------------------------------------------------------------------
		
		int inicioTmp = fimJob - currantProcessingTime;
		int fimTmp = fimJob;


		fimTmp = inicioTmp + originalJobs[s[jL]].processingTime;

		solutionReportFile << originalJobs[s[jL]].indexJob << ";";
		solutionReportFile << originalJobs[s[jL]].indexOperation << ";";
		solutionReportFile << inicioTmp << ";";
		solutionReportFile << fimTmp << ";";
		solutionReportFile << originalJobs[s[jL]].priority << ";";

		for(unsigned int t = 0; t < magazineCL.size(); ++t){
			if(magazineCL[t]){ 
				solutionReportFile << t << ",";
			}
		}
		solutionReportFile << "\n";

		inicioTmp = fimTmp;

		
		// ---------------------------------------------------------------------------

		switchs += currantSwitchs;
		if(currantSwitchs > 0) ++switchsInstances;
		fineshedPriorityCount += originalJobs[s[jL]].priority;

		// ---------------------------------------------------------------------------

	}

	// ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------


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
	vector<vector<int>> sols;
	int totalCost = 0;
	
	solutionReportFile << planingHorizon << ";" << unsupervised << ";" << TIMESCALE << endl;

	for (int i = 0; i < machine.size(); ++i){
        size_t start = machine[i];
        size_t end = (i + 1 < machine.size()) ? machine[i + 1] : sol.size();

        vector<int> slice(sol.begin() + start, sol.begin() + end);
        sols.push_back(slice);
    }

    for (int i = 0; i < sols.size(); ++i) {
        solutionReportFile << "Machine: " << i << " = ";
        for (int j = 0; j < sols[i].size(); ++j)
			for (auto it = superJobs[sols[i][j]].originalJobs.begin(); it != superJobs[sols[i][j]].originalJobs.end(); ++it){
				solutionReportFile << "(" << originalJobs[*it].indexJob << "," << originalJobs[*it].indexOperation << ");";
			}
		solutionReportFile << "\n";

        totalCost += KTNSReport(sols[i], solutionReportFile);
    }

	return totalCost;

}

unsigned int cost(vector<int> sol){
	return 0;
}