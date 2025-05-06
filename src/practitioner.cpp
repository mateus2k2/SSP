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

    // Balancing workloads
    while (true) {
        auto maxIt = max_element(machines.begin(), machines.end(), [](const Machine &a, const Machine &b) {
            return a.totalWorkload < b.totalWorkload;
        });
        auto minIt = min_element(machines.begin(), machines.end(), [](const Machine &a, const Machine &b) {
            return a.totalWorkload < b.totalWorkload;
        });

        int diff = maxIt->totalWorkload - minIt->totalWorkload;
        if (diff <= B1 || minIt->totalWorkload >= B2) break;

        auto it = min_element(maxIt->assignedFamilies.begin(), maxIt->assignedFamilies.end(), [](const Family &a, const Family &b) {
            return a.totalProcTime < b.totalProcTime;
        });

        if (it != maxIt->assignedFamilies.end()) {
            minIt->assignedFamilies.push_back(*it);
            minIt->totalWorkload += it->totalProcTime;
            maxIt->totalWorkload -= it->totalProcTime;
            maxIt->assignedFamilies.erase(it);
        } else break;
    }
}

// Phase 2: Schedule Creation
void createSchedules(const vector<Machine> &machines) {
    for (int i = 0; i < machines.size(); ++i) {
        cout << "Machine " << i+1 << " schedule:\n";
        vector<Operation> sequence;
        for (const auto &fam : machines[i].assignedFamilies) {
            for (const auto &op : fam.operations) {
                if (op.isPriority)
                    sequence.insert(sequence.begin(), op); // insert early
                else
                    sequence.push_back(op);
            }
        }

        for (const auto &op : sequence) {
            cout << "  Job " << op.jobId << " - Op " << op.opId
                 << " (Tool: " << op.toolSet << ", Time: " << op.processingTime << ")\n";
        }
    }
}

int SSP:: practitioner(string filenameoutput){
    vector<Operation> operations = {
        {1, 1, 5, 1, true}, {1, 2, 4, 1, true}, {7, 1, 6, 1, true},
        {2, 1, 9, 2, false}, {4, 1, 10, 2, true}, {4, 2, 12, 2, true},
        {3, 1, 8, 3, false}, {3, 2, 7, 3, false},
        {5, 1, 6, 4, false}, {6, 1, 6, 5, false}
    };

    vector<Machine> machines;
    allocateOperationsToMachines(operations, 2, machines);
    createSchedules(machines);
    return 0;
}