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

double reverseSearchMap(int value, map<int, int> ferramentas){
	for(auto it = ferramentas.begin(); it != ferramentas.end(); ++it){
		if(it->second == value) return it->first;
	}
	return -1;
}

double SSP::evaluate(solSSP solution){	
	vector<int> s = solution.sol;
	
	int pipes_count = 0;
	int last_full = 0;
	vector<int> last_seen(numberTools);
	vector<vector<int>> M;
	
	int switchsInstances = 0;
	int toolSwitches = 0;
	int fineshedJobsCount = 0;
	int unfineshedPriorityCount = 0;

	int inicioJob = 0;
	int fimJob = originalJobs[s[0]].processingTime;
	int extendedPlaningHorizon = ((planingHorizon * numberMachines)*DAY);
	int isFirstJobOfMachine = 0;

	//Completa o last_seen
	for(unsigned int i = 0; i < numberTools; ++i){
		if(toolJob[i][s[0]]) last_seen[i] = 0;
		else last_seen[i] = -1;
	}
	M.push_back(originalJobs[s[0]].toolSetNormalized.tools);
	fineshedJobsCount += 1;

	inicioJob = fimJob;
	
	for(unsigned int e = 1; e < numberJobs; ++e){
		int toolChange = false;
		int currantSwitchs = 0;

		// ---------------------------------------------------------------------------
		// METODO
		// ---------------------------------------------------------------------------

		M.push_back(originalJobs[s[e]].toolSetNormalized.tools);
				
		for (auto t = originalJobs[s[e]].toolSetNormalized.tools.begin(); t != originalJobs[s[e]].toolSetNormalized.tools.end(); ++t){
			if(last_full <= last_seen[*t]){
				++pipes_count;
				currantSwitchs += 1;
				for(unsigned int i = (last_seen[*t]+1); i < e; ++i){
					toolChange = true;
					M[i].push_back(*t);					
					if(M[i].size() == capacityMagazine) last_full = i;
				}
			}
			last_seen[*t] = e; 	
		}
		
		if(M[e].size() == capacityMagazine) last_full = e;

		// ---------------------------------------------------------------------------
		// TIME VERIFICATIONS
		// ---------------------------------------------------------------------------

		fimJob = inicioJob + originalJobs[s[e]].processingTime;
		
		if( ((inicioJob % DAY) >= unsupervised && (currantSwitchs > 0)) || 								        //verificar se estou em um periodo sem supervisao e houve troca de ferramenta
		    (inicioJob % (planingHorizon*DAY) + (originalJobs[s[e]].processingTime) > (planingHorizon*DAY)) ){ //verificar se o job excede o horizonte de planejamento unico (iria extender de uma maquina para outra)
			
			inicioJob += DAY - (inicioJob % DAY);
			fimJob = inicioJob + originalJobs[s[e]].processingTime;

		}

		if ((inicioJob % (planingHorizon*DAY) == 0)) {
			isFirstJobOfMachine = 1;
		}
		else isFirstJobOfMachine = 0;

		if(fimJob > extendedPlaningHorizon) break;
		
		inicioJob = fimJob;

		// ---------------------------------------------------------------------------
		// COSTS
		// ---------------------------------------------------------------------------

		fineshedJobsCount += 1;
		if (toolChange) ++switchsInstances;
		toolSwitches += currantSwitchs;
		
		std::vector<int> diff;
		std::sort(M[e].begin(), M[e].end());
		std::sort(M[e-1].begin(), M[e-1].end());
		std::set_difference(M[e].begin(), M[e].end(), M[e-1].begin(), M[e-1].end(), std::inserter(diff, diff.begin()));

		if (!isFirstJobOfMachine) toolSwitches += diff.size();
		if (diff.size() > 0 && !isFirstJobOfMachine) ++switchsInstances;

	}

	for(int v = fineshedJobsCount; v < numberJobs; ++v){
		unfineshedPriorityCount += originalJobs[s[v]].priority;
	}

	int cost = (PROFITYFINISHED * fineshedJobsCount) - (COSTSWITCH * (toolSwitches)) - (COSTSWITCHINSTANCE * switchsInstances) - (COSTPRIORITY * unfineshedPriorityCount);
	
	return -cost;
}

double SSP::evaluateReportGPCA(solSSP solution, string filenameJobs, string filenameTools, string solutionReportFileName, int time){
	vector<vector<int>> fineshedIndex;
	vector<vector<int>> fineshedInicio;
	vector<vector<int>> fineshedFim;
	vector<int> machines;
	
	// iterate over numberMachines and push vector to each fineshedIndex, fineshedInicio, fineshedFim
	for(int i = 0; i < numberMachines; i++){
		vector<int> tmp;
		fineshedIndex.push_back(tmp);
		fineshedInicio.push_back(tmp);
		fineshedFim.push_back(tmp);
	}

	vector<int> s = solution.sol;
	
	fstream solutionReportFile;
	solutionReportFile.open(solutionReportFileName, ios::out);

	int pipes_count = 0;
	int last_full = 0;
	vector<int> last_seen(numberTools);
	vector<vector<int>> M;
	
	int switchsInstances = 0;
	int toolSwitches = 0;
	int fineshedJobsCount = 0;
	int unfineshedPriorityCount = 0;

	int inicioJob = 0;
	int fimJob = originalJobs[s[0]].processingTime;
	int extendedPlaningHorizon = ((planingHorizon * numberMachines)*DAY);
	int isFirstJobOfMachine = 0;

	int logicalMachine = 0;	
	solutionReportFile << filenameJobs << ";" << filenameTools << endl;
	solutionReportFile << planingHorizon << ";" << unsupervised << ";" << DAY << endl;

	//Completa o last_seen
	for(unsigned int i = 0; i < numberTools; ++i){
		if(toolJob[i][s[0]]) last_seen[i] = 0;
		else last_seen[i] = -1;
	}
	M.push_back(originalJobs[s[0]].toolSetNormalized.tools);
	fineshedJobsCount += 1;

	fineshedIndex[logicalMachine].push_back(s[0]);
	fineshedInicio[logicalMachine].push_back(0);
	fineshedFim[logicalMachine].push_back(fimJob);

	inicioJob = fimJob;
	machines.push_back(logicalMachine);
	logicalMachine = logicalMachine + 1;
	
	for(unsigned int e = 1; e < numberJobs; ++e){
		int toolChange = false;
		int currantSwitchs = 0;

		// ---------------------------------------------------------------------------
		// METODO
		// ---------------------------------------------------------------------------

		M.push_back(originalJobs[s[e]].toolSetNormalized.tools);
				
		for (auto t = originalJobs[s[e]].toolSetNormalized.tools.begin(); t != originalJobs[s[e]].toolSetNormalized.tools.end(); ++t){
			if(last_full <= last_seen[*t]){
				int tool = reverseSearchMap(*t, ferramentas);
                cout << last_full << " <= " << last_seen[*t] << endl;
				cout << "tool: " << tool << endl;
				++pipes_count;
				toolChange = true;
				currantSwitchs += 1;
				for(unsigned int i = (last_seen[*t]+1); i < e; ++i){
					M[i].push_back(*t);					
					if(M[i].size() == capacityMagazine) last_full = i;
				}
			}
			last_seen[*t] = e; 	
		}
		
		if(M[e].size() == capacityMagazine) last_full = e;

		// ---------------------------------------------------------------------------
		// TIME VERIFICATIONS
		// ---------------------------------------------------------------------------

		fimJob = inicioJob + originalJobs[s[e]].processingTime;
		
		if( ((inicioJob % DAY) >= unsupervised && (currantSwitchs > 0)) || 								        //verificar se estou em um periodo sem supervisao e houve troca de ferramenta
		    (inicioJob % (planingHorizon*DAY) + (originalJobs[s[e]].processingTime) > (planingHorizon*DAY)) ){ //verificar se o job excede o horizonte de planejamento unico (iria extender de uma maquina para outra)
			
			inicioJob += DAY - (inicioJob % DAY);
			fimJob = inicioJob + originalJobs[s[e]].processingTime;

		}

		if ((inicioJob % (planingHorizon*DAY) == 0)) {
			isFirstJobOfMachine = 1;
		}
		else isFirstJobOfMachine = 0;

		if(fimJob > extendedPlaningHorizon) break;
		
		inicioJob = fimJob;

		// ---------------------------------------------------------------------------
		// COSTS
		// ---------------------------------------------------------------------------

		fineshedJobsCount += 1;

		if (toolChange) ++switchsInstances;
		toolSwitches += currantSwitchs;
		cout << "e: " << e << " currantSwitchs: " << currantSwitchs << endl;
		
		// std::vector<int> diff;
		// std::sort(M[e].begin(), M[e].end());
		// std::sort(M[e-1].begin(), M[e-1].end());
		// std::set_difference(M[e].begin(), M[e].end(), M[e-1].begin(), M[e-1].end(), std::inserter(diff, diff.begin()));

		// if (!isFirstJobOfMachine) toolSwitches += diff.size();
		// if (diff.size() > 0 && !isFirstJobOfMachine) ++switchsInstances;

		// ---------------------------------------------------------------------------
		// PRINTS
		// ---------------------------------------------------------------------------
		
		int inicioTMP = (fimJob - originalJobs[s[e]].processingTime) % (planingHorizon*DAY);
		int fimTMP    = ((fimJob-1) % (planingHorizon*DAY))+1;
		
		if(inicioTMP % (planingHorizon*DAY) == 0){
			machines.push_back(logicalMachine);
			logicalMachine = logicalMachine + 1;
		}
		fineshedIndex[logicalMachine-1].push_back(s[e]);
		fineshedInicio[logicalMachine-1].push_back(inicioTMP);
		fineshedFim[logicalMachine-1].push_back(fimTMP);
	}
	
	// vector<bool> usedG(numberTools, false);
	
	// unsigned int i,j;
	// for(j=1; j < numberJobs ; ++j){
	// 	i = j - 1;
		
	// 	for (auto t = M[j].begin(); t != M[j].end(); ++t){
	// 		usedG[*t] = true;
	// 	}
		
	// 	for (auto t = M[i].begin(); t != M[i].end(); ++t){
	// 		if((!usedG[*t]) && (M[j].size() < capacityMagazine)){
	// 			M[j].push_back(*t);
	// 		}
	// 	}
		
	// 	for (auto t = M[j].begin(); t != M[j].end(); ++t){
	// 		usedG[*t] = false;
	// 	}		 
	// }
	
	// for(i=(numberJobs-1); i > 0; --i){
	// 	j = i - 1;
		
	// 	for (auto t = M[j].begin(); t != M[j].end(); ++t){
	// 		usedG[*t] = true;
	// 	}
		
	// 	for (auto t = M[i].begin(); t != M[i].end(); ++t){
	// 		if((!usedG[*t]) && (M[j].size() < capacityMagazine)){
	// 			M[j].push_back(*t);
	// 		}
	// 	}
		
	// 	for (auto t = M[j].begin(); t != M[j].end(); ++t){
	// 		usedG[*t] = false;
	// 	}		 
	// }

	// iterate of the machines and print the results
	int countTmp = 0;
	for(int i = 0; i < machines.size(); i++){
		int tmpMachineIndex = machines[i];
		solutionReportFile << "Machine: " << tmpMachineIndex << endl;

		for(int j = 0; j < fineshedIndex[tmpMachineIndex].size(); j++){
			int tmp = fineshedIndex[tmpMachineIndex][j];

			solutionReportFile << originalJobs[tmp].indexJob << ";";
			solutionReportFile << originalJobs[tmp].indexOperation << ";";
			solutionReportFile << fineshedInicio[tmpMachineIndex][j] << ";";
			solutionReportFile << fineshedFim[tmpMachineIndex][j] << ";";
			solutionReportFile << originalJobs[tmp].priority << ";";

			for(unsigned int t = 0; t < M[countTmp].size(); ++t){
				int realTool = reverseSearchMap(M[countTmp][t], ferramentas);
				solutionReportFile << realTool << ",";
			}
			solutionReportFile << "\n";
			countTmp++;
		}
	}

	// Contar quantar tarefas prioritarias faltaram
	for(int v = fineshedJobsCount; v < numberJobs; ++v){
		unfineshedPriorityCount += originalJobs[s[v]].priority;
	}

	int cost = (PROFITYFINISHED * fineshedJobsCount) - (COSTSWITCH * (toolSwitches)) - (COSTSWITCHINSTANCE * switchsInstances) - (COSTPRIORITY * unfineshedPriorityCount);
	
	solutionReportFile << "END;";
	solutionReportFile << fineshedJobsCount << ";";
	solutionReportFile << toolSwitches << ";";
	solutionReportFile << switchsInstances << ";";
	solutionReportFile << unfineshedPriorityCount << ";";
	solutionReportFile << cost << "\n";

	solutionReportFile << "TIME;" << time << endl;

	int lowerBoundValue = lowerBound();
	solutionReportFile << "LOWERBOUND;" << lowerBoundValue << endl;
	
	solutionReportFile.close();

	return cost;
}
