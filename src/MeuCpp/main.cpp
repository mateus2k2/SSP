#include <vector>
#include <iostream>
#include <atomic>
#include <thread>
using namespace std;

/* 
g++ src/MeuCpp/code.cpp -o src/MeuCpp/code -Wall
./src/MeuCpp/code
*/

// ------------------------------------------------------------------------------------------------------------------------------------------------------
// VARIVEIS GLOBAIS
// ------------------------------------------------------------------------------------------------------------------------------------------------------

int numberMachines = 1;		   // Nuemro de maquinas
int numberTools = 20;      	   // Numero de Ferramentas diferentes 
int capacityMagazine = 8;  	   // Capacidade do magazine das maquina
int numberJobs = 10; 	   	   // Numero de Jobs
vector<vector<int>> JobTools;  // Vetor de jobs com cada ferramenta que ele usa
vector<vector<bool>> toolJob;

unsigned int sum = 0;   	   // auxiliary of GPCA
int meio = 0 ; 				   // auxiliary of KTNS PAR
vector<int> neigh; 			   // auxiliary of KTNS PAR
atomic_bool fase{true}; 	   // auxiliary of KTNS PAR
atomic_bool endD{false};	   // auxiliary of KTNS PAR
atomic_bool stop{false};	   // auxiliary of KTNS PAR
vector<bool> magazineD;		   // auxiliary of KTNS PAR
int evalSolD;				   // auxiliary of KTNS PAR
thread tKT;					   // auxiliary of GPCA PAR
int last_fullD;				   // auxiliary of GPCA PAR
vector<int> last_seenD;		   // auxiliary of GPCA PAR
vector<vector<int>> MD;		   // auxiliary of GPCA PAR
int pipes_countD = 0;		   // auxiliary of GPCA PAR


// ------------------------------------------------------------------------------------------------------------------------------------------------------
// LOAD DATA
// ------------------------------------------------------------------------------------------------------------------------------------------------------
void makeJobTools(){
	JobTools.resize(numberTools);
	for(int i=0; i < numberTools; ++i){
		JobTools[i].resize(numberJobs);
	}

	// JobTools[0] = {1,2,3,4,5}; 			 // (1,1) 
	// JobTools[1] = {1,2,3,4,5};		     // (1,2)
	// JobTools[2] = {12,13,14,15,16,17,18}; // (2,1)
	// JobTools[3] = {4,5,8,9,10,11,12,13};  // (3,1)
	// JobTools[4] = {4,5,8,9,10,11,12,13};  // (3,2)
	// JobTools[5] = {12,13,14,15,16,17,18}; // (4,1)
	// JobTools[6] = {12,13,14,15,16,17,18}; // (4,2)
	// JobTools[7] = {5,6,7};			  	 // (5,1)	
	// JobTools[8] = {15,16,17,18,19,20};    // (6,1)
	// JobTools[9] = {1,2,3,4,5}; 			 // (7,1)
	
	JobTools[0] = {0,1,2,3,4}; 			  // (1,1) 
	JobTools[1] = {0,1,2,3,4};		      // (1,2)
	JobTools[2] = {11,12,13,14,15,16,17}; // (2,1)
	JobTools[3] = {3,4,7,8,9,10,11,12};   // (3,1)
	JobTools[4] = {3,4,7,8,9,10,11,12};   // (3,2)
	JobTools[5] = {11,12,13,14,15,16,17}; // (4,1)
	JobTools[6] = {11,12,13,14,15,16,17}; // (4,2)
	JobTools[7] = {4,5,6};			  	  // (5,1)	
	JobTools[8] = {14,15,16,17,18,19};    // (6,1)
	JobTools[9] = {0,1,2,3,4}; 			  // (7,1) 

}

void makeToolJob(){
	toolJob.resize(numberJobs);
	for(int i=0; i < numberJobs; ++i){
		toolJob[i].resize(numberTools);
	}

	JobTools[0]  = {1,1,0,0,0,0,0,0,0,1};
	JobTools[1]  = {1,1,0,0,0,0,0,0,0,1};
	JobTools[2]  = {1,1,0,0,0,0,0,0,0,1};
	JobTools[3]  = {1,1,0,1,1,0,0,0,0,1};
	JobTools[4]  = {1,1,0,1,1,0,0,1,0,1};
	JobTools[5]  = {0,0,0,0,0,0,0,1,0,0};
	JobTools[6]  = {0,0,0,0,0,0,0,1,0,0};
	JobTools[7]  = {0,0,0,1,1,0,0,0,0,0};
	JobTools[8]  = {0,0,0,1,1,0,0,0,0,0};
	JobTools[9]  = {0,0,0,1,1,0,0,0,0,0};
	JobTools[10] = {0,0,0,1,1,0,0,0,0,0};
	JobTools[11] = {0,0,1,1,1,1,1,0,0,0};
	JobTools[12] = {0,0,1,1,1,1,1,0,0,0};
	JobTools[13] = {0,0,1,0,0,1,1,0,0,0};
	JobTools[14] = {0,0,1,0,0,1,1,0,1,0};
	JobTools[15] = {0,0,1,0,0,1,1,0,1,0};
	JobTools[16] = {0,0,1,0,0,1,1,0,1,0};
	JobTools[17] = {0,0,1,0,0,1,1,0,1,0};
	JobTools[18] = {0,0,0,0,0,0,0,0,1,0};
	JobTools[19] = {0,0,0,0,0,0,0,0,1,0};

}

// ------------------------------------------------------------------------------------------------------------------------------------------------------
// KTNS
// ------------------------------------------------------------------------------------------------------------------------------------------------------

unsigned int KTNS(vector<int> s){
	
    // Variaveis
	vector<bool> magazineL(numberTools, true);	
	unsigned int evalSol = 0; 
	int jL;

	// Percorre todas as ferramentas
	for(jL= 0; jL < numberJobs; ++jL){
		
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
					++evalSol;
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
		
		magazineL = magazineCL;
		
	}

	return evalSol;
}

unsigned int GPCAPar(vector<int> s){
	
	meio = numberJobs/2;
	neigh = s;
	fase = false;


	int pipes_count = 0;
	int last_full = 0;
	vector<int> last_seen(numberTools);
	vector<vector<int>> M;
	

	//Completa o last_seen
	for(unsigned int i = 0; i < numberTools; ++i){
		if(toolJob[i][s[0]]) last_seen[i] = 0;
		else last_seen[i] = -1;
	}
	M.push_back(JobTools[s[0]]);
	
	
	for(unsigned int e = 1; e <= meio; ++e){
		
		M.push_back(JobTools[s[e]]);
				
		for (auto t = JobTools[s[e]].begin(); t != JobTools[s[e]].end(); ++t){
			
			if(last_full <= last_seen[*t]){
				
				++pipes_count;
				
			//	cout<<"PIPE last_see:"<< last_seen[*t] <<" Atual:"<< e <<" last_full:"<<last_full<<"\n";
				
				for(unsigned int i = (last_seen[*t]+1); i < e; ++i){
					
					M[i].push_back(*t);					
					if(M[i].size() == capacityMagazine) last_full = i;
				}
			}
			last_seen[*t] = e; 	
		}
		
		if(M[e].size() == capacityMagazine) last_full = e;
	}

	
	while(!endD){}
	endD = false;


	for(unsigned int e = (meio+1); e <= last_fullD; ++e){
		
		M.push_back(MD[((numberJobs-1) - e)]);
				
		for (auto t = JobTools[s[e]].begin(); t != JobTools[s[e]].end(); ++t){
			
			if((last_full <= last_seen[*t]) && (last_seen[*t] != meio)){
				
				++pipes_count;
								
				for(unsigned int i = (last_seen[*t]+1); i < e; ++i){
					
					M[i].push_back(*t);					
					if(M[i].size() == capacityMagazine) last_full = i;
				}
			last_seen[*t] = -1; 	
			}
		}
		
		if(M[e].size() == capacityMagazine) last_full = e;
	}


 return (sum - capacityMagazine - (pipes_count+pipes_countD));
 
 
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
// TESTE SOLUTION
// ------------------------------------------------------------------------------------------------------------------------------------------------------

int main(){
	vector<int> s(numberJobs);
	s = {0,1,2,3,4,5,6,7,8,9};
    makeJobTools();
	makeToolJob();

	tKT = thread(threadRunGPCA);
	unsigned int GPCAParSum = GPCAPar(s);
	stop = true;
	tKT.join();

	unsigned int KTNSSum = KTNS(s);

	cout << "Solucao GPSAPar: " << GPCAParSum << endl;
    cout << "Solucao KTNS: "    << KTNSSum 	  << endl;

    return 0;
}