#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <algorithm> 

#include "headers/loadData.h"
#include "headers/GlobalVars.h"

using namespace std;

// ------------------------------------------------------------------------------------------------------------------------------------------------------
// LOAD DUMMY
// ------------------------------------------------------------------------------------------------------------------------------------------------------


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
		cout << "Could not open the file! \n";
	}
}

int laodRealInstance(string filename){
    ifstream file(filename);

    if (!file.is_open()) {
        cerr << "Error opening file!" << endl;
        return 1;
    }

    string line;
	getline(file, line);
    while (getline(file, line)) {
        stringstream ss(line);
        string value;

        getline(ss, value, ';');
        operation.push_back(stoi(value));

        getline(ss, value, ';');
        job.push_back(stoi(value));

        getline(ss, value, ';');
        toolSet.push_back(stoi(value));

        getline(ss, value, '\n');
        processingTime.push_back(stoi(value));
    }

    file.close();

    return 0;
}

int laodToolSet(string filename) {
    ifstream file(filename);

    if (!file.is_open()) {
        cerr << "Error opening file!" << endl;
        return 1;
    }

    string line;
    while (getline(file, line)) {
        stringstream ss(line);
        string value;
        vector<int> lineData;

		getline(ss, value, ';');
        while (getline(ss, value, ';')) {
			if (!value.empty()){
                lineData.push_back(stoi(value));
			}
			else{
				break;
			}
        }

        toolSets.push_back(lineData);
    }

    file.close();

    return 0;
}


// ------------------------------------------------------------------------------------------------------------------------------------------------------
// LOAD DUMMY
// ------------------------------------------------------------------------------------------------------------------------------------------------------


void makeJobTools(){
	JobTools.resize(numberTools);
	for(int i=0; i < numberTools; ++i){
		JobTools[i].resize(numberJobs);
	}

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
	toolJob.resize(numberTools);
	for(int i=0; i < numberJobs; ++i){
		toolJob[i].resize(numberJobs);
	}

	toolJob[0]  = {1,1,0,0,0,0,0,0,0,1};
	toolJob[1]  = {1,1,0,0,0,0,0,0,0,1};
	toolJob[2]  = {1,1,0,0,0,0,0,0,0,1};
	toolJob[3]  = {1,1,0,1,1,0,0,0,0,1};
	toolJob[4]  = {1,1,0,1,1,0,0,1,0,1};
	toolJob[5]  = {0,0,0,0,0,0,0,1,0,0};
	toolJob[6]  = {0,0,0,0,0,0,0,1,0,0};
	toolJob[7]  = {0,0,0,1,1,0,0,0,0,0};
	toolJob[8]  = {0,0,0,1,1,0,0,0,0,0};
	toolJob[9]  = {0,0,0,1,1,0,0,0,0,0};
	toolJob[10] = {0,0,0,1,1,0,0,0,0,0};
	toolJob[11] = {0,0,1,1,1,1,1,0,0,0};
	toolJob[12] = {0,0,1,1,1,1,1,0,0,0};
	toolJob[13] = {0,0,1,0,0,1,1,0,0,0};
	toolJob[14] = {0,0,1,0,0,1,1,0,1,0};
	toolJob[15] = {0,0,1,0,0,1,1,0,1,0};
	toolJob[16] = {0,0,1,0,0,1,1,0,1,0};
	toolJob[17] = {0,0,1,0,0,1,1,0,1,0};
	toolJob[18] = {0,0,0,0,0,0,0,0,1,0};
	toolJob[19] = {0,0,0,0,0,0,0,0,1,0};

}


/* 

/home/mateus/WSL/IC/dataRaw/KTNS/Instances/Catanzaro/Tabela1/datA1
10
10
4
0 0 1 1 0 1 0 0 1 1 
0 1 0 0 0 0 0 0 0 1 
0 1 0 0 0 1 0 0 0 0 
0 0 0 0 0 0 0 0 1 1 
0 0 0 1 1 0 1 1 1 0 
1 0 1 0 0 1 1 0 0 0 
1 0 0 0 0 0 0 1 0 0 
0 0 0 0 1 1 1 1 1 0 
0 0 1 1 0 0 1 0 0 0 
0 0 0 0 1 0 0 0 0 1 


/home/mateus/WSL/IC/dataRaw/KTNS/Instances/Catanzaro/Tabela1/datB9
15
20
6
0 0 0 1 0 0 0 1 0 1 0 0 1 0 0 
0 0 1 1 0 0 0 0 0 0 1 0 1 0 0 
0 0 0 0 0 1 0 1 0 0 0 0 0 0 0 
0 0 0 1 0 0 0 0 0 1 0 0 0 0 0 
0 0 0 0 0 1 0 0 1 0 1 0 0 0 0 
1 0 0 0 0 1 0 0 0 0 1 0 0 0 1 
0 0 0 1 0 0 0 0 1 1 0 0 0 0 1 
0 0 0 0 1 1 0 1 0 0 1 0 0 0 1 
0 0 1 0 0 0 0 0 0 0 0 0 0 0 0 
1 0 0 0 0 0 0 0 0 0 0 0 1 0 1 
0 0 0 0 1 0 0 0 0 0 0 0 1 1 0 
0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 
0 0 0 1 0 0 0 0 0 0 0 1 0 0 0 
0 0 0 1 0 0 1 0 0 0 0 0 0 0 0 
0 0 0 0 1 0 0 1 0 1 1 0 1 1 0 
0 1 0 0 1 0 1 0 0 0 0 1 0 1 0 
0 0 0 0 0 0 0 1 0 0 0 0 0 1 0 
0 0 0 0 0 0 0 1 0 0 0 0 0 1 0 
1 1 0 0 0 1 0 0 0 0 1 0 0 0 0 
0 0 0 0 1 0 0 0 0 0 0 0 0 0 1
*/