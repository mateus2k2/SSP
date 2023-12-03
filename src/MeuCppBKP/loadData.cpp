#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <algorithm> 
#include <unordered_set>

#include "headers/loadData.h"
#include "headers/GlobalVars.h"
#include <set>

using namespace std;

// ------------------------------------------------------------------------------------------------------------------------------------------------------
// LOAD DUMMY
// ------------------------------------------------------------------------------------------------------------------------------------------------------


void loadInstance(string filename){
    vector<vector<bool>> toolJob;
    // Vetor de ferramentas com cada job que ela pode fazer
    // Era Global Mas tirei
    
    //Codigo para converter o JobTools para toolJob
    // toolJob.resize(numberTools);
    // for(int i=0; i < numberTools; ++i){
    //     toolJob[i].resize(numberJobs);
    // }

    // for(int i=0; i < numberJobs; ++i){
    //     for(int j=0; j < JobTools[i].size(); ++j){
    //         toolJob[JobTools[i][j]-1][i] = true;
    //     }
    // }

	
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
					//  ++sum;
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
        JobToolsIndex.push_back(tmpToolSetIndex);

        getline(ss, value, '\n');
        processingTime.push_back(stoi(value));
    }

    file.close();

    // ------------------------------------------------------------------------------------------------------------------------------------------------------

    // Numbero total de jobs
    numberJobs = job.size();    

    // Capacidade do magazine, mapacidade do maior toolSet
    auto maxIt = max_element(JobTools.begin(), JobTools.end(),
        [](const vector<int>& a, const vector<int>& b) {
            return a.size() < b.size();
        });
    capacityMagazine = maxIt->size();

    // Numero de ferramentas;
    set <int> allTools;
    for(int i = 0; i < jobsType.size(); i++){
        for(int j = 0; j < jobsType[i].JobTools.size(); j++){
            allTools.insert(jobsType[i].JobTools[j]);
        }
    }
    numberTools = allTools.size();

    // ------------------------------------------------------------------------------------------------------------------------------------------------------

    planingHorizon = 7;   //ToDo Nunca Muda? 		  
    unsupervised   = 720; //ToDo Nunca Muda? 			 
    numberMachines = 0;   //ToDo Da onde Carregar?
    priority = {};        //ToDo Da onde Carregar?
    machine  = {};        //ToDo Parte da Solução
    
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
                lineData.push_back(stoi(value));
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

void printDataReport() {
    cout << "\n------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------" << endl;
    cout << "DATA REPORT" << endl;
    cout << "------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------\n" << endl;

    cout << "Number of Machines: " << numberMachines << endl;
    cout << "Number of Tools: " << numberTools << endl;
    cout << "Number of Jobs: " << numberJobs << endl;
    cout << "Capacity of Magazine: " << capacityMagazine << endl;
    cout << "Planing Horizon: " << planingHorizon << endl;
    cout << "Unsupervised: " << unsupervised << endl;
    
    cout << endl << endl;
    
    for (const auto& pair : mapToolSets) {
        cout << "Key: " << pair.first << ", Values: ";
        for (const auto& value : pair.second) {
            cout << value << " ";
        }
        cout << endl;
    }

    cout << endl << endl;

    cout << "Priority: " << endl;
    for (const auto &p : priority) {
        cout << p << " ";
    }
    
    cout << endl;
    
    cout << "Operation: " << endl;
    for (const auto &op : operation) {
        cout << op << " ";
    }
    cout << endl;

    cout << "Job: " << endl;
    for (const auto &j : job) {
        cout << j << " ";
    }
    cout << endl;

    cout << "Processing Time: " << endl;
    for (const auto &pt : processingTime) {
        cout << pt << " ";
    }

    cout << endl << endl << endl;

    cout << "JobTools: " << endl;
    for (const auto &ts : JobTools) {
        for (const auto &t : ts) {
            cout << t << " ";
        }
        cout << endl;
    }

    cout << "\n------------------------------------------------------------------------------------------" << endl;
    cout << "JOBS DATA TYPE" << endl;
    cout << "------------------------------------------------------------------------------------------\n" << endl;

    for (int i = 0; i < jobsType.size(); i++){
        cout << "Job: " << jobsType[i].indexJob << "\nOperation: " << jobsType[i].indexOperation << "\nToolSet: " << jobsType[i].indexToolSet << "\nProcessingTime: " << jobsType[i].processingTime << "\nPriority: " << jobsType[i].priority << "\nMachine: " << jobsType[i].indexMachine << "\nJobTools: ";
        
        for(int j = 0; j < jobsType[i].JobTools.size(); j++){
            cout << jobsType[i].JobTools[j] << " ";
        }

        cout << "\n";

        cout << "superJob: " << jobsType[i].superJob << "\noriginalJobs: ";
        for(int j = 0; j < jobsType[i].originalJobs.size(); j++){
            cout << jobsType[i].originalJobs[j] << " ";
        }

        cout << "\n\n";
    }

    cout << "------------------------------------------------------------------------------------------" << endl;
    cout << "TOOL SET TYPE" << endl;
    cout << "------------------------------------------------------------------------------------------\n" << endl;

    for(int i = 0; i < ToolSetsType.size(); i++){
        cout << "ToolSet: " << ToolSetsType[i].indexToolSet << "\nTools: ";

        for(int j = 0; j < ToolSetsType[i].tools.size(); j++){
            cout << ToolSetsType[i].tools[j] << " ";
        }
        
        cout << "\n";

        cout << "superToolSet: " << ToolSetsType[i].superToolSet << "\noriginalToolSets: ";
        for(int j = 0; j < ToolSetsType[i].originalToolSets.size(); j++){
            cout << ToolSetsType[i].originalToolSets[j] << " ";
        }
        
        cout << "\n\n";
    }


    cout << "\n------------------------------------------------------------------------------------------" << endl;
    cout << "JOBS DATA TYPE EXCLUIDOS" << endl;
    cout << "------------------------------------------------------------------------------------------\n" << endl;

    for (int i = 0; i < jobsTypeDeletados.size(); i++){
        cout << "Job: " << jobsTypeDeletados[i].indexJob << "\nOperation: " << jobsTypeDeletados[i].indexOperation << "\nToolSet: " << jobsTypeDeletados[i].indexToolSet << "\nProcessingTime: " << jobsTypeDeletados[i].processingTime << "\nPriority: " << jobsTypeDeletados[i].priority << "\nMachine: " << jobsTypeDeletados[i].indexMachine << "\nJobTools: ";
        
        for(int j = 0; j < jobsTypeDeletados[i].JobTools.size(); j++){
            cout << jobsTypeDeletados[i].JobTools[j] << " ";
        }

        cout << "\n\n";
    }

    cout << "------------------------------------------------------------------------------------------" << endl;
    cout << "TOOL SET TYPE EXCLUIDOS" << endl;
    cout << "------------------------------------------------------------------------------------------\n" << endl;

    for(int i = 0; i < ToolSetsTypeDeletados.size(); i++){
        cout << "ToolSet: " << ToolSetsTypeDeletados[i].indexToolSet << "\nTools: ";

        for(int j = 0; j < ToolSetsTypeDeletados[i].tools.size(); j++){
            cout << ToolSetsTypeDeletados[i].tools[j] << " ";
        }
        
        cout << "\n\n";
    }
}

