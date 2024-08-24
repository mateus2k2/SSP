#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <algorithm> 
#include <unordered_set>
#include <fmt/core.h>
#include <fmt/ranges.h>

#include "headers/loadData.h"
#include "headers/GlobalVars.h"

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
        job.push_back(stoi(value));

        getline(ss, value, ';');    
        operation.push_back(stoi(value));

        getline(ss, value, ';');
        tmpToolSetIndex = stoi(value);
        // sort(mapToolSets[tmpToolSetIndex].begin(), mapToolSets[tmpToolSetIndex].end()); // ****************************
        JobTools.push_back(mapToolSets[tmpToolSetIndex]);
        JobToolsIndex.push_back(tmpToolSetIndex + 1); // ****************************

        getline(ss, value, ';');
        processingTime.push_back(stoi(value));

        getline(ss, value, '\n');
        priority.push_back(stoi(value));

    }

    file.close();

    // ------------------------------------------------------------------------------------------------------------------------------------------------------

    // Numbero total de jobs
    numberJobs = job.size();    

    // Capacidade do magazine, mapacidade do maior toolSet
    // auto maxIt = max_element(JobTools.begin(), JobTools.end(),
    //     [](const vector<int>& a, const vector<int>& b) {
    //         return a.size() < b.size();
    //     });
    // capacityMagazine = maxIt->size();

    // Numero de ferramentas ****************************
    // set <int> allTools;
    // for(int i = 0; i < JobTools.size(); i++){
    //     for(int j = 0; j < JobTools[i].size(); j++){
    //         allTools.insert(JobTools[i][j]);
    //     }
    // }
    // numberTools = allTools.size();
    capacityMagazine = 80;

    int maxTool = -1;
    for(int i = 0; i < JobTools.size(); i++){
        for(int j = 0; j < JobTools[i].size(); j++){
            if (JobTools[i][j] > maxTool){
                maxTool = JobTools[i][j];
            }
        }
    }
    numberTools = maxTool+1; 
    
    
    // ------------------------------------------------------------------------------------------------------------------------------------------------------

    planingHorizon = 7;   
    unsupervised   = 0.5*TIMESCALE;
    
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
    fmt::print("\n------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------\n");
    fmt::print("DATA REPORT\n");
    fmt::print("------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------\n\n");

    fmt::print("Number of Machines: {}\n", numberMachines);
    fmt::print("Capacity of Magazine: {}\n", capacityMagazine);
    fmt::print("Planing Horizon: {}\n", planingHorizon);
    fmt::print("Unsupervised: {}\n\n", unsupervised);

    fmt::print("Number Of ToolSets Originais: {} | {}\n", originalToolSets.size(), mapToolSets.size());
    fmt::print("Number of Tools: {}\n\n", numberTools);

    fmt::print("Number of Original Jobs: {}\n", originalJobs.size());
    fmt::print("Number of Super Jobs: {}\n\n", numberJobs);

    for (const auto& [key, values] : mapToolSets) {
        fmt::print("Key: {}, Values: {}\n", key, fmt::join(values, " "));
    }

    fmt::print("\nPriority:\n{}\n", fmt::join(priority, " "));
    fmt::print("Operation:\n{}\n", fmt::join(operation, " "));
    fmt::print("Job:\n{}\n", fmt::join(job, " "));
    fmt::print("Processing Time:\n{}\n", fmt::join(processingTime, " "));

    fmt::print("\nJobTools:\n");
    for (const auto& ts : JobTools) {
        fmt::print("{}\n", fmt::join(ts, " "));
    }

    fmt::print("\n------------------------------------------------------------------------------------------\n");
    fmt::print("JOBS DATA\n");
    fmt::print("------------------------------------------------------------------------------------------\n\n");

    for (const auto& thisJob : originalJobs) {
        fmt::print("Job: {}\n", thisJob.indexJob);
        fmt::print("Operation: {}\n", thisJob.indexOperation);
        fmt::print("ToolSet: {}\n", thisJob.indexToolSet);
        fmt::print("ProcessingTime: {}\n", thisJob.processingTime);
        fmt::print("Priority: {}\n", thisJob.priority);
        fmt::print("JobTools: {}\n\n", fmt::join(thisJob.JobTools, " "));
    }

    fmt::print("------------------------------------------------------------------------------------------\n");
    fmt::print("TOOL SET\n");
    fmt::print("------------------------------------------------------------------------------------------\n\n");

    for (const auto& toolSet : originalToolSets) {
        fmt::print("ToolSet: {}\n", toolSet.indexToolSet);
        fmt::print("Tools: {}\n\n", fmt::join(toolSet.tools, " "));
    }
}

