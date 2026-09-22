#include <algorithm>
#include <iostream>
#include <limits>
#include <map>
#include <set>
#include <vector>

#include "headers/SSP.h"
// #include "../../PTAPI/include/ExecTime.h"

using namespace std;

int H;
int U;
float B1;
float B2;
vector<Machine> machines;

int intersectionSize(const std::vector<int>& a, const std::vector<int>& b) {
    std::unordered_map<int, int> countA;
    for (int num : a) {
        countA[num]++;
    }

    int intersectionCount = 0;
    for (int num : b) {
        if (countA[num] > 0) {
            intersectionCount++;
            countA[num]--;
        }
    }

    return intersectionCount;
}

int differenceSize(const std::vector<int>& a, const std::vector<int>& b) {
    std::unordered_map<int, int> countB;
    for (int num : b) {
        countB[num]++;
    }

    int diffCount = 0;
    for (int num : a) {
        if (countB[num] > 0) {
            countB[num]--;
        } else {
            diffCount++;
        }
    }

    return diffCount;
}

bool compareByTotalProcTime(const Family& a, const Family& b) { return a.totalProcTime < b.totalProcTime; }

// Op 1 of a reentrant job that was not grouped with its op 0 (different-toolset
// mode). The strong chain requires it on op 0's machine, right after op 0.
static bool isSecondOperation(const Job& job) { return job.isReentrant && !job.isGrouped && job.indexOperation == 1; }

void SSP::allocateOperationsToMachines(int numMachines) {
    // op 1 joins the family of its op 0, so both land on the same machine
    map<int, int> firstOperationToolSet;  // job -> tool set of its op 0
    for (const auto& op : originalJobs) {
        if (op.indexOperation == 0) firstOperationToolSet[op.indexJob] = op.toolSetNormalized.indexToolSet;
    }

    map<int, Family> families;
    for (const auto& op : originalJobs) {
        int key = isSecondOperation(op) ? firstOperationToolSet[op.indexJob] : op.toolSetNormalized.indexToolSet;
        families[key].toolSet = key;
        families[key].operations.push_back(op);
        families[key].totalProcTime += op.processingTime;
    }

    vector<Family> sortedFamilies;
    for (const auto& p : families) {
        sortedFamilies.push_back(p.second);
    }
    sort(sortedFamilies.begin(), sortedFamilies.end(), compareByTotalProcTime);

    machines.resize(numMachines);
    for (const auto& fam : sortedFamilies) {
        auto minIt = min_element(machines.begin(), machines.end(), [](const Machine& a, const Machine& b) { return a.totalWorkload < b.totalWorkload; });
        minIt->assignedFamilies.push_back(fam);
        minIt->totalWorkload += fam.totalProcTime;
    }

    bool balancing = true;
    while (balancing) {
        auto maxIt = max_element(machines.begin(), machines.end(), [](const Machine& a, const Machine& b) { return a.totalWorkload < b.totalWorkload; });
        auto minIt = min_element(machines.begin(), machines.end(), [](const Machine& a, const Machine& b) { return a.totalWorkload < b.totalWorkload; });

        int w_max = maxIt->totalWorkload;
        int w_min = minIt->totalWorkload;

        if ((w_max - w_min) <= B1 || w_min >= B2) {
            balancing = false;
        } else {
            auto famIt = min_element(maxIt->assignedFamilies.begin(), maxIt->assignedFamilies.end(), [](const Family& a, const Family& b) { return a.totalProcTime < b.totalProcTime; });

            // Moving a family at least as long as the gap only swaps which machine is
            // the heaviest, and the next iteration moves it back: an endless loop
            // whenever a machine stays empty (e.g. 6 machines and fewer than 6 families).
            if (famIt != maxIt->assignedFamilies.end() && famIt->totalProcTime < w_max - w_min) {
                minIt->assignedFamilies.insert(minIt->assignedFamilies.begin(), *famIt);
                minIt->totalWorkload += famIt->totalProcTime;

                maxIt->totalWorkload -= famIt->totalProcTime;
                maxIt->assignedFamilies.erase(famIt);
            } else {
                balancing = false;
            }
        }
    }
}

void SSP::createSchedules(int condition) {
    // for (size_t i = 0; i < machines.size(); i++) {
    //     alocatedMachines.push_back(AlocatedMachine());
    //     for (size_t j = 0; j < machines[i].assignedFamilies.size(); j++) {
    //         Family& family = machines[i].assignedFamilies[j];
    //         for (const auto& job : family.operations) {
    //             alocatedMachines[i].operations.push_back(job);
    //         }
    //     }
    // }
    // return;
    
    // Reentrant pairs are sequenced as one unit: only op 0 goes through the two
    // passes below, and op 1 is put right after it at the end. The next operation
    // then follows op 1, so op 1's tools are what similarity is measured against.
    map<int, Job> secondOperation;  // job -> its op 1
    for (const auto& op : originalJobs) {
        if (isSecondOperation(op)) secondOperation[op.indexJob] = op;
    }
    auto lastToolsOfUnit = [&](const Job& job) -> const vector<int>& {
        auto it = secondOperation.find(job.indexJob);
        return (it != secondOperation.end() && job.indexOperation == 0) ? it->second.toolSetNormalized.tools : job.toolSetNormalized.tools;
    };

    // prioritarios primeiro
    for (size_t i = 0; i < machines.size(); i++) {
        for (size_t j = 0; j < machines[i].assignedFamilies.size(); j++) {
            Family& family = machines[i].assignedFamilies[j];
            for (const auto& job : family.operations) {
                if (job.priority && !isSecondOperation(job)) machines[i].operations.push_back(job);
            }
        }
    }

    // depois mais similares em sequencia
    for (size_t i = 0; i < machines.size(); i++) {
        for (size_t j = 0; j < machines[i].assignedFamilies.size(); j++) {
            Family& family = machines[i].assignedFamilies[j];
            for (const auto& jobCurrant : family.operations) {
                if (!jobCurrant.priority && !isSecondOperation(jobCurrant)) {
                    int bestIndex = -1;
                    int bestIntersection = -numeric_limits<int>::max();
                    int bestDiference = numeric_limits<int>::max();
                    for (const auto& jobTesting : machines[i].operations) {
                        vector<int> previusTools = lastToolsOfUnit(jobTesting);
                        vector<int> currentTools = jobCurrant.toolSetNormalized.tools;

                        // interceçao entre os toolsets
                        if (condition == 0) {
                            int intersection = intersectionSize(previusTools, currentTools);
                            if (intersection > bestIntersection) {
                                bestIntersection = intersection;
                                bestIndex = jobTesting.indexJob;
                            }
                        }

                        // Menor diferença
                        if (condition == 1) {
                            int diference = differenceSize(previusTools, currentTools);
                            if (diference < bestDiference) {
                                bestDiference = diference;
                                bestIndex = jobTesting.indexJob;
                            }
                        }
                    }
                    if (bestIndex != -1) {
                        auto it = find_if(machines[i].operations.begin(), machines[i].operations.end(), [&](const Job& job) { return job.indexJob == bestIndex; });
                        int index = distance(machines[i].operations.begin(), it);
                        machines[i].operations.insert(machines[i].operations.begin() + index + 1, jobCurrant);
                    }
                    else{
                        machines[i].operations.push_back(jobCurrant);
                    }
                }
            }
        }
    }

    // op 1 right after its op 0
    for (auto& machine : machines) {
        vector<Job> sequence;
        for (const auto& job : machine.operations) {
            sequence.push_back(job);
            auto it = secondOperation.find(job.indexJob);
            if (it != secondOperation.end()) sequence.push_back(it->second);
        }
        machine.operations = sequence;
    }
}

void SSP::reportDataPractitioner(fstream& solutionReportFile, string filenameJobs, string filenameTools) {
    solutionReportFile << filenameJobs << ";" << filenameTools << endl;
    solutionReportFile << H / DAY << ";" << U << ";" << DAY << ";" << H << endl;

    int fineshedJobsCountTotal = 0;
    int switchsTotal = 0;
    int switchsInstancesTotal = 0;
    int unfineshedPriorityCountTotal = numberOfPriorityJobs;
    int totalUnfineshed = numberJobsUngrouped;

    for (size_t i = 0; i < machines.size(); i++) {
        vector<int> jobsInMachine;
        for (const auto& machineJob : machines[i].operations) {
            auto it = find_if(originalJobs.begin(), originalJobs.end(), [&](const Job& job) { return job.indexJob == machineJob.indexJob && job.indexOperation == machineJob.indexOperation; });
            int index = distance(originalJobs.begin(), it);
            jobsInMachine.push_back(index);
        }
        // KTNSReport returns the FINISHED priority operations (same as evaluateReport)
        auto [fineshedJobsCount, switchs, switchsInstances, fineshedPriorityCount, _, lastJob] = KTNSReport(jobsInMachine, 0, solutionReportFile, i);
        fineshedJobsCountTotal += fineshedJobsCount;
        switchsTotal += switchs;
        switchsInstancesTotal += switchsInstances;
        unfineshedPriorityCountTotal -= fineshedPriorityCount;
        totalUnfineshed -= fineshedJobsCount;
    }

    int cost = (PROFITYFINISHED * fineshedJobsCountTotal) - (COSTSWITCH * switchsTotal) - (COSTSWITCHINSTANCE * switchsInstancesTotal) - (COSTPRIORITY * unfineshedPriorityCountTotal);

    solutionReportFile << "END" << endl;
    solutionReportFile << "fineshedJobsCount: " << fineshedJobsCountTotal << endl;
    solutionReportFile << "switchs: " << switchsTotal << endl;
    solutionReportFile << "switchsInstances: " << switchsInstancesTotal << endl;
    solutionReportFile << "unfineshedPriorityCount: " << unfineshedPriorityCountTotal << endl;
    solutionReportFile << "finalSolution: " << cost << endl;
    solutionReportFile << "totalUnfineshed: " << totalUnfineshed << endl;
}

vector<Machine> SSP::practitioner(fstream& solutionReportFile, int condition) {
    // ExecTime et;
    auto start = std::chrono::high_resolution_clock::now();
    H = horizonMinutes;      // minutes, like every workload below
    U = unsupervisedStart;
    B1 = 0.1f * H;           // machines are balanced until the gap is under B1
    B2 = 0.8f * H;           // or the lightest machine is loaded past B2

    allocateOperationsToMachines(numberMachines);
    createSchedules(condition);
    reportDataPractitioner(solutionReportFile, inputJobsFile, inputToolsetsFile);
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    
    solutionReportFile << "Time: " << duration.count() << endl;
    
    return machines;
}