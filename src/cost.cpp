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

unsigned int KTNS(vector<int> s){
	vector<bool> magazineL(numberTools, true);	
	unsigned int switchs = 0; 
	int jL;

	int switchsInstances = 0; 		// Conta quantas trocas de instancia foram feitas, quando pelo menos uma troca de ferramenta foi trocada do magazine
	int currantSwitchs = 0; 		// Conta quantas trocas de ferramenta foram feitas, no job atual

	int inicioJob = 0; 				// Conta quantas horas ja foram usadas no dia atual  		       
	int fimJob = 0; 				// Conta quantos dias ja foram usados no horizonte de planejamento 

	int fineshedPriorityCount = 0;
	int unfineshedPriorityCount = 0;

	int numberJobsSol = s.size();

	int currantProcessingTime = 0; 
	int logicalMachine = 0;	

	#ifdef PRINTS
	solutionReportFile << planingHorizon << ";" << unsupervised << ";" << DAY << endl;
	solutionReportFile << "Machine: " << logicalMachine << endl;
	#endif

	for(jL= 0; jL < numberJobsSol; ++jL){
		
		currantSwitchs = 0;

		vector<bool> magazineCL(numberTools);		
		int left = jL;
		int cmL = 0;

		while((cmL < capacityMagazine) && (left < numberJobsSol)){
			for (auto it=originalJobs[s[left]].toolSet.tools.begin(); ((it!=originalJobs[s[left]].toolSet.tools.end()) && (cmL < capacityMagazine)); ++it){
				
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
		// TIME VERIFICATIONS
		// ---------------------------------------------------------------------------

		currantProcessingTime = originalJobs[s[jL]].processingTime;
		fimJob = inicioJob + currantProcessingTime;
		int jobStartInDay = inicioJob % DAY;

		// verifica se a hora é sem supervisao e se houve troca de ferramenta
		if ((jobStartInDay >= unsupervised) && (currantSwitchs > 0)){
			//Verificar de consigo acabar essa tarefa antes de exceder o horizonte de planejamento
			if(fimJob >= planingHorizon * DAY * numberMachines){
				break;
			}
			else{
				inicioJob += DAY - jobStartInDay;
				fimJob = inicioJob + currantProcessingTime;
			}
		}

		//Tarefa vai vazar para o proximo dia
		if (jobStartInDay + currantProcessingTime >= DAY){
			//Verificar de consigo acabar essa tarefa antes de exceder o horizonte de planejamento extendido
			if(fimJob >= planingHorizon * DAY * numberMachines){
				break;
			}
			else{
				inicioJob += DAY - jobStartInDay;
				fimJob = inicioJob + currantProcessingTime;
				#ifdef PRINTS
				locagicalMachine = logicalMachine + 1;
				solutionReportFile << "Machine: " << logicalMachine << endl;
				#endif
			}
		}

		inicioJob = fimJob;

		// ---------------------------------------------------------------------------
		// PRINTS
		// ---------------------------------------------------------------------------

		#ifdef PRINTS
		int inicioTmp = fimJob - currantProcessingTime;
		int fimTmp = fimJob;

		fimTmp = inicioTmp + originalJobs[s[jL]].processingTime;
		solutionReportFile << originalJobs[s[jL]].indexJob << ";";
		solutionReportFile << originalJobs[s[jL]].indexOperation << ";";
		solutionReportFile << inicioTmp % (logicalMachine + 1) << ";";
		solutionReportFile << fimTmp % (logicalMachine + 1) << ";";
		solutionReportFile << originalJobs[s[jL]].priority << ";";

		for(unsigned int t = 0; t < magazineCL.size(); ++t){
			if(magazineCL[t]){ 
				solutionReportFile << t << ",";
			}
		}
		solutionReportFile << "\n";
		inicioTmp = fimTmp;
		#endif
		
		// ---------------------------------------------------------------------------
		// COSTS
		// ---------------------------------------------------------------------------

		switchs += currantSwitchs;
		if(currantSwitchs > 0) ++switchsInstances;
		fineshedPriorityCount += originalJobs[s[jL]].priority;

	}

	// Contar quantar tarefas prioritarias faltaram
	for(unsigned int v = jL; v < numberJobsSol; ++v){
		unfineshedPriorityCount += originalJobs[s[v]].priority;
	}

	int cost = (PROFITYPRIORITY * fineshedPriorityCount) - (COSTSWITCH * switchs) - (COSTSWITCHINSTANCE * switchsInstances) - (COSTPRIORITY * unfineshedPriorityCount);
	
	#ifdef PRINTS
	solutionReportFile << "END;";
	solutionReportFile << fineshedPriorityCount << ";";
	solutionReportFile << switchs << ";";
	solutionReportFile << switchsInstances << ";";
	solutionReportFile << unfineshedPriorityCount << ";";
	solutionReportFile << cost << "\n";
	#endif

	return cost;
}
