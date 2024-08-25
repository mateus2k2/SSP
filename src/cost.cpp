#include <vector>
#include <iostream>
#include <atomic>
#include <thread>
#include <fstream>
#include <sstream>
#include <string>
#include <algorithm> 
#include <fstream>
#include <fmt/core.h>
#include <fmt/ranges.h>

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
	int extendedPlaningHorizon = (planingHorizon * numberMachines)*DAY;

	#ifdef PRINTS
	solutionReportFile << planingHorizon << ";" << unsupervised << ";" << DAY << endl;
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

		fimJob = inicioJob + originalJobs[s[jL]].processingTime;

		//normalize o fimJob e inicioJob para estarem em relacao ao primeiro dia
		int normalizedFimJob = fimJob % DAY;
		int normalizedInicioJob = inicioJob % DAY;

		//verificar e estou em um periodo sem supervisao e houve troca de ferramenta
		if((normalizedInicioJob >= unsupervised && (currantSwitchs > 0))){
			inicioJob += DAY - normalizedInicioJob;
			fimJob = inicioJob + originalJobs[s[jL]].processingTime;
		}

		// verificar se o job excede o horizonte de planejamento unico
		if(inicioJob % (planingHorizon*DAY) + (originalJobs[s[jL]].processingTime) > (planingHorizon*DAY)){
			inicioJob += DAY - normalizedInicioJob;
			fimJob = inicioJob + originalJobs[s[jL]].processingTime;
			#ifdef PRINTS
			logicalMachine = logicalMachine + 1;
			solutionReportFile << "Machine: " << logicalMachine << endl;
			#endif
		}
		
		//verificar se o job excede o horizonte de planejamento extendido
		if(fimJob > extendedPlaningHorizon) break;
		
		// o job comeca exatamente no inicio do horizonte de planejamento ou comeca no fim do anterior e termina no inicio do proximo
		#ifdef PRINTS
		if(inicioJob % (planingHorizon*DAY) == 0){
			solutionReportFile << "Machine: " << logicalMachine << endl;
			logicalMachine = logicalMachine + 1;
		}
		#endif

		inicioJob = fimJob;

		// ---------------------------------------------------------------------------
		// PRINTS
		// ---------------------------------------------------------------------------

		#ifdef PRINTS
		solutionReportFile << originalJobs[s[jL]].indexJob << ";";
		solutionReportFile << originalJobs[s[jL]].indexOperation << ";";
		solutionReportFile << (fimJob-originalJobs[s[jL]].processingTime) % (planingHorizon*DAY) << ";";
		solutionReportFile << fimJob % (planingHorizon*DAY) << ";";
		solutionReportFile << originalJobs[s[jL]].priority << ";";

		for(unsigned int t = 0; t < magazineCL.size(); ++t){
			if(magazineCL[t]){ 
				solutionReportFile << t << ",";
			}
		}
		solutionReportFile << "\n";
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
