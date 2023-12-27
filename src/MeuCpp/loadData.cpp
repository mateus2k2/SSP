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

int laodInstance(string filename){
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
    for(int i = 0; i < JobTools.size(); i++){
        for(int j = 0; j < JobTools[i].size(); j++){
            allTools.insert(JobTools[i][j]);
        }
    }
    numberTools = allTools.size();
    
    // ------------------------------------------------------------------------------------------------------------------------------------------------------

    planingHorizon = 7;   //ToDo Nunca Muda? 		  
    unsupervised   = 12;  //ToDo Nunca Muda? 			 
    numberMachines = 0;   //ToDo Da onde Carregar?
    priority = {};        //ToDo Da onde Carregar?
    
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

void loadDataTypes(){

    for (auto it = mapToolSets.begin(); it != mapToolSets.end(); ++it) {
        ToolSet toolSetTmp;

        toolSetTmp.indexToolSet = it->first;
        toolSetTmp.tools = it->second;

        originalToolSets.push_back(toolSetTmp);
    }

    for (int i = 0; i < numberJobs; i++){
        Job jobTmp;

        jobTmp.indexJob = job[i];
        jobTmp.indexOperation = operation[i];
        
        // jobTmp.indexToolSet = JobToolsIndex[i];
        for (int j = 0; j < originalToolSets.size(); j++){
            if (originalToolSets[j].indexToolSet == JobToolsIndex[i]){
                jobTmp.indexToolSet = j;
                break;
            }
        }


        jobTmp.processingTime = processingTime[i];
        jobTmp.priority = priority[i];

        jobTmp.JobTools = JobTools[i];

        originalJobs.push_back(jobTmp);
    }

}

void printDataReport() {
    cout << "\n------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------" << endl;
    cout << "DATA REPORT" << endl;
    cout << "------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------\n" << endl;

    cout << "Number of Machines: " << numberMachines << endl;
    cout << "Capacity of Magazine: " << capacityMagazine << endl;
    cout << "Planing Horizon: " << planingHorizon << endl;
    cout << "Unsupervised: " << unsupervised << endl << endl;

    cout << "Number Of ToolSets Originais: " << originalToolSets.size() << endl;
    cout << "Number Of ToolSets Super: " << superToolSet.size() << endl;
    cout << "Number of Tools: " << numberTools << endl << endl;

    cout << "Number of Original Jobs: " << originalJobs.size() << endl;
    cout << "Number of Super Jobs: " << numberJobs << endl << endl;

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
    cout << "JOBS DATA" << endl;
    cout << "------------------------------------------------------------------------------------------\n" << endl;

    for (int i = 0; i < originalJobs.size(); i++){
        cout << "Job: " << originalJobs[i].indexJob << "\n";
        cout << "Operation: " << originalJobs[i].indexOperation << "\n";
        cout << "ToolSet: " << originalJobs[i].indexToolSet << "\n";
        cout << "ProcessingTime: " << originalJobs[i].processingTime << "\n";
        cout << "Priority: " << originalJobs[i].priority << "\n";
        cout << "JobTools: ";
        
        for(int j = 0; j < originalJobs[i].JobTools.size(); j++){
            cout << originalJobs[i].JobTools[j] << " ";
        }

        cout << "\n\n";
    }

    cout << "------------------------------------------------------------------------------------------" << endl;
    cout << "TOOL SET" << endl;
    cout << "------------------------------------------------------------------------------------------\n" << endl;

    for(int i = 0; i < originalToolSets.size(); i++){
        cout << "ToolSet: " << originalToolSets[i].indexToolSet << "\n"; 
        cout << "Tools: ";

        for(int j = 0; j < originalToolSets[i].tools.size(); j++){
            cout << originalToolSets[i].tools[j] << " ";
        }
        
        cout << "\n\n";
    }

    cout << "------------------------------------------------------------------------------------------" << endl;
    cout << "SUPER JOBS" << endl;
    cout << "------------------------------------------------------------------------------------------\n" << endl;

    for(int i = 0; i < superJobs.size(); i++){
        cout << "indexSuperToolSet: " << superJobs[i].indexSuperToolSet << "\n";
        cout << "processingTimeSum: " << superJobs[i].processingTimeSum << "\n";
        cout << "prioritySum: " << superJobs[i].prioritySum << "\n";
        cout << "originalJobs: ";

        for (auto it = superJobs[i].originalJobs.begin(); it != superJobs[i].originalJobs.end(); ++it){
            cout << *it << " ";
        }
        
        cout << "\n\n";
    }

    cout << "------------------------------------------------------------------------------------------" << endl;
    cout << "SUPER TOOL SET" << endl;
    cout << "------------------------------------------------------------------------------------------\n" << endl;

    for(int i = 0; i < superToolSet.size(); i++){
        cout << "indexOriginalToolSet: " << superToolSet[i].indexOriginalToolSet << "\n";
        cout << "originalToolSets: ";

        for (auto it = superToolSet[i].originalToolSets.begin(); it != superToolSet[i].originalToolSets.end(); ++it){
            cout << *it << " ";
        }
        
        cout << "\n\n";
    }

    cout << "------------------------------------------------------------------------------------------" << endl;
    cout << "PRIORITY INDEX" << endl;
    cout << "------------------------------------------------------------------------------------------\n" << endl;

    for (auto it = priorityIndex.begin(); it != priorityIndex.end(); ++it) {
        cout << "SuperJob: " << (*it).first << " OriginalJob: " << (*it).second <<"\n";
    }

}

