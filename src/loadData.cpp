#include <algorithm>
#include <fstream>
#include <iostream>
#include <numeric>
#include <sstream>
#include <string>
#include <unordered_set>
#include <vector>

#include "headers/GlobalVars.h"

#ifdef DEBUG
#include <fmt/core.h>
#include <fmt/ranges.h>
#endif

#include "headers/SSP.h"

using namespace std;

// ------------------------------------------------------------------------------------------------------------------------------------------------------
// LOAD FUNCTIONS
// ------------------------------------------------------------------------------------------------------------------------------------------------------

int SSP::laodInstance(string filename) {
    ifstream file(filename);

    if (!file.is_open()) {
        cerr << "Error opening instance file!" << endl;
        exit(1);
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
        numberOfPriorityJobs += tmpJob.priority;

        tmpJob.toolSet = originalToolSets[tmpJob.indexToolSet];
        tmpJob.isGrouped = false;

        originalJobs.push_back(tmpJob);
    }

    // ------------------------------------------------------------------------------------------------------------------------------------------------------

    // iterate over all the toolSets in the originalJobs
    int ferramentaIndex = 0;
    for (auto &thisJob : originalJobs) {
        for (auto &tool : thisJob.toolSet.tools) {
            if (ferramentas.find(tool) == ferramentas.end()) {
                ferramentas[tool] = ferramentaIndex;
                ferramentNormalizadaXFerramentaReal[ferramentaIndex] = tool;
                ferramentaIndex++;
            }
        }
    }
    numberOfTools = ferramentaIndex;

    // iterate over all iriginalJobs toolsets and create the normalized toolsets
    for (auto &thisJob : originalJobs) {
        ToolSet tmpToolSet;
        tmpToolSet.indexToolSet = thisJob.indexToolSet;
        for (auto &tool : thisJob.toolSet.tools) {
            tmpToolSet.tools.push_back(ferramentas[tool]);
        }
        normalizedToolSets[thisJob.indexToolSet] = tmpToolSet;
        thisJob.toolSetNormalized = normalizedToolSets[thisJob.indexToolSet];
    }

    file.close();

    // ------------------------------------------------------------------------------------------------------------------------------------------------------

    numberJobs = originalJobs.size();
    capacityMagazine = 80;
    numberTools = ferramentaIndex + 1;
    numberToolsReal = 0;
    for (const auto &[key, value] : originalToolSets) {
        for (const auto &tool : value.tools) {
            if (tool > numberToolsReal) {
                numberToolsReal = tool;
            }
        }
    }
    numberToolsReal++;

    // ------------------------------------------------------------------------------------------------------------------------------------------------------

    // planingHorizon = 7   * (24*60); // 7 dias em minutos
    // unsupervised   = 0.5 * (24*60); // 0.5 dia em minutos
    planingHorizon = 7;
    unsupervised = 0.5;

    // ------------------------------------------------------------------------------------------------------------------------------------------------------

    // make a for loop numberTools and push a vector of numberJobs with false
    for (int i = 0; i < numberTools; i++) {
        vector<bool> tmpVector(numberJobs, false);
        toolJob.push_back(tmpVector);
    }

    // iterate of the originalJobs and create the toolJob matrix
    for (auto &thisJob : originalJobs) {
        for (auto &tool : thisJob.toolSetNormalized.tools) {
            toolJob[tool][thisJob.indexJob] = true;
        }
    }

    return 0;
}

int SSP::laodToolSet(string filename) {
    ifstream file(filename);
    int tmpIndex;

    if (!file.is_open()) {
        cerr << "Error opening toolset file!" << endl;
        exit(1);
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
            if (!value.empty()) {
                lineData.push_back(stoi(value));
            } else {
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

int SSP::loadInstanceParans(string filename) {
    string datFilename = filename.substr(0, filename.size() - 4) + ".dat";

    std::ifstream file(datFilename);
    if (!file.is_open()) {
        std::cerr << "Error: Could not open file " << datFilename << std::endl;
        exit(1);
    }

    std::string line;
    while (std::getline(file, line)) {
        size_t spacePos = line.find(' ');
        if (spacePos != std::string::npos) {
            std::string key = line.substr(0, spacePos);
            std::string value = line.substr(spacePos + 1);

            if (key == "CAPACITY") {
                capacityMagazine = std::stoi(value);
            } else if (key == "MACHINES") {
                numberMachines = std::stoi(value);
            } else if (key == "DAYS") {
                planingHorizon = std::stoi(value);
            } else if (key == "UNSUPERVISED_MINUTS") {
                unsupervised = std::stoi(value);
            }
        }
    }

    file.close();
    return 0;  
}

// ------------------------------------------------------------------------------------------------------------------------------------------------------
// GRUPING
// ------------------------------------------------------------------------------------------------------------------------------------------------------

void SSP::groupJobs() {
    numberJobsUngrouped = originalJobs.size();
    std::vector<int> indicesToDelete;

    for (size_t i = 0; i < originalJobs.size(); ++i) {
        auto &thisJob = originalJobs[i];
        if (thisJob.indexOperation == 0) {
            for (size_t j = 0; j < originalJobs.size(); ++j) {
                auto &otherJob = originalJobs[j];
                if (otherJob.indexOperation == 1 && otherJob.indexJob == thisJob.indexJob) {
                    thisJob.isGrouped = true;
                    thisJob.processingTimes.push_back(otherJob.processingTime);
                    thisJob.processingTimes.push_back(thisJob.processingTime);
                    thisJob.processingTime = otherJob.processingTime + thisJob.processingTime;

                    indicesToDelete.push_back(j);
                }
            }
        }
    }

    std::sort(indicesToDelete.rbegin(), indicesToDelete.rend());
    for (int index : indicesToDelete) {
        originalJobs.erase(originalJobs.begin() + index);
    }

    numberJobs = originalJobs.size();

}

// ------------------------------------------------------------------------------------------------------------------------------------------------------
// LOAD PRINTS
// ------------------------------------------------------------------------------------------------------------------------------------------------------
void SSP::printDataReport() {
#ifdef DEBUG
    fmt::print("\n------------------------------------------------------------------------------------------------------------------------------------------\n");
    fmt::print("DATA REPORT\n");
    fmt::print("------------------------------------------------------------------------------------------------------------------------------------------\n\n");

    fmt::print("Number of Machines: {}\n", numberMachines);
    fmt::print("Capacity of Magazine: {}\n", capacityMagazine);
    fmt::print("Planing Horizon: {}\n", planingHorizon);
    fmt::print("Unsupervised: {}\n\n", unsupervised);
    fmt::print("Number of Tools: {}\n\n", numberTools);
    fmt::print("Number of Original Jobs: {}\n\n", numberJobs);

    fmt::print("\n------------------------------------------------------------------------------------------------------------------------------------------\n");
    fmt::print("JOBS DATA\n");
    fmt::print("------------------------------------------------------------------------------------------------------------------------------------------\n\n");

    size_t index = 0;
    for (const auto &thisJob : originalJobs) {
        fmt::print("Index: {}\n", index);
        fmt::print("Job: {}\n", thisJob.indexJob);
        fmt::print("Operation: {}\n", thisJob.indexOperation);
        fmt::print("ProcessingTime: {}\n", thisJob.processingTime);
        fmt::print("Priority: {}\n", thisJob.priority);
        fmt::print("ToolSet Tools: {}\n", fmt::join(thisJob.toolSet.tools, " "));
        fmt::print("thisJob.indexToolSet: {}\n", thisJob.indexToolSet);
        fmt::print("thisJob.toolSet.indexToolSet: {}\n", thisJob.toolSet.indexToolSet);
        fmt::print("Normalized ToolSet Tools: {}\n", fmt::join(normalizedToolSets[thisJob.indexToolSet].tools, " "));
        fmt::print("isGrouped: {}\n", thisJob.isGrouped);
        fmt::print("ProcessingTimes: {}\n\n", fmt::join(thisJob.processingTimes, " "));
        ++index;
    }

    fmt::print("------------------------------------------------------------------------------------------------------------------------------------------\n");
    fmt::print("TOOL SET\n");
    fmt::print("------------------------------------------------------------------------------------------------------------------------------------------\n\n");

    // print the tool set map
    for (const auto &[key, value] : originalToolSets) {
        fmt::print("ToolSet: {}\n", key);
        fmt::print("Tools: {}\n\n", fmt::join(value.tools, " "));
    }

    fmt::print("------------------------------------------------------------------------------------------------------------------------------------------\n");
    fmt::print("TOOL NORMALIZED\n");
    fmt::print("------------------------------------------------------------------------------------------------------------------------------------------\n\n");

    // print the tool set map
    for (const auto &[key, value] : normalizedToolSets) {
        fmt::print("ToolSet: {}\n", key);
        fmt::print("Tools: {}\n\n", fmt::join(value.tools, " "));
    }

    fmt::print("------------------------------------------------------------------------------------------------------------------------------------------\n");
    fmt::print("TOOL JOB\n");
    fmt::print("------------------------------------------------------------------------------------------------------------------------------------------\n\n");

    // print the toolJob matrix
    for (int i = 0; i < numberTools; i++) {
        fmt::print("Tool: {}\n", i);
        fmt::print("Jobs: {}\n\n", fmt::join(toolJob[i], " "));
    }

#endif
}

