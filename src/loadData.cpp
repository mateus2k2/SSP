#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <algorithm> 
#include <numeric>
#include <unordered_set>

#include "headers/GlobalVars.h"

#ifndef IGNORE_FMT
#include <fmt/core.h>
#include <fmt/ranges.h>
#endif

#include "headers/SSP.h"

using namespace std;

// ------------------------------------------------------------------------------------------------------------------------------------------------------
// LOAD FUNCTIONS
// ------------------------------------------------------------------------------------------------------------------------------------------------------

int SSP::laodInstance(string filename){
    ifstream file(filename);

    if (!file.is_open()) {
        cerr << "Error opening file!" << endl;
        return 1;
    }

    string line;
	getline(file, line);
    while (getline(file, line)) {
        Job tmpJob;
        stringstream ss(line);
        string value;

        getline(ss, value, ';');
        tmpJob.indexJob = stoi(value);

        getline(ss, value, ';');    
        tmpJob.indexOperation = stoi(value);

        getline(ss, value, ';');
        tmpJob.indexToolSet = stoi(value);

        getline(ss, value, ';');
        tmpJob.processingTime = stoi(value);

        getline(ss, value, '\n');
        tmpJob.priority = stoi(value);

        tmpJob.toolSet = originalToolSets[tmpJob.indexToolSet];

        originalJobs.push_back(tmpJob);
    }

    // ------------------------------------------------------------------------------------------------------------------------------------------------------

    map<int, int> ferramentas;

    //iterate over all the toolSets in the originalJobs 
    int ferramentaIndex = 0;
    for (auto& thisJob : originalJobs) {
        for(auto& tool : thisJob.toolSet.tools){
            if (ferramentas.find(tool) == ferramentas.end()) {
                ferramentas[tool] = ferramentaIndex;
                ferramentaIndex++;
            }
        }
    }

    //iterate over all iriginalJobs toolsets and create the normalized toolsets
    for (auto& thisJob : originalJobs) {
        ToolSet tmpToolSet;
        tmpToolSet.indexToolSet = thisJob.indexToolSet;
        for(auto& tool : thisJob.toolSet.tools){
            tmpToolSet.tools.push_back(ferramentas[tool]);
        }
        normalizedToolSets[thisJob.indexToolSet] = tmpToolSet;
        thisJob.toolSetNormalized = normalizedToolSets[thisJob.indexToolSet];
    }

    file.close();

    // ------------------------------------------------------------------------------------------------------------------------------------------------------

    numberJobs = originalJobs.size();    
    capacityMagazine = 80;
    numberTools = ferramentaIndex;
    numberToolsReal = 0;
    for (const auto& [key, value] : originalToolSets) {
        for (const auto& tool : value.tools) {
            if (tool > numberToolsReal) {
                numberToolsReal = tool;
            }
        }
    }

    // ------------------------------------------------------------------------------------------------------------------------------------------------------

    // planingHorizon = 7   * (24*60); // 7 dias em minutos
    // unsupervised   = 0.5 * (24*60); // 0.5 dia em minutos
    planingHorizon = 7;
    unsupervised   = 0.5;
    
    // ------------------------------------------------------------------------------------------------------------------------------------------------------
    
    // make a for loop numberTools and push a vector of numberJobs with false
    for (int i = 0; i < numberTools; i++) {
        vector<bool> tmpVector(numberJobs, false);
        toolJob.push_back(tmpVector);
    }

    //iterate of the originalJobs and create the toolJob matrix
    for (auto& thisJob : originalJobs) {
        for(auto& tool : thisJob.toolSetNormalized.tools){
            toolJob[tool][thisJob.indexJob] = true;
        }
    }

    return 0;
}

int SSP::laodToolSet(string filename) {
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
        ToolSet tmpToolSet;

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

        tmpToolSet.indexToolSet = tmpIndex;
        tmpToolSet.tools = lineData;
        originalToolSets[tmpIndex] = tmpToolSet;
    }

    file.close();

    return 0;
}

// ------------------------------------------------------------------------------------------------------------------------------------------------------
// LOAD PRINTS
// ------------------------------------------------------------------------------------------------------------------------------------------------------

void SSP::printDataReport() {
    #ifndef IGNORE_FMT
    fmt::print("\n------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------\n");
    fmt::print("DATA REPORT\n");
    fmt::print("------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------\n\n");

    fmt::print("Number of Machines: {}\n", numberMachines);
    fmt::print("Capacity of Magazine: {}\n", capacityMagazine);
    fmt::print("Planing Horizon: {}\n", planingHorizon);
    fmt::print("Unsupervised: {}\n\n", unsupervised);
    fmt::print("Number of Tools: {}\n\n", numberTools);
    fmt::print("Number of Original Jobs: {}\n\n", numberJobs);


    fmt::print("\n------------------------------------------------------------------------------------------\n");
    fmt::print("JOBS DATA\n");
    fmt::print("------------------------------------------------------------------------------------------\n\n");

    for (const auto& thisJob : originalJobs) {
        fmt::print("Job: {}\n", thisJob.indexJob);
        fmt::print("Operation: {}\n", thisJob.indexOperation);
        fmt::print("ProcessingTime: {}\n", thisJob.processingTime);
        fmt::print("Priority: {}\n", thisJob.priority);
        fmt::print("toolSet Tools: {}\n", fmt::join(thisJob.toolSet.tools, " "));
        fmt::print("thisJob.indexToolSet: {}\n", thisJob.indexToolSet);
        fmt::print("thisJob.toolSet.indexToolSet: {}\n", thisJob.toolSet.indexToolSet);
        fmt::print("Normalized toolSet Tools: {}\n\n", fmt::join(normalizedToolSets[thisJob.indexToolSet].tools, " "));
    }

    fmt::print("------------------------------------------------------------------------------------------\n");
    fmt::print("TOOL SET\n");
    fmt::print("------------------------------------------------------------------------------------------\n\n");

    //print the tool set map
    for (const auto& [key, value] : originalToolSets) {
        fmt::print("ToolSet: {}\n", key);
        fmt::print("Tools: {}\n\n", fmt::join(value.tools, " "));
    }

    fmt::print("------------------------------------------------------------------------------------------\n");
    fmt::print("TOOL NORMALIZED\n");
    fmt::print("------------------------------------------------------------------------------------------\n\n");

    //print the tool set map
    for (const auto& [key, value] : normalizedToolSets) {
        fmt::print("ToolSet: {}\n", key);
        fmt::print("Tools: {}\n\n", fmt::join(value.tools, " "));
    }

    fmt::print("------------------------------------------------------------------------------------------\n");
    fmt::print("TOOL JOB\n");
    fmt::print("------------------------------------------------------------------------------------------\n\n");

    //print the toolJob matrix
    for (int i = 0; i < numberTools; i++) {
        fmt::print("Tool: {}\n", i);
        fmt::print("Jobs: {}\n\n", fmt::join(toolJob[i], " "));
    }

    #endif

}
