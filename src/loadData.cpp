#include <algorithm>
#include <fstream>
#include <iostream>
#include <numeric>
#include <sstream>
#include <string>
#include <unordered_set>
#include <vector>

#include "headers/GlobalVars.h"

#ifdef FMT
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
        tmpJob.isReentrant = false;
        tmpJob.flag = false;

        originalJobs.push_back(tmpJob);
    }

    // ------------------------------------------------------------------------------------------------------------------------------------------------------
    
    // iterate and mark the jobs with operation 1 as reentrant. find the respective job with operation 0 and mark it as reentrant too.
    for (auto &thisJob : originalJobs) {
        if (thisJob.indexOperation == 1) {
            for (auto &otherJob : originalJobs) {
                if (otherJob.indexOperation == 0 && otherJob.indexJob == thisJob.indexJob) {
                    thisJob.isReentrant = true;
                    otherJob.isReentrant = true;
                }
            }
        }
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
    numberJobsUngrouped = originalJobs.size();
    numberJobs = originalJobs.size();

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
    planingHorizon = 7 * DAY;
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

    if (filename.find("Beezao") != string::npos) {
        return 0;
    }

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
    std::vector<int> indicesToDelete;
    
    for (size_t i = 0; i < originalJobs.size(); ++i) {
        auto &thisJob = originalJobs[i];
        if (thisJob.indexOperation == 0) {
            for (size_t j = 0; j < originalJobs.size(); ++j) {
                auto &otherJob = originalJobs[j];
                if (otherJob.indexOperation == 1 && otherJob.indexJob == thisJob.indexJob) {
                    thisJob.processingTimes.push_back(otherJob.processingTime);
                    thisJob.processingTimes.push_back(thisJob.processingTime);
                    thisJob.toolSets.push_back(otherJob.toolSet);
                    thisJob.toolSets.push_back(thisJob.toolSet);
                    thisJob.processingTime = otherJob.processingTime + thisJob.processingTime;
                    
                    if(diferent_toolset_mode == 0) {
                        thisJob.isGrouped = true;
                        indicesToDelete.push_back(j);
                    }
                }
            }
            groupedJobs.push_back(thisJob);
        }
    }

    std::sort(indicesToDelete.rbegin(), indicesToDelete.rend());
    for (int index : indicesToDelete) {
        originalJobs.erase(originalJobs.begin() + index);
    }

    for (size_t i = 0; i < originalJobs.size(); ++i) {
        int indexJob = originalJobs[i].indexJob;
        int indexOperation = originalJobs[i].indexOperation;
        std::tuple<int, int> jobKey = std::make_tuple(indexJob, indexOperation);
        mapJobsToOriginalIndex[jobKey] = i;
    }

    // numberJobs = originalJobs.size();
    numberJobs = groupedJobs.size();

}

// ------------------------------------------------------------------------------------------------------------------------------------------------------
// LOAD BEEZAO
// ------------------------------------------------------------------------------------------------------------------------------------------------------

double getMakespan(const std::string& instanceNamePath){
    auto pos1 = instanceNamePath.find_last_of('/');
    std::string instanceName = (pos1 != std::string::npos) ? instanceNamePath.substr(pos1 + 1) : instanceNamePath;

    std::ifstream file("./input/BeezaoRaw/alns-original.csv");
    if (!file.is_open()) {
        throw std::runtime_error("Could not open CSV file");
    }

    std::string line;
    double sum = 0.0;
    int count = 0;

    std::getline(file, line);

    while (std::getline(file, line)) {
        std::stringstream ss(line);
        std::string field;

        std::getline(ss, field, ',');

        auto pos = field.find('/');
        std::string cleanInstance =
            (pos != std::string::npos) ? field.substr(pos + 1) : field;

        if (cleanInstance != instanceName)
            continue;

        for (int i = 0; i < 5; ++i)
            std::getline(ss, field, ',');

        std::getline(ss, field, ',');
        sum += std::stod(field);
        count++;
    }

    if (count == 0) {
        throw std::runtime_error("Instance not found in CSV");
    }

    return sum / count;
}

int SSP::loadInstanceBeezao(string filename) {
    ifstream file(filename);
    if (!file.is_open()) {
        cerr << "Error opening Beezao instance file!" << endl;
        exit(1);
    }

    // ---------------------------------------------------------------------
    // Row 1: #machines, #jobs, #tools, capacity of magazine
    int machines, jobs, tools, capacity;
    file >> machines >> jobs >> tools >> capacity;

    numberMachines   = machines;
    numberJobs       = jobs;
    numberJobsUngrouped = jobs;
    numberToolsReal  = tools;
    capacityMagazine = capacity;

    // ---------------------------------------------------------------------
    // Row 2: switching time
    int switchingTime;
    file >> switchingTime;

    // ---------------------------------------------------------------------
    // Row 3: processing times
    vector<int> processingTimes(jobs);
    for (int j = 0; j < jobs; j++) {
        file >> processingTimes[j];
    }

    // ---------------------------------------------------------------------
    // Remaining rows: tool requirement matrix (tools × jobs)
    vector<vector<int>> toolMatrix(tools, vector<int>(jobs, 0));
    for (int t = 0; t < tools; t++) {
        for (int j = 0; j < jobs; j++) {
            file >> toolMatrix[t][j];
        }
    }


    // ---------------------------------------------------------------------
    // Create ToolSets: ONE toolset per job
    originalToolSets.clear();
    for (int j = 0; j < jobs; j++) {
        ToolSet ts;
        ts.indexToolSet = j;
        for (int t = 0; t < tools; t++) {
            if (toolMatrix[t][j] == 1) {
                ts.tools.push_back(t);
            }
        }
        originalToolSets[j] = ts;
    }

    // ---------------------------------------------------------------------
    // Create Jobs
    originalJobs.clear();
    numberOfPriorityJobs = 0;

    for (int j = 0; j < jobs; j++) {
        Job job;
        job.indexJob        = j;
        job.indexOperation = 0;
        job.indexToolSet   = j;
        job.processingTime = processingTimes[j];
        job.priority       = 1;   // default priority
        numberOfPriorityJobs += job.priority;

        job.toolSet        = originalToolSets[j];
        job.toolSetNormalized = originalToolSets[j];
        job.isGrouped      = false;
        job.isReentrant    = false;
        job.flag           = false;

        originalJobs.push_back(job);
    }

    // ---------------------------------------------------------------------
    // Build tool-job boolean matrix
    numberTools = tools;
    toolJob.clear();
    toolJob.resize(numberTools, vector<bool>(numberJobs, false));

    for (int j = 0; j < jobs; j++) {
        for (int t : originalToolSets[j].tools) {
            toolJob[t][j] = true;
        }
    }

    // ---------------------------------------------------------------------
    planingHorizon = getMakespan(filename);
    unsupervised = planingHorizon;

    file.close();

    return 0;
}


// ------------------------------------------------------------------------------------------------------------------------------------------------------
// LOAD PRINTS
// ------------------------------------------------------------------------------------------------------------------------------------------------------

void SSP::printDataReport() {
#ifdef FMT
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

