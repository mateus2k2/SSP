#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <algorithm> 
#include <unordered_set>

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

    numberJobs = job.size();    

    auto maxIt = max_element(JobTools.begin(), JobTools.end(),
        [](const vector<int>& a, const vector<int>& b) {
            return a.size() < b.size();
        });
    capacityMagazine = maxIt->size();

    int largestElement = numeric_limits<int>::min();
    for (const auto& innerVector : JobTools) {
        for (int element : innerVector) {
            largestElement = max(largestElement, element);
        }
    }
    numberTools = largestElement;

    planingHorizon = 7;   //ToDo 		 
    unsupervised   = 720; //ToDo 			 
    numberMachines = 0;   //ToDo
    priority = {};        //ToDo
    machine  = {};        //ToDo

    // ------------------------------------------------------------------------------------------------------------------------------------------------------

    toolJob.resize(numberTools);
    for(int i=0; i < numberTools; ++i){
        toolJob[i].resize(numberJobs);
    }

    for(int i=0; i < numberJobs; ++i){
        for(int j=0; j < JobTools[i].size(); ++j){
            toolJob[JobTools[i][j]-1][i] = true;
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

void printDataReport(){
    cout << "\n------------------------------------------------------------------------------------------" << endl;
    cout << "DATA REPORT" << endl;
    cout << "------------------------------------------------------------------------------------------\n" << endl;

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
    
    cout << endl << endl;

    cout << "toolJob: " << endl;
    for (const auto &ts : toolJob) {
        for (const auto &t : ts) {
            cout << t << " ";
        }
        cout << endl;
    }
}

