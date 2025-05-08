#include "headers/SSP.h"

using namespace std;

#include <iostream>
#include <vector>
#include <map>
#include <set>
#include <algorithm>
#include <limits>
using namespace std;

struct Operation {
    int jobId;
    int opId;
    int processingTime;
    int toolSet;
    bool isPriority;
    vector<int> tools; // List of tools required for this operation
};

struct Family {
    int toolSet;
    vector<Operation> operations;
    int totalProcTime;
};

struct Machine {
    vector<Family> assignedFamilies;
    int totalWorkload = 0;
};

// Constants (example values)
const int H = 48; // time horizon
const float B1 = 0.1f * H;  // max allowed workload difference
const float B2 = 0.8f * H;  // min workload per machine

// Comparator for sorting by processing time
bool compareByTotalProcTime(const Family &a, const Family &b) {
    return a.totalProcTime < b.totalProcTime;
}

// Phase 1: Grouping and Allocation
void allocateOperationsToMachines(
    vector<Operation> &operations,
    int numMachines,
    vector<Machine> &machines
) {
    map<int, Family> families;
    for (const auto &op : operations) {
        families[op.toolSet].toolSet = op.toolSet;
        families[op.toolSet].operations.push_back(op);
        families[op.toolSet].totalProcTime += op.processingTime;
    }

    vector<Family> sortedFamilies;
    for (const auto &p : families) {
        sortedFamilies.push_back(p.second);
    }
    sort(sortedFamilies.begin(), sortedFamilies.end(), compareByTotalProcTime);

    machines.resize(numMachines);
    for (const auto &fam : sortedFamilies) {
        auto minIt = min_element(machines.begin(), machines.end(), [](const Machine &a, const Machine &b) {
            return a.totalWorkload < b.totalWorkload;
        });
        minIt->assignedFamilies.push_back(fam);
        minIt->totalWorkload += fam.totalProcTime;
    }

    bool balancing = true;
    while (balancing) {
        auto maxIt = max_element(machines.begin(), machines.end(), [](const Machine &a, const Machine &b) {
            return a.totalWorkload < b.totalWorkload;
        });
        auto minIt = min_element(machines.begin(), machines.end(), [](const Machine &a, const Machine &b) {
            return a.totalWorkload < b.totalWorkload;
        });

        int w_max = maxIt->totalWorkload;
        int w_min = minIt->totalWorkload;

        if ((w_max - w_min) <= B1 || w_min >= B2) {
            balancing = false;
        } else {
            // Find the family with the smallest processing time in maxIt
            auto famIt = min_element(maxIt->assignedFamilies.begin(), maxIt->assignedFamilies.end(), [](const Family &a, const Family &b) {
                return a.totalProcTime < b.totalProcTime;
            });

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

// Phase 2: Schedule Creation
void createSchedules(const vector<Machine>& machines) {
    for (size_t m = 0; m < machines.size(); ++m) {
        const auto& machine = machines[m];
        vector<Operation> schedule;

        // Helper lambda to compute tool similarity
        auto toolSimilarity = [](const Operation& a, const Operation& b) {
            set<int> setA(a.tools.begin(), a.tools.end());
            int common = 0;
            for (int tool : b.tools) {
                if (setA.count(tool)) ++common;
            }
            return common;
        };

        // First insert PRIORITY operations
        for (const auto& fam : machine.assignedFamilies) {
            for (const auto& op : fam.operations) {
                if (!op.isPriority) continue;

                int bestScore = -1;
                auto bestIt = schedule.end();

                for (auto it = schedule.begin(); it != schedule.end(); ++it) {
                    int sim = toolSimilarity(op, *it);
                    if (sim > bestScore) {
                        bestScore = sim;
                        bestIt = it + 1;
                    }
                }

                if (bestScore > 0 && bestIt != schedule.end()) {
                    schedule.insert(bestIt, op);
                } else {
                    schedule.push_back(op);
                }
            }
        }

        // Then insert NON-PRIORITY operations
        for (const auto& fam : machine.assignedFamilies) {
            for (const auto& op : fam.operations) {
                if (op.isPriority) continue;

                int bestScore = -1;
                auto bestIt = schedule.end();

                for (auto it = schedule.begin(); it != schedule.end(); ++it) {
                    int sim = toolSimilarity(op, *it);
                    if (sim > bestScore) {
                        bestScore = sim;
                        bestIt = it + 1;
                    }
                }

                if (bestScore > 0 && bestIt != schedule.end()) {
                    schedule.insert(bestIt, op);
                } else {
                    schedule.push_back(op);
                }
            }
        }

        // Output
        cout << "Schedule for Machine " << m + 1 << ":\n";
        for (const auto& op : schedule) {
            cout << "  Job " << op.jobId << ", Op " << op.opId
                 << ", ToolSet " << op.toolSet
                 << ", Priority: " << (op.isPriority ? "Yes" : "No") << "\n";
        }
        cout << endl;
    }
}


int SSP:: practitioner(string filenameoutput){
    vector<Operation> operations = {
        {0,0,3,1,true, {1,1,2,3,4,5}},
        {0,1,5,1,true, {1,1,2,3,4,5}},
        {1,0,7,2,false, {2,12,13,14,15,16,17,18}},
        {2,0,6,3,false, {3,4,5,8,9,10,11,12,13}},
        {2,1,8,3,false, {3,4,5,8,9,10,11,12,13}},
        {3,0,4,2,true, {2,12,13,14,15,16,17,18}},
        {3,1,9,2,true, {2,12,13,14,15,16,17,18}},
        {4,0,6,4,false, {4,5,6,7}},
        {5,0,10,5,false, {5,15,16,17,18,19,20}},
        {6,0,5,1,true, {1,1,2,3,4,5}}
    };

    vector<Machine> machines;
    allocateOperationsToMachines(operations, 2, machines);
    createSchedules(machines);
    return 0;
}