#include "headers/SSP.h"

using namespace std;

#include <algorithm>
#include <iostream>
#include <limits>
#include <map>
#include <set>
#include <vector>
using namespace std;

struct Family {
    int toolSet;
    int totalProcTime;
    vector<Job> operations;
};

struct Machine {
    vector<Family> assignedFamilies;
    int totalWorkload = 0;
};

struct AlocatedMachine {
    vector<Job> operations;
};

int H;
int U;
float B1;
float B2;
vector<Machine> machines;
vector<AlocatedMachine> alocatedMachines;

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

void SSP::allocateOperationsToMachines(int numMachines) {
    map<int, Family> families;
    for (const auto& op : originalJobs) {
        families[op.toolSetNormalized.indexToolSet].toolSet = op.toolSetNormalized.indexToolSet;
        families[op.toolSetNormalized.indexToolSet].operations.push_back(op);
        families[op.toolSetNormalized.indexToolSet].totalProcTime += op.processingTime;
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
            // Find the family with the smallest processing time in maxIt
            auto famIt = min_element(maxIt->assignedFamilies.begin(), maxIt->assignedFamilies.end(), [](const Family& a, const Family& b) { return a.totalProcTime < b.totalProcTime; });

            if (famIt != maxIt->assignedFamilies.end()) {
                // Move family to the start of the array
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

void createSchedules(int condition) {
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
    
    // prioritarios primeiro
    for (size_t i = 0; i < machines.size(); i++) {
        alocatedMachines.push_back(AlocatedMachine());
        for (size_t j = 0; j < machines[i].assignedFamilies.size(); j++) {
            Family& family = machines[i].assignedFamilies[j];
            for (const auto& job : family.operations) {
                if (job.priority) alocatedMachines[i].operations.push_back(job);
            }
        }
    }

    // depois mais similares em sequencia
    for (size_t i = 0; i < machines.size(); i++) {
        for (size_t j = 0; j < machines[i].assignedFamilies.size(); j++) {
            Family& family = machines[i].assignedFamilies[j];
            for (const auto& jobCurrant : family.operations) {
                if (!jobCurrant.priority) {
                    int bestIndex = -1;
                    int bestIntersection = -numeric_limits<int>::max();
                    int bestDiference = numeric_limits<int>::max();
                    for (const auto& jobTesting : alocatedMachines[i].operations) {
                        vector<int> previusTools = jobTesting.toolSetNormalized.tools;
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
                    cout << "Best index: " << bestIndex << " i: "  << i << endl;
                    if (bestIndex != -1) {
                        auto it = find_if(alocatedMachines[i].operations.begin(), alocatedMachines[i].operations.end(), [&](const Job& job) { return job.indexJob == bestIndex; });
                        int index = distance(alocatedMachines[i].operations.begin(), it);
                        alocatedMachines[i].operations.insert(alocatedMachines[i].operations.begin() + index + 1, jobCurrant);
                    }
                    else{
                        alocatedMachines[i].operations.push_back(jobCurrant);
                    }
                }
            }
        }
    }
}

void SSP::reportDataPractitioner(fstream& solutionReportFile, string filenameJobs, string filenameTools) {
    solutionReportFile << filenameJobs << ";" << filenameTools << endl;
    solutionReportFile << H << ";" << U << ";" << DAY << endl;

    int fineshedJobsCountTotal = 0;
    int switchsTotal = 0;
    int switchsInstancesTotal = 0;
    int unfineshedPriorityCountTotal = 0;

    for (size_t i = 0; i < alocatedMachines.size(); i++) {
        vector<int> jobsInMachine;
        for (const auto& machineJob : alocatedMachines[i].operations) {
            auto it = find_if(originalJobs.begin(), originalJobs.end(), [&](const Job& job) { return job.indexJob == machineJob.indexJob && job.indexOperation == machineJob.indexOperation; });
            int index = distance(originalJobs.begin(), it);
            jobsInMachine.push_back(index);
        }
        auto [fineshedJobsCount, switchs, switchsInstances, unfineshedPriorityCount] = evaluateReportKTNSMachine(jobsInMachine, solutionReportFile, i);
        fineshedJobsCountTotal += fineshedJobsCount;
        switchsTotal += switchs;
        switchsInstancesTotal += switchsInstances;
        unfineshedPriorityCountTotal += unfineshedPriorityCount;
    }

    int cost = (PROFITYFINISHED * fineshedJobsCountTotal) - (COSTSWITCH * switchsTotal) - (COSTSWITCHINSTANCE * switchsInstancesTotal) - (COSTPRIORITY * unfineshedPriorityCountTotal);

    solutionReportFile << "END" << endl;
    solutionReportFile << "fineshedJobsCount: " << fineshedJobsCountTotal << endl;
    solutionReportFile << "switchs: " << switchsTotal << endl;
    solutionReportFile << "switchsInstances: " << switchsInstancesTotal << endl;
    solutionReportFile << "unfineshedPriorityCount: " << unfineshedPriorityCountTotal << endl;
    solutionReportFile << "cost: " << cost << endl;
}

int SSP::practitioner(fstream& solutionReportFile, int condition) {
    H = planingHorizon;
    U = unsupervised;
    B1 = 0.1f * H;
    B2 = 0.8f * H;

    allocateOperationsToMachines(2);
    createSchedules(condition);
    reportDataPractitioner(solutionReportFile, inputJobsFile, inputToolsetsFile);
    return 0;
}