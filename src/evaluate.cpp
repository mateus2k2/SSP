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

// ------------------------------------------------------------------------------------------------------------------------------------------------------
// KTNS
// ------------------------------------------------------------------------------------------------------------------------------------------------------

double SSP::GPCA(solSSP solution){
	vector<int> s = solution.sol;

	vector<bool> magazineL(numberTools, true);	
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
		
		if(fimJob > extendedPlaningHorizon) break;
		
		if ((inicioJob % (planingHorizon*DAY) == 0)) isFirstJobOfMachine = 1;
		else isFirstJobOfMachine = 0;
		
		inicioJob = fimJob;

		// ---------------------------------------------------------------------------
		// switchs
		// ---------------------------------------------------------------------------

		currantSwitchs = 0;
		vector<bool> magazineCL(numberTools);		
		int left = jL;
		int cmL = 0;

		while((cmL < capacityMagazine) && (left < numberJobsSol)){
			for (auto it=originalJobs[s[left]].toolSetNormalized.tools.begin(); ((it!=originalJobs[s[left]].toolSetNormalized.tools.end()) && (cmL < capacityMagazine)); ++it){
				
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
		// COSTS
		// ---------------------------------------------------------------------------

		if (isFirstJobOfMachine) currantSwitchs = 0;
		switchs += currantSwitchs;
		if(currantSwitchs > 0) ++switchsInstances;
		fineshedJobsCount += 1;

	}

	// Contar quantar tarefas prioritarias faltaram
	for(int v = jL; v < numberJobsSol; ++v){
		unfineshedPriorityCount += originalJobs[s[v]].priority;
	}

	int cost = (PROFITYFINISHED * fineshedJobsCount) - (COSTSWITCH * switchs) - (COSTSWITCHINSTANCE * switchsInstances) - (COSTPRIORITY * unfineshedPriorityCount);
	
	return (-1) * cost;
}

double SSP::evaluateReport(solSSP solution, string filenameJobs, string filenameTools, string solutionReportFileName, int time){
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

// ------------------------------------------------------------------------------------------------------------------------------------------------------
// GPCA
// ------------------------------------------------------------------------------------------------------------------------------------------------------

double SSP::evaluate(solSSP solution){	

	int inicioJob = 0; 				
	int fimJob = 0; 				
	int extendedPlaningHorizon = (planingHorizon * numberMachines)*DAY;

	int switchsInstances = 0;
	int currantSwitchs = 0;
	int fineshedJobsCount = 0;
	int unfineshedPriorityCount = 0;
	
	int pipes_count = 0;
	int last_full = 0;
	vector<int> last_seen(numberTools);
	vector<vector<int>> M;
	vector<int> s = solution.sol;
	
	//Completa o last_seen
	for(unsigned int i = 0; i < numberTools; ++i){
		if(toolJob[i][s[0]]) last_seen[i] = 0;
		else last_seen[i] = -1;
	}
	M.push_back(originalJobs[s[0]].toolSetNormalized.tools);
	fineshedJobsCount += 1;
	
	for(unsigned int e = 1; e < numberJobs; ++e){
		int toolChange = false;

		// ---------------------------------------------------------------------------
		// TIME VERIFICATIONS
		// ---------------------------------------------------------------------------

		fimJob = inicioJob + originalJobs[e].processingTime;
		
		if( ((inicioJob % DAY) >= unsupervised && (currantSwitchs > 0)) || 								    //verificar se estou em um periodo sem supervisao e houve troca de ferramenta
		    (inicioJob % (planingHorizon*DAY) + (originalJobs[e].processingTime) > (planingHorizon*DAY)) ){ //verificar se o job excede o horizonte de planejamento unico (iria extender de uma maquina para outra)
			
			inicioJob += DAY - (inicioJob % DAY);
			fimJob = inicioJob + originalJobs[e].processingTime;
		}
		
		if(fimJob > extendedPlaningHorizon) break;
		
		inicioJob = fimJob;
		
		// ---------------------------------------------------------------------------
		// METODO
		// ---------------------------------------------------------------------------
		
		M.push_back(originalJobs[s[e]].toolSetNormalized.tools);
				
		for (auto t = originalJobs[s[e]].toolSetNormalized.tools.begin(); t != originalJobs[s[e]].toolSetNormalized.tools.end(); ++t){
			
			if(last_full <= last_seen[*t]){
				++pipes_count;
				toolChange = true;
				for(unsigned int i = (last_seen[*t]+1); i < e; ++i){
					M[i].push_back(*t);					
					if(M[i].size() == capacityMagazine) last_full = i;
				}
			}
			last_seen[*t] = e; 	
		}
		
		if(M[e].size() == capacityMagazine) last_full = e;

		fineshedJobsCount += 1;
		if (toolChange) ++switchsInstances;
	}

	// Contar quantar tarefas prioritarias faltaram
	for(int v = fineshedJobsCount; v < numberJobs; ++v){
		unfineshedPriorityCount += originalJobs[s[v]].priority;
	}

	int cost = (PROFITYFINISHED * fineshedJobsCount) - (COSTSWITCH * pipes_count-capacityMagazine) - (COSTSWITCHINSTANCE * switchsInstances) - (COSTPRIORITY * unfineshedPriorityCount);
	// cout << "fineshedJobsCount: " << fineshedJobsCount << endl;
	// cout << "pipes_count: " << pipes_count-capacityMagazine << endl;
	// cout << "switchsInstances: " << switchsInstances << endl;
	// cout << "unfineshedPriorityCount: " << unfineshedPriorityCount << endl;
	// cout << cost << endl;
	return -cost;
}