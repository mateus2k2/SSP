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

unsigned int KTNS(vector<int> s){
	
    // Variaveis
	vector<bool> magazineL(numberTools, true);	
	unsigned int evalSol = 0; 
	int jL;

	int sumChanges = 0;
	bool change = false;

	// Percorre todas as ferramentas
	for(jL= 0; jL < numberJobs; ++jL){
		
		vector<bool> magazineCL(numberTools);		
		int left = jL;
		int cmL = 0;
		change = false;

		while((cmL < capacityMagazine) && (left < numberJobs)){
			for (auto it=JobTools[s[left]].begin(); ((it!=JobTools[s[left]].end()) && (cmL < capacityMagazine)); ++it){
				if((magazineL[*it]) && (!magazineCL[*it])){
					magazineCL[*it] = true;
					++cmL;
				}else if((jL == left) && (!magazineCL[*it])){
					magazineCL[*it] = true;
					++evalSol;
					change = true;
					++cmL;
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
		
		if(change) ++sumChanges;

		magazineL = magazineCL;
		
		//print magazineCL
		// for(unsigned int n = 0; n < magazineCL.size(); ++n){
		// 	cout<<magazineCL[n]<<" ";
		// }
		
	}

	return (PROFITYPRIORITY * s.size()) - (COSTSWITCH * evalSol) - (COSTSWITCHINSTANCE * sumChanges);
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
// GPCA With Output
// ------------------------------------------------------------------------------------------------------------------------------------------------------

void GPCAOut(vector<int> s){

}