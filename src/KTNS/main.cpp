/*
 * main.cpp
 * 
 * Copyright 2023 André Almeida <andrealmeida@MacBook-Air-de-Andre.local>
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA 02110-1301, USA.
 * 
 * 
 */

#include <algorithm> 
#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdlib>
#include <random>
#include <atomic>
#include <string>
#include <thread>
#include <vector>
#include <iostream>
#include <fstream>
#include <queue>
#include <tuple>
#include <utility>
#include "./include/ExecTime.h"

using namespace std;


// Prototype
void loadInstance(string filename);
unsigned int KTNS(vector<int> s);
unsigned int KTNS_MECLER(vector<int> s);
unsigned int GPCA(vector<int> s);
pair<int,int> shortestPath(vector<int>* s, vector<bool>* m, vector<vector<int>>* d, vector<vector<pair<int,int>>>* , vector<vector<int>>* capacity);
unsigned int MinCostFlow(vector<int> s);
unsigned int KTNSPar(vector<int> s);
unsigned int fixSolution(vector<bool>* LK, vector<bool>* RK, int j);
void threadRun();
unsigned int GPCAPar(vector<int> s);
void threadRunGPCA();
unsigned int toFullMag(vector<int> s);






/****************************************** Variaveis Globais ******************************************/

	int numberJobs, numberTools, capacityMagazine;	
	std::vector<std::vector<bool>>  toolJob;
	std::vector<std::vector<int>>  JobTools; 
	
	//Jordana Mercler
	vector<vector<unsigned int> > L; ///< auxiliary matrix for KTNS
    vector<unsigned int> W_n; ///< auxiliary vector for KTNS
    vector<bool> used; ///< maps if tool is loaded in KTNS
    
    unsigned int sum = 0; /// auxiliary of GPCA
	
	int meio = 0 ; /// auxiliary of KTNS PAR
	vector<int> neigh; /// auxiliary of KTNS PAR
	// atomic_bool fase = true; /// auxiliary of KTNS PAR
	// atomic_bool endD = false; /// auxiliary of KTNS PAR
	// atomic_bool stop = false; /// auxiliary of KTNS PAR
	atomic_bool fase{true}; /// auxiliary of KTNS PAR
	atomic_bool endD{false}; /// auxiliary of KTNS PAR
	atomic_bool stop{false}; /// auxiliary of KTNS PAR
	vector<bool> magazineD;/// auxiliary of KTNS PAR
	int evalSolD;/// auxiliary of KTNS PAR
	thread tKT;
	
	int last_fullD;/// auxiliary of GPCA PAR
	vector<int> last_seenD;/// auxiliary of GPCA PAR
	vector<vector<int>> MD;/// auxiliary of GPCA PAR
	int pipes_countD = 0;/// auxiliary of GPCA PAR

	
/**************************************** Variaveis Globais FIM ****************************************/

int main(int argc, char **argv){
	
	// varible aux
	int certo = 0;
	string lineSol; 
    ifstream fnPermu;

	
	// Load config
	string fn = argv[1];
	int KTNS_Type = atoi(argv[2]);
			
	// Load instance	
	loadInstance(fn);
	
	// Aux Jordana
	vector<vector<unsigned int> > L_AUX(numberTools, vector<unsigned int>(numberJobs)); ///< auxiliary matrix for KTNS
    vector<unsigned int> W_n_AUX(numberTools); ///< auxiliary vector for KTNS
    vector<bool> used_AUX(numberTools); ///< maps if tool is loaded in KTNS
	L = L_AUX;
	used = used_AUX;
	W_n = W_n_AUX;
	
	// Aux KTNS Par
	if((KTNS_Type == 5) ||(KTNS_Type == 10)) tKT = thread(threadRun);
	if(KTNS_Type == 6) tKT = thread(threadRunGPCA);

	
	// Abre o arquivo com a instância
	fnPermu.open(argv[3]);
	if(!fnPermu.is_open()) cout<<"Could not open the permutation file! \n";
	// permutacoes
	vector<vector<int>> sol(2000000, vector<int>(numberJobs,0));
	
	for(unsigned i = 0; i < 2000000; i++){
		getline(fnPermu,lineSol);
		stringstream ss(lineSol);
		for(int c = 0; c < numberJobs; ++c){
			getline(ss ,lineSol, ' ');
			sol[i][c] = stoi(lineSol);
		}
	}

	//close file
	fnPermu.close();

	// Comeca a contar o tempo
	ExecTime et(fn);
		
	
	//2000000
	for(unsigned i = 0; i < 2000000; i++){

		// Qual KTNS rodar
		switch (KTNS_Type){
			case 1 : 				
				KTNS(sol[i]);
			break;
			case 2 : 
				KTNS_MECLER(sol[i]);
			break;
			case 3 : 
				GPCA(sol[i]);
				break;
			case 4 : 
				toFullMag(sol[i]);
				break;
			case 5 : 
				KTNSPar(sol[i]);
				break;
			case 6 : 
				GPCAPar(sol[i]);
				break;
			case 10 :
				{					
					unsigned int v1 = KTNSPar(sol[i]);
					unsigned int v2 = KTNS_MECLER(sol[i]);
		
					if(v1 == v2){
						++certo;
					}
					break;	
				}
		}
	

	}
	
	if(KTNS_Type == 10){
		cout <<fn<<" "<<((float)certo/(float)2000000)<<" "<<certo<<"\n";
	}
	
		// Aux KTNS Par
	if((KTNS_Type == 5)|| (KTNS_Type == 6) || (KTNS_Type == 10)){
		stop = true;
		tKT.join();
	}

	return 0;
}

void loadInstance(string filename){
	
	// Variaveis locais
	string line; 
    ifstream ifs;
	int i=0;
	int j=0;
	
	// Abre o arquivo com a instância
	ifs.open(filename);
			
	// Ler o arquivo
	if ( ifs.is_open()){
		getline(ifs,line);
		numberJobs = stoi(line); // Recupera a quantidade de tarefas

		getline(ifs,line);
		numberTools = stoi(line); // Recupera a quantidade de Ferramentas
		
		getline(ifs,line);
		capacityMagazine = stoi(line); // Recupera a capacidade do magazine
		
		// Redimensiona as variaveis 		
		toolJob.assign(numberTools, vector<bool>(numberJobs,false));
		JobTools.resize(numberJobs);
			
		// Preenche as variaveis	
		while(getline(ifs,line)){
			
			stringstream ss(line);
			while ( getline (ss ,line, ' ')){
				toolJob[i][j] = stoi(line);
				if(toolJob[i][j]){
					 JobTools[j].push_back(i);
					 ++sum;
				}
			j++;
			}
		j=0;
		i++;
		}
	// Fecha o arquivo	
	ifs.close();
	}else{
		std::cout << "Could not open the file! \n";
	}
}


/****************************************** KTNS VERSAO PROPOSTA ******************************************/

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

void threadRun(){

	while(true){
		if(!fase){			
			vector<bool> magazineL(numberTools, true);
			evalSolD = 0; 
			int jL;


			for(jL= (numberJobs-1); jL >= (meio+1); --jL){
		
				vector<bool> magazineCL(numberTools);		
				int left = jL;
				int cmL = 0;

				while((cmL < capacityMagazine) && (left >= 0)){
					for (auto it=JobTools[neigh[left]].begin(); ((it!=JobTools[neigh[left]].end()) && (cmL < capacityMagazine)); ++it){
						if((magazineL[*it]) && (!magazineCL[*it])){
							magazineCL[*it] = true;
							++cmL;
						}else if((jL == left) && (!magazineCL[*it])){

							magazineCL[*it] = true;
							if((!magazineL[*it]))++evalSolD;
							++cmL;
						}
					}
					--left;
				}
		
				left = jL -1;
				while((cmL < capacityMagazine) && (left >= 0)){
					for (auto it=JobTools[neigh[left]].begin(); ((it!=JobTools[neigh[left]].end()) && (cmL < capacityMagazine)); ++it){
						if((!magazineL[*it]) && (!magazineCL[*it])){
							magazineCL[*it] = true;
							++evalSolD;
							++cmL;
						}
					}
					--left;
				}
				
				magazineL = magazineCL;		
			}

			magazineD = magazineL;

			fase = true;			
			endD = true;
		}
		
		if(stop) break;
	}
}

unsigned int KTNSPar(vector<int> s){

	
	
	meio = numberJobs/2;
	neigh = s;
	fase = false;
	
	// Variaveis
	vector<bool> magazineL(numberTools, true);	
	unsigned int evalSol = 0; 
	int jL;

	// Percorre todas as ferramentas
	for(jL= 0; jL <= (meio-1); ++jL){
		
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
		
		left = jL+1;
		while((cmL < capacityMagazine) && (left < numberJobs)){
			for (auto it=JobTools[s[left]].begin(); ((it!=JobTools[s[left]].end()) && (cmL < capacityMagazine)); ++it){
				if((!magazineL[*it]) && (!magazineCL[*it])){
					magazineCL[*it] = true;
					++cmL;
					++evalSol;
				}
			}
		left++;
		}
		
		magazineL = magazineCL;
		
	}

	while(!endD){}
	endD = false;
		
	evalSol += evalSolD;
	evalSol += fixSolution(&magazineL,&magazineD,s[meio]);
	
	return evalSol;
}


unsigned int fixSolution(vector<bool>* LK, vector<bool>* RK, int j){

	int free = capacityMagazine - JobTools[j].size();
	vector<bool> magazine(numberTools, false); 	
	unsigned int add = 0;
	
	for(int c = 0;c < numberTools; ++c){
		if(!(*LK)[c] && toolJob[c][j]) ++add;
					
		if((*LK)[c] && (*RK)[c] && (!toolJob[c][j]) && (free > 0)){ 
			magazine[c] = true;
			--free;
		}
					
		if(toolJob[c][j]) magazine[c] = true;
	}
				
	for(int c = 0;c < numberTools; ++c){
		if(!magazine[c] && (*RK)[c]) ++add;
	}
	return add;
}

/**************************************** KTNS VERSAO PROPOSTA FIM ******************************************/

/****************************************** KTNS VERSAO PROPOSTA MECLER ******************************************/
unsigned int KTNS_MECLER(vector<int> s){
	
	// This method implements the KTNS policy
    // (keep tools needed soonest)
    // It is a dynamic programming approach to calculate
    // the number of tool switches given a fixed order of
    // jobs in polynomial time
    
    // Fills L matrix (auxiliary structure used by KTNS)
    for (int i = (numberJobs - 1); i >= 0; i--) {
        for (unsigned int j = 0; j < numberTools; j++) {
            if (i != -1 && toolJob[j][s[i]] == 1) {
                L[j][i] = (unsigned int)i;
            }
            else if (i < (int)numberJobs - 1) {
                L[j][i] = L[j][i + 1];
            }
            else {
                L[j][i] = numberJobs;
            }
            used[j] = false;
        }
    }

    unsigned int switches = 0;
    unsigned int capacity = 0;
    unsigned int tool = 0;
    double minVal;
    
    // Fills auxiliary vector W_n that corresponds to the
    // matrix row at instant n
    for (unsigned int i = 0; i < numberTools; i++) {
        if (L[i][0] == 0) {
            W_n[i] = 1;
            used[i] = true;
            capacity++;
        }
        else {
            W_n[i] = 0;
        }
    }
    
    while (capacity < capacityMagazine) {
        minVal = numeric_limits<double>::infinity();
        for (unsigned int i = 0; i < numberTools; i++) {
            if (!used[i] && (L[i][0] < minVal)) {
                tool = i;
                minVal = L[i][0];
            }
        }
        used[tool] = true;
        W_n[tool] = 1;
        capacity++;
    }
    
    //loadedMatrix[0] = W_n; 
    
    unsigned int maxVal;
    for (unsigned int n = 1; n < numberJobs; n++) {
        for (unsigned int i = 0; i < numberTools; i++) {
            if (W_n[i] != 1 && L[i][n] == n) {
                W_n[i] = 1;
                capacity++;
            }
        }
        while (capacity > capacityMagazine) {
            maxVal = n;
            for (unsigned int i = 0; i < numberTools; i++) {
                if (W_n[i] == 1 && L[i][n] > maxVal) {
                    tool = i;
                    maxVal = L[i][n];
                }
            }
            W_n[tool] = 0;
            capacity--;
            switches++;
        }
     //   loadedMatrix[n] = W_n;
    }
    
    return switches;   
    
}
/**************************************** KTNS VERSAO PROPOSTA MECLER FIM ******************************************/

/****************************************** GPCA PROPOSTA MIKHAIL ******************************************/

unsigned int GPCA(vector<int> s){	
	
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
	
	
	for(unsigned int e = 1; e < numberJobs; ++e){
		
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

 return (sum - capacityMagazine - pipes_count);
}

unsigned int toFullMag(vector<int> s){	
	
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
	
	
	for(unsigned int e = 1; e < numberJobs; ++e){
		
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
	
	vector<bool> usedG(numberTools, false);
	
	unsigned int i,j;
	for(j=1; j < numberJobs ; ++j){
		i = j - 1;
		
		for (auto t = M[j].begin(); t != M[j].end(); ++t){
			usedG[*t] = true;
		}
		
		for (auto t = M[i].begin(); t != M[i].end(); ++t){
			if((!usedG[*t]) && (M[j].size() < capacityMagazine)){
				M[j].push_back(*t);
			}
		}
		
		for (auto t = M[j].begin(); t != M[j].end(); ++t){
			usedG[*t] = false;
		}		 
	}
	
	for(i=(numberJobs-1); i > 0; --i){
		j = i - 1;
		
		for (auto t = M[j].begin(); t != M[j].end(); ++t){
			usedG[*t] = true;
		}
		
		for (auto t = M[i].begin(); t != M[i].end(); ++t){
			if((!usedG[*t]) && (M[j].size() < capacityMagazine)){
				M[j].push_back(*t);
			}
		}
		
		for (auto t = M[j].begin(); t != M[j].end(); ++t){
			usedG[*t] = false;
		}		 
	}
	
 return (sum - capacityMagazine - pipes_count);
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

/**************************************** GPCA PROPOSTA MIKHAIL FIM ******************************************/
