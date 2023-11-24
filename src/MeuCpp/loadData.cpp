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
    int tmpToolSetIndex;

    // ------------------------------------------------------------------------------------------------------------------------------------------------------

    numberMachines = 1;		   
    numberTools = 20;      	    
    numberJobs = 10; 	   	   
    capacityMagazine = 8; 

    // ------------------------------------------------------------------------------------------------------------------------------------------------------

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
        tmpToolSetIndex = stoi(value);
        JobTools.push_back(mapToolSets[tmpToolSetIndex]);


        getline(ss, value, '\n');
        processingTime.push_back(stoi(value));
    }

    file.close();

    // ------------------------------------------------------------------------------------------------------------------------------------------------------

    toolJob.resize(numberTools);
    for(int i=0; i < numberTools; ++i){
        toolJob[i].resize(numberJobs);
    }

    for(int i=0; i < numberJobs; ++i){
        for(int j=0; j < JobTools[i].size(); ++j){
            toolJob[JobTools[i][j]][i] = true;
        }
    }

    // ------------------------------------------------------------------------------------------------------------------------------------------------------
    
    return 0;
}

int laodToolSet(string filename) {
    ifstream file(filename);
    int tmpIndex;

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
        tmpIndex = stoi(value);
        while (getline(ss, value, ';')) {
			if (!value.empty()){
                lineData.push_back(stoi(value)-1);
			}
			else{
				break;
			}
        }

        mapToolSets.insert(pair<int, vector<int>>(tmpIndex, lineData));
    }

    file.close();

    return 0;
}

// ------------------------------------------------------------------------------------------------------------------------------------------------------

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