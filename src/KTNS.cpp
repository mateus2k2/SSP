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
#include "headers/SSP.h"

#ifndef IGNORE_FMT
#include <fmt/core.h>
#include <fmt/ranges.h>
#endif

using namespace std;

// double SSP::evaluate(solSSP solution){
// 	vector<int> s = solution.sol;

// 	vector<bool> magazineL(numberTools, true);	
// 	unsigned int switchs = 0; 
// 	int numberJobsSol = s.size();
// 	int jL;

// 	int switchsInstances = 0;
// 	int currantSwitchs = 0;
// 	int fineshedJobsCount = 0;
// 	int unfineshedPriorityCount = 0;

// 	int inicioJob = 0; 				
// 	int fimJob = 0; 				
// 	int extendedPlaningHorizon = (planingHorizon * numberMachines)*DAY;
// 	int isFirstJobOfMachine = 1;

// 	for(jL= 0; jL < numberJobsSol; ++jL){
// 		// ---------------------------------------------------------------------------
// 		// TIME VERIFICATIONS
// 		// ---------------------------------------------------------------------------

// 		fimJob = inicioJob + originalJobs[s[jL]].processingTime;
		
// 		if( ((inicioJob % DAY) >= unsupervised && (currantSwitchs > 0)) || 								        //verificar se estou em um periodo sem supervisao e houve troca de ferramenta
// 		    (inicioJob % (planingHorizon*DAY) + (originalJobs[s[jL]].processingTime) > (planingHorizon*DAY)) ){ //verificar se o job excede o horizonte de planejamento unico (iria extender de uma maquina para outra)
			
// 			inicioJob += DAY - (inicioJob % DAY);
// 			fimJob = inicioJob + originalJobs[s[jL]].processingTime;
// 		}
		
// 		if(fimJob > extendedPlaningHorizon) break;
		
// 		if ((inicioJob % (planingHorizon*DAY) == 0)) isFirstJobOfMachine = 1;
// 		else isFirstJobOfMachine = 0;
		
// 		inicioJob = fimJob;

// 		// ---------------------------------------------------------------------------
// 		// switchs
// 		// ---------------------------------------------------------------------------

// 		currantSwitchs = 0;
// 		vector<bool> magazineCL(numberTools);		
// 		int left = jL;
// 		int cmL = 0;

// 		while((cmL < capacityMagazine) && (left < numberJobsSol)){
// 			for (auto it=originalJobs[s[left]].toolSetNormalized.tools.begin(); ((it!=originalJobs[s[left]].toolSetNormalized.tools.end()) && (cmL < capacityMagazine)); ++it){
				
// 				if((magazineL[*it]) && (!magazineCL[*it])){
// 					magazineCL[*it] = true;
// 					++cmL;
// 				}else if((jL == left) && (!magazineCL[*it])){
// 					magazineCL[*it] = true;
// 					++cmL;
// 					++currantSwitchs;
// 				}
// 			}
// 			++left;
// 		}

// 		for(int t=0; ((t < numberTools) && (cmL < capacityMagazine)); t++){
// 			if((magazineL[t]) && (!magazineCL[t])){
// 				magazineCL[t] = true;
// 				++cmL;			
// 			}
// 		}
// 		magazineL = magazineCL;

		
// 		// ---------------------------------------------------------------------------
// 		// COSTS
// 		// ---------------------------------------------------------------------------

// 		if (isFirstJobOfMachine) currantSwitchs = 0;
// 		switchs += currantSwitchs;
// 		if(currantSwitchs > 0) ++switchsInstances;
// 		fineshedJobsCount += 1;

// 	}

// 	// Contar quantar tarefas prioritarias faltaram
// 	for(int v = jL; v < numberJobsSol; ++v){
// 		unfineshedPriorityCount += originalJobs[s[v]].priority;
// 	}

// 	int cost = (PROFITYFINISHED * fineshedJobsCount) - (COSTSWITCH * switchs) - (COSTSWITCHINSTANCE * switchsInstances) - (COSTPRIORITY * unfineshedPriorityCount);
	
// 	return (-1) * cost;
// }

double SSP::evaluateReportKTNS(solSSP solution, string filenameJobs, string filenameTools, string solutionReportFileName, int time){
	vector<int> s = solution.sol;
	
	fstream solutionReportFile;
	solutionReportFile.open(solutionReportFileName, ios::out);

	vector<bool> magazineL(numberToolsReal, true);	
	unsigned int switchs = 0; 
	int numberJobsSol = s.size();
	int jL;

	int switchsInstances = 0;
	int currantSwitchs = 0;
	int fineshedJobsCount = 0;
	int unfineshedPriorityCount = 0;

	int inicioJob = 0;
	int fimJob = 0;
	int extendedPlaningHorizon = (planingHorizon * numberMachines)*DAY;
	int isFirstJobOfMachine = 1;

	int logicalMachine = 0;	
	solutionReportFile << filenameJobs << ";" << filenameTools << endl;
	solutionReportFile << planingHorizon << ";" << unsupervised << ";" << DAY << endl;

	for(jL= 0; jL < numberJobsSol; ++jL){
		// ---------------------------------------------------------------------------
		// TIME VERIFICATIONS
		// ---------------------------------------------------------------------------

		fimJob = inicioJob + originalJobs[s[jL]].processingTime;
		
		if( ((inicioJob % DAY) >= unsupervised && (currantSwitchs > 0)) || 								        //verificar se estou em um periodo sem supervisao e houve troca de ferramenta
		    (inicioJob % (planingHorizon*DAY) + (originalJobs[s[jL]].processingTime) > (planingHorizon*DAY)) ){ //verificar se o job excede o horizonte de planejamento unico (iria extender de uma maquina para outra)
			
			inicioJob += DAY - (inicioJob % DAY);
			fimJob = inicioJob + originalJobs[s[jL]].processingTime;

		}

		if ((inicioJob % (planingHorizon*DAY) == 0)) isFirstJobOfMachine = 1;
		else isFirstJobOfMachine = 0;
		
		if(fimJob > extendedPlaningHorizon) break;
		
		inicioJob = fimJob;

		// ---------------------------------------------------------------------------
		// switchs
		// ---------------------------------------------------------------------------

		currantSwitchs = 0;
		vector<bool> magazineCL(numberToolsReal);		
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

		
		for(int t=0; ((t < numberToolsReal) && (cmL < capacityMagazine)); t++){
			if((magazineL[t]) && (!magazineCL[t])){
				magazineCL[t] = true;
				++cmL;			
			}
		}
		magazineL = magazineCL;
		
		// ---------------------------------------------------------------------------
		// COSTS
		// ---------------------------------------------------------------------------

		if (isFirstJobOfMachine) currantSwitchs = 0;
		switchs += currantSwitchs;
		if(currantSwitchs > 0) ++switchsInstances;
		// fineshedJobsCount += originalJobs[s[jL]].priority;
		fineshedJobsCount += 1;

		// ---------------------------------------------------------------------------
		// PRINTS
		// ---------------------------------------------------------------------------
		
		int inicioTMP = (fimJob - originalJobs[s[jL]].processingTime) % (planingHorizon*DAY);
		int fimTMP    = ((fimJob-1) % (planingHorizon*DAY))+1;
		
		if(inicioTMP % (planingHorizon*DAY) == 0){
			// if (logicalMachine > 0){
			// 	for(unsigned int v = jL; v < numberJobsSol; ++v) unfineshedPriorityCount += originalJobs[s[v]].priority;
			// 	int cost = (PROFITYFINISHED * fineshedJobsCount) - (COSTSWITCH * switchs) - (COSTSWITCHINSTANCE * switchsInstances) - (COSTPRIORITY * unfineshedPriorityCount);
				// solutionReportFile << "END;";
				// solutionReportFile << fineshedJobsCount << ";";
				// solutionReportFile << switchs << ";";
				// solutionReportFile << switchsInstances << ";";
				// solutionReportFile << unfineshedPriorityCount << ";";
				// solutionReportFile << cost << "\n";
			// }
			solutionReportFile << "Machine: " << logicalMachine << endl;
			logicalMachine = logicalMachine + 1;
		}

		solutionReportFile << originalJobs[s[jL]].indexJob << ";";
		solutionReportFile << originalJobs[s[jL]].indexOperation << ";";
		solutionReportFile << inicioTMP << ";";
		solutionReportFile << fimTMP << ";";
		solutionReportFile << originalJobs[s[jL]].priority << ";";

		for(unsigned int t = 0; t < magazineCL.size(); ++t){
			if(magazineCL[t]){ 
				solutionReportFile << t << ",";
			}
		}
		solutionReportFile << "\n";

	}

	// Contar quantar tarefas prioritarias faltaram
	for(int v = jL; v < numberJobsSol; ++v){
		unfineshedPriorityCount += originalJobs[s[v]].priority;
	}

	int cost = (PROFITYFINISHED * fineshedJobsCount) - (COSTSWITCH * switchs) - (COSTSWITCHINSTANCE * switchsInstances) - (COSTPRIORITY * unfineshedPriorityCount);
	
	solutionReportFile << "END;";
	solutionReportFile << fineshedJobsCount << ";";
	solutionReportFile << switchs << ";";
	solutionReportFile << switchsInstances << ";";
	solutionReportFile << unfineshedPriorityCount << ";";
	solutionReportFile << cost << "\n";

	solutionReportFile << "TIME;" << time << endl;

	int lowerBoundValue = lowerBound();
	solutionReportFile << "LOWERBOUND;" << lowerBoundValue << endl;
	
	solutionReportFile.close();

	return cost;
}
