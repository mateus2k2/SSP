#include <vector>
#include <iostream>
#include <atomic>
#include <thread>
#include <fstream>
#include <sstream>
#include <string>
#include <algorithm> 

#include "headers/GlobalVars.h"
#include "headers/loadData.h"
#include "headers/cost.h"

using namespace std;

// ------------------------------------------------------------------------------------------------------------------------------------------------------
// KTNS
// ------------------------------------------------------------------------------------------------------------------------------------------------------

unsigned int KTNSReport(vector<int> s){
    // Variaveis
	vector<bool> magazineL(numberTools, true);	
	unsigned int evalSol = 0; 
	int jL;

	int sumChanges = 0; // Conta quantas trocas de instancia foram feitas, quando pelo menos uma troca de ferramenta foi trocada do magazine
	int currantChanges = 0; // Conta quantas trocas de ferramenta foram feitas, no job atual
	int currantMinute = 0; // Conta quantas horas ja foram usadas no dia atual  		       
	int currantDay = 0; // Conta quantos dias ja foram usados no horizonte de planejamento 

	vector<int> fineshedPriority; // Jobs prioritarios que foram feitos
	vector<int> unfinesedPriority; // Jobs prioritarios que foram feitos

	numberJobs = s.size();

	// Percorre todas as ferramentas
	for(jL= 0; jL < numberJobs; ++jL){
		
		currantChanges = 0;

		vector<bool> magazineCL(numberTools);		
		int left = jL;
		int cmL = 0;

		while((cmL < capacityMagazine) && (left < numberJobs)){
			for (auto it=JobTools[s[left]].begin(); ((it!=JobTools[s[left]].end()) && (cmL < capacityMagazine)); ++it){
				if((magazineL[*it]) && (!magazineCL[*it])){
					magazineCL[*it] = true;
					++cmL;
				}else if((jL == left) && (!magazineCL[*it])){
					magazineCL[*it] = true;
					++cmL;
					++currantChanges;
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

		if (jL == 0) currantChanges = capacityMagazine;

		// ---------------------------------------------------------------------------

		// verifica se a hora é sem supervisao e se houve troca de ferramenta
		if ((currantMinute >= unsupervised) && (currantChanges > 0)){
			//Verificar de consigo acabar essa tarefa antes de exceder o horizonte de planejamento
			if(currantDay + 1 > planingHorizon){
				// Contar quantar tarefas prioritarias faltaram
				for(unsigned int v = jL; v < numberJobs; ++v){
					if(priority[s[v]]) unfinesedPriority.push_back(s[v]);
				}
				// Pode sair do loop 
				break;
			}
			else{
				currantDay++;
				currantMinute = 0;
			}
		}

		//Tarefa vai vazar para o proximo dia
		if (currantMinute + processingTime[s[jL]] > 1440){
			//Verificar de consigo acabar essa tarefa antes de exceder o horizonte de planejamento
			if(currantDay + 1 > planingHorizon){
				// Contar quantar tarefas prioritarias faltaram
				for(unsigned int v = jL; v < numberJobs; ++v){
					if(priority[s[v]]) unfinesedPriority.push_back(s[v]);
				}
				// Pode sair do loop 
				break;
			}
			else{
				currantDay++;
				// Acumular as horas que vazaram para o dia seguinte
				currantMinute += (currantMinute + processingTime[s[jL]])-1440;
			}
		}
		//Tarefa pode continuar no mesmo dia
		else{
			currantMinute += processingTime[s[jL]];
		}

		// ---------------------------------------------------------------------------

		cout << "Job Processado: " << s[jL] << " | " << "Processing Time: "  << processingTime[s[jL]]  << " | " << "Priority: " << priority[s[jL]] << " | " << "Count Mudanças Magazine: " << currantChanges << endl;
		printf("Minuto do Dia: %i/1440 | Hora: %d/24\n", currantMinute, currantMinute/60);
		printf("Dia Atual: %i/%i\n", currantDay, planingHorizon);
		
		cout << "\n";
		
		cout << "Magazine: ";
		for(unsigned int t = 0; t < magazineCL.size(); ++t){
			if(magazineCL[t]) 
				cout << t << " ";
		}

		cout << "\n\n";

		
		// ---------------------------------------------------------------------------

		evalSol += currantChanges;
		if(currantChanges > 0) ++sumChanges;
		if (priority[s[jL]]) fineshedPriority.push_back(s[jL]);

		// ---------------------------------------------------------------------------

	}

	int fineshedPriorityCount = fineshedPriority.size();
	int unfinesedPriorityCount = unfinesedPriority.size();

	cout << "DONE\n";

	printf("Fineshed Priority (%i) : ", fineshedPriorityCount);
	for(unsigned int t = 0; t < fineshedPriorityCount; ++t){
		cout << fineshedPriority[t] << " ";
	}
	cout << "\n";

	printf("Unfineshed Priority (%i) : ", unfinesedPriorityCount);
	for(unsigned int t = 0; t < unfinesedPriorityCount; ++t){
		cout << unfinesedPriority[t] << " ";
	}
	cout << "\n";

	cout << "Number of Switchs: " << evalSol << "\n";
	cout << "Number of Switchs Instances: " << sumChanges << "\n";

	int cost = (PROFITYPRIORITY * fineshedPriorityCount) - (COSTSWITCH * evalSol) - (COSTSWITCHINSTANCE * sumChanges) - (COSTPRIORITY * unfinesedPriorityCount);
	printf("(%i * %i) - (%i * %i) - (%i * %i) - (%i * %i) = %i\n", PROFITYPRIORITY, fineshedPriorityCount, COSTSWITCH, evalSol, COSTSWITCHINSTANCE, sumChanges, COSTPRIORITY, unfinesedPriorityCount, cost);

	return cost;
}

unsigned int KTNSReportIfDef(vector<int> s){
    // Variaveis
	vector<bool> magazineL(numberTools, true);	
	unsigned int evalSol = 0; 
	int jL;

	int sumChanges = 0; // Conta quantas trocas de instancia foram feitas, quando pelo menos uma troca de ferramenta foi trocada do magazine
	int currantChanges = 0; // Conta quantas trocas de ferramenta foram feitas, no job atual
	int currantMinute = 0; // Conta quantas horas ja foram usadas no dia atual  		       
	int currantDay = 0; // Conta quantos dias ja foram usados no horizonte de planejamento 

	#ifdef DEBUG
	vector<int> fineshedPriority; // Jobs prioritarios que foram feitos
	vector<int> unfinesedPriority; // Jobs prioritarios que foram feitos
	#else
	int fineshedPriority = 0; // Jobs prioritarios que foram feitos
	int unfinesedPriority = 0; // Jobs prioritarios que foram feitos
	#endif

	numberJobs = s.size();

	// Percorre todas as ferramentas
	for(jL= 0; jL < numberJobs; ++jL){
		
		currantChanges = 0;

		vector<bool> magazineCL(numberTools);		
		int left = jL;
		int cmL = 0;

		while((cmL < capacityMagazine) && (left < numberJobs)){
			for (auto it=JobTools[s[left]].begin(); ((it!=JobTools[s[left]].end()) && (cmL < capacityMagazine)); ++it){
				if((magazineL[*it]) && (!magazineCL[*it])){
					magazineCL[*it] = true;
					++cmL;
				}else if((jL == left) && (!magazineCL[*it])){
					magazineCL[*it] = true;
					++cmL;
					++currantChanges;
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

		if (jL == 0) currantChanges = capacityMagazine;

		// ---------------------------------------------------------------------------

		// verifica se a hora é sem supervisao e se houve troca de ferramenta
		if ((currantMinute >= unsupervised) && (currantChanges > 0)){
			//Verificar de consigo acabar essa tarefa antes de exceder o horizonte de planejamento
			if(currantDay + 1 > planingHorizon){
				// Contar quantar tarefas prioritarias faltaram
				for(unsigned int v = jL; v < numberJobs; ++v){
					#ifdef DEBUG
					if(priority[s[v]]) unfinesedPriority.push_back(s[v]);
					#else
					if(priority[s[v]]) ++unfinesedPriority;
					#endif
				}
				// Pode sair do loop 
				break;
			}
			else{
				currantDay++;
				currantMinute = 0;
			}
		}

		//Tarefa vai vazar para o proximo dia
		if (currantMinute + processingTime[s[jL]] > 1440){
			//Verificar de consigo acabar essa tarefa antes de exceder o horizonte de planejamento
			if(currantDay + 1 > planingHorizon){
				// Contar quantar tarefas prioritarias faltaram
				for(unsigned int v = jL; v < numberJobs; ++v){
					#ifdef DEBUG
					if(priority[s[v]]) unfinesedPriority.push_back(s[v]);
					#else
					if(priority[s[v]]) ++unfinesedPriority;
					#endif
				}
				// Pode sair do loop 
				break;
			}
			else{
				currantDay++;
				// Acumular as horas que vazaram para o dia seguinte
				currantMinute += (currantMinute + processingTime[s[jL]])-1440;
			}
		}
		//Tarefa pode continuar no mesmo dia
		else{
			currantMinute += processingTime[s[jL]];
		}

		// ---------------------------------------------------------------------------

		#ifdef DEBUG
		cout << "Job Processado: " << s[jL] << " | " << "Processing Time: "  << processingTime[s[jL]]  << " | " << "Priority: " << priority[s[jL]] << " | " << "Count Mudanças Magazine: " << currantChanges << endl;
		printf("Minuto do Dia: %i/1440 | Hora: %d/24\n", currantMinute, currantMinute/60);
		printf("Dia Atual: %i/%i\n", currantDay, planingHorizon);
		
		cout << "\n";
		
		cout << "Magazine: ";
		for(unsigned int t = 0; t < magazineCL.size(); ++t){
			if(magazineCL[t]) 
				cout << t << " ";
		}

		cout << "\n\n";
		#endif

		
		// ---------------------------------------------------------------------------

		evalSol += currantChanges;
		if(currantChanges > 0) ++sumChanges;
		#ifdef DEBUG
		if (priority[s[jL]]) fineshedPriority.push_back(s[jL]);
		#else
		if (priority[s[jL]]) ++fineshedPriority;
		#endif

		// ---------------------------------------------------------------------------

	}

	#ifdef DEBUG
	int fineshedPriorityCount = fineshedPriority.size();
	int unfinesedPriorityCount = unfinesedPriority.size();

	cout << "DONE\n";

	printf("Fineshed Priority (%i) : ", fineshedPriorityCount);
	for(unsigned int t = 0; t < fineshedPriorityCount; ++t){
		cout << fineshedPriority[t] << " ";
	}
	cout << "\n";

	printf("Unfineshed Priority (%i) : ", unfinesedPriorityCount);
	for(unsigned int t = 0; t < unfinesedPriorityCount; ++t){
		cout << unfinesedPriority[t] << " ";
	}
	cout << "\n";

	cout << "Number of Switchs: " << evalSol << "\n";
	cout << "Number of Switchs Instances: " << sumChanges << "\n";

	int cost = (PROFITYPRIORITY * fineshedPriorityCount) - (COSTSWITCH * evalSol) - (COSTSWITCHINSTANCE * sumChanges) - (COSTPRIORITY * unfinesedPriorityCount);
	printf("(%i * %i) - (%i * %i) - (%i * %i) - (%i * %i) = %i\n", PROFITYPRIORITY, fineshedPriorityCount, COSTSWITCH, evalSol, COSTSWITCHINSTANCE, sumChanges, COSTPRIORITY, unfinesedPriorityCount, cost);
	#else
	int cost = (PROFITYPRIORITY * fineshedPriority) - (COSTSWITCH * evalSol) - (COSTSWITCHINSTANCE * sumChanges) - (COSTPRIORITY * unfinesedPriority);
	#endif

	return cost;
}

unsigned int KTNS(vector<int> s){
	
	// Variaveis
	vector<bool> magazineL(numberTools, true);	
	unsigned int evalSol = 0; 
	int jL;

	int sumChanges = 0; // Conta quantas trocas de instancia foram feitas, quando pelo menos uma troca de ferramenta foi trocada do magazine
	int currantChanges = 0; // Conta quantas trocas de ferramenta foram feitas, no job atual
	int currantMinute = 0; // Conta quantas horas ja foram usadas no dia atual  		       
	int currantDay = 0; // Conta quantos dias ja foram usados no horizonte de planejamento 

	int fineshedPriority = 0; // Jobs prioritarios que foram feitos
	int unfinesedPriority = 0; // Jobs prioritarios que foram feitos

	numberJobs = s.size();

	// Percorre todas as ferramentas
	for(jL= 0; jL < numberJobs; ++jL){
		
		currantChanges = 0;

		vector<bool> magazineCL(numberTools);		
		int left = jL;
		int cmL = 0;

		while((cmL < capacityMagazine) && (left < numberJobs)){
			for (auto it=JobTools[s[left]].begin(); ((it!=JobTools[s[left]].end()) && (cmL < capacityMagazine)); ++it){
				if((magazineL[*it]) && (!magazineCL[*it])){
					magazineCL[*it] = true;
					++cmL;
				}else if((jL == left) && (!magazineCL[*it])){
					magazineCL[*it] = true;
					++cmL;
					++currantChanges;
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

		if (jL == 0) currantChanges = capacityMagazine;

		// ---------------------------------------------------------------------------

		// verifica se a hora é sem supervisao e se houve troca de ferramenta
		if ((currantMinute >= unsupervised) && (currantChanges > 0)){
			//Verificar de consigo acabar essa tarefa antes de exceder o horizonte de planejamento
			if(currantDay + 1 > planingHorizon){
				// Contar quantar tarefas prioritarias faltaram
				for(unsigned int v = jL; v < numberJobs; ++v){
					unfinesedPriority += priority[s[v]];
				}
				// Pode sair do loop 
				break;
			}
			else{
				currantDay++;
				currantMinute = 0;
			}
		}

		//Tarefa vai vazar para o proximo dia
		if (currantMinute + processingTime[s[jL]] > 1440){
			//Verificar de consigo acabar essa tarefa antes de exceder o horizonte de planejamento
			if(currantDay + 1 > planingHorizon){
				// Contar quantar tarefas prioritarias faltaram
				for(unsigned int v = jL; v < numberJobs; ++v){
					unfinesedPriority += priority[s[v]];
				}
				// Pode sair do loop 
				break;
			}
			else{
				currantDay++;
				// Acumular as horas que vazaram para o dia seguinte
				currantMinute += (currantMinute + processingTime[s[jL]])-1440;
			}
		}
		//Tarefa pode continuar no mesmo dia
		else{
			currantMinute += processingTime[s[jL]];
		}

		// ---------------------------------------------------------------------------

		evalSol += currantChanges;
		if(currantChanges > 0) ++sumChanges;
		fineshedPriority += priority[s[jL]];

		// ---------------------------------------------------------------------------

	}

	int cost = (PROFITYPRIORITY * fineshedPriority) - (COSTSWITCH * evalSol) - (COSTSWITCHINSTANCE * sumChanges) - (COSTPRIORITY * unfinesedPriority);

	return cost;
}

// ------------------------------------------------------------------------------------------------------------------------------------------------------
// GPCA
// ------------------------------------------------------------------------------------------------------------------------------------------------------

unsigned int GPCA(vector<int> s){	
	int pipes_count = 0;
	int last_full = 0;
	vector<int> last_seen(numberTools);
	vector<vector<int>> M;

	int sumChanges = 0;
	bool change = false;

	//Completa o last_seen
	for(unsigned int i = 0; i < numberTools; ++i){
		if(toolJob[i][s[0]]) last_seen[i] = 0;
		else last_seen[i] = -1;
	}
	M.push_back(JobTools[s[0]]);
	
	
	for(unsigned int e = 1; e < numberJobs; ++e){
		
		M.push_back(JobTools[s[e]]);
		change = false;
		
		//print M
		// for(unsigned int t = 0; t < M.size(); ++t){
		// 	cout<<"M["<<t<<"]:";
		// 	for(unsigned int n = 0; n < M[t].size(); ++n){
		// 		cout<<M[t][n]<<" ";
		// 	}
		// 	cout<<"\n";
		// }
		// cout<<"\n";

		for (auto t = JobTools[s[e]].begin(); t != JobTools[s[e]].end(); ++t){
			
			if(last_full <= last_seen[*t]){
				
				++pipes_count;
				change = true;
				
				// cout<<"PIPE last_see:"<< last_seen[*t] <<" Atual:"<< e <<" last_full:"<<last_full<<"\n";
				
				for(unsigned int i = (last_seen[*t]+1); i < e; ++i){
					
					M[i].push_back(*t);					
					if(M[i].size() == capacityMagazine) last_full = i;
				}
			}
			last_seen[*t] = e; 	
		}
		if (change) ++sumChanges;
		
		if(M[e].size() == capacityMagazine) last_full = e;

	}

	return (PROFITYPRIORITY * s.size()) - (COSTSWITCHINSTANCE * sumChanges) - (COSTSWITCH * pipes_count);
}

// ------------------------------------------------------------------------------------------------------------------------------------------------------
// GPCA Paralelo
// ------------------------------------------------------------------------------------------------------------------------------------------------------

unsigned int GPCAPar(vector<int> s){
	
	meio = numberJobs/2;
	neigh = s;
	fase = false;


	int pipes_count = 0;
	int last_full = 0;
	vector<int> last_seen(numberTools);
	vector<vector<int>> M;

	int sumChanges = 0;
	bool change = false;
	

	//Completa o last_seen
	for(unsigned int i = 0; i < numberTools; ++i){
		if(toolJob[i][s[0]]) last_seen[i] = 0;
		else last_seen[i] = -1;
	}
	M.push_back(JobTools[s[0]]);
	
	
	for(unsigned int e = 1; e <= meio; ++e){
		change = false;
		
		M.push_back(JobTools[s[e]]);

		// print M
		// for(unsigned int t = 0; t < M.size(); ++t){
		// 	cout<<"M["<<t<<"]:";
		// 	for(unsigned int n = 0; n < M[t].size(); ++n){
		// 		cout<<M[t][n]<<" ";
		// 	}
		// 	cout<<"\n";
		// }
		// cout<<"\n";
				
		for (auto t = JobTools[s[e]].begin(); t != JobTools[s[e]].end(); ++t){
			
			if(last_full <= last_seen[*t]){
				
				++pipes_count;
				change = true;
				
			//	cout<<"PIPE last_see:"<< last_seen[*t] <<" Atual:"<< e <<" last_full:"<<last_full<<"\n";
				
				for(unsigned int i = (last_seen[*t]+1); i < e; ++i){
					
					M[i].push_back(*t);					
					if(M[i].size() == capacityMagazine) last_full = i;
				}
			}
			last_seen[*t] = e; 	
		}

		if (change) ++sumChanges;
		
		if(M[e].size() == capacityMagazine) last_full = e;
	}

	
	while(!endD){}
	endD = false;


	for(unsigned int e = (meio+1); e <= last_fullD; ++e){
		change = false;
		
		M.push_back(MD[((numberJobs-1) - e)]);
				
		for (auto t = JobTools[s[e]].begin(); t != JobTools[s[e]].end(); ++t){
			
			if((last_full <= last_seen[*t]) && (last_seen[*t] != meio)){
				
				++pipes_count;
				change = true;
								
				for(unsigned int i = (last_seen[*t]+1); i < e; ++i){
					
					M[i].push_back(*t);					
					if(M[i].size() == capacityMagazine) last_full = i;
				}
			last_seen[*t] = -1; 	
			}
		}
		if (change) ++sumChanges;
		
		if(M[e].size() == capacityMagazine) last_full = e;
	}


	return (PROFITYPRIORITY * s.size()) - (COSTSWITCHINSTANCE * sumChanges) - (COSTSWITCH * pipes_count);
 
 
}

void threadRunGPCA(){

	while(true){
		if(!fase){			

			int pipes_count = 0;
			int last_full = (numberJobs-1);
			vector<int> last_seen(numberTools);
			vector<vector<int>> M;
	

			//Completa o last_seen
			for(unsigned int i = 0; i < numberTools; ++i){
				if(toolJob[i][neigh[(numberJobs-1)]]) last_seen[i] = (numberJobs-1);
				else last_seen[i] = numberJobs;
			}
			
			M.push_back(JobTools[neigh[(numberJobs-1)]]);
	
	
			for(unsigned int e = (numberJobs-2); e >= meio; --e){
		
				M.push_back(JobTools[neigh[e]]);
				
				for (auto t = JobTools[neigh[e]].begin(); t != JobTools[neigh[e]].end(); ++t){
			
					if(last_full >= last_seen[*t]){
				
						++pipes_count;
								
						for(unsigned int i = (last_seen[*t]-1); i > e; --i){
					
							M[((numberJobs-1) - i)].push_back(*t);					
							if(M[((numberJobs-1) - i)].size() == capacityMagazine) last_full = i;
						}
					}
					last_seen[*t] = e; 	
				}
		
				if(M[((numberJobs-1) - e)].size() == capacityMagazine) last_full = e;
			}

			
			last_fullD = last_full;
			last_seenD = last_seen;
			MD = M;
			pipes_countD = pipes_count;

			fase = true;			
			endD = true;
		}
		
		if(stop) break;
	}
}

// ------------------------------------------------------------------------------------------------------------------------------------------------------
// COST
// ------------------------------------------------------------------------------------------------------------------------------------------------------

unsigned int printSolutionReport(vector<int> sol){
	
	vector<vector<int>> sols(numberMachines);
	int totalCost = 0;

	cout << "\n------------------------------------------------------------------------------------------\n";
	cout << "SOLUTION";
	cout << "\n------------------------------------------------------------------------------------------\n\n";

	cout << "Solution: " << endl;
    for (const auto &p : sol)
        cout << p << " ";

	cout << endl;

	cout << "Machine: " << endl;
    for (const auto &p : machine)
        cout << p << " ";

	cout << endl;

    for (int i = 0; i < sol.size(); ++i){
        sols[machine[sol[i]]].push_back(sol[i]);
    }

    for (int i = 0; i < sols.size(); ++i) {
        cout << "\n------------------------------------------------------------------------------------------\n";
        cout << "Machine: " << i << " = ";
        for (int j = 0; j < sols[i].size(); ++j)
            cout << sols[i][j] << " ";
        cout << "\n------------------------------------------------------------------------------------------\n\n";

        totalCost += KTNSReport(sols[i]);
    }

	cout << "\n------------------------------------------------------------------------------------------\n";
	cout << "ALL DONE, TOTAL COST: " << totalCost;
	cout << "\n------------------------------------------------------------------------------------------\n";

	return totalCost;

}

unsigned int printSolutionReportIfDef(vector<int> sol){
	
	vector<vector<int>> sols(numberMachines);
	int totalCost = 0;

	#ifdef DEBUG
	cout << "\n------------------------------------------------------------------------------------------\n";
	cout << "SOLUTION";
	cout << "\n------------------------------------------------------------------------------------------\n\n";

	cout << "Solution: " << endl;
    for (const auto &p : sol)
        cout << p << " ";

	cout << endl;

	cout << "Machine: " << endl;
    for (const auto &p : machine)
        cout << p << " ";

	cout << endl;
	#endif

    for (int i = 0; i < sol.size(); ++i){
        sols[machine[sol[i]]].push_back(sol[i]);
    }

    for (int i = 0; i < sols.size(); ++i) {
		#ifdef DEBUG
        cout << "\n------------------------------------------------------------------------------------------\n";
        cout << "Machine: " << i << " = ";
        for (int j = 0; j < sols[i].size(); ++j)
            cout << sols[i][j] << " ";
        cout << "\n------------------------------------------------------------------------------------------\n\n";
		#endif

        totalCost += KTNSReport(sols[i]);
    }

	#ifdef DEBUG
	cout << "\n------------------------------------------------------------------------------------------\n";
	cout << "ALL DONE, TOTAL COST: " << totalCost;
	cout << "\n------------------------------------------------------------------------------------------\n";
	#endif

	return totalCost;

}

unsigned int cost(vector<int> sol){
	
	vector<vector<int>> sols(numberMachines);
	int totalCost = 0;

    for (int i = 0; i < sol.size(); ++i){
        sols[machine[sol[i]]].push_back(sol[i]);
    }

    for (int i = 0; i < sols.size(); ++i) {
        totalCost += KTNS(sols[i]);
    }

	return totalCost;

}