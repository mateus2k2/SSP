#pragma once

#include <string>
#include <thread>

// ─────────────────────────────────────────────────────────────────────────────
// Which solving method main() should run for this instance.
// ─────────────────────────────────────────────────────────────────────────────
enum class Method {
    GA,           // Genetic Algorithm (default)
    PT,           // Parallel Tempering
    ONB,          // One-Block-Neighborhood local search on a random solution
    MODELO,       // Gurobi ILP model
    PRACTITIONER  // Practitioner heuristic
};

// ─────────────────────────────────────────────────────────────────────────────
// All parsed CLI configuration for a run. Grouped by the subsystem each
// field belongs to; unused groups (e.g. PT params when --METHOD ga) are
// simply ignored by main().
// ─────────────────────────────────────────────────────────────────────────────
struct RunConfig {
    // Required positional arguments
    std::string jobsFile;
    std::string toolsFile;
    std::string outputFile;

    Method method = Method::GA;

    // Parallel Tempering (PT) parameters
    float tempInit                 = 0.01f;
    float tempFim                  = 2.0f;
    int   nReplicas                = 10;
    int   MCL                      = 0;
    int   PTL                      = 2;
    int   passoGatilho             = 0;
    int   tempDist                 = 1;
    int   typeUpdate               = 0;
    int   tempUpdate               = 50;
    int   ptlTempUpdateProportion  = 1;
    int   threadsUsed              = (int)std::thread::hardware_concurrency();
    int   initSolType              = 0;

    // Problem / instance parameters
    int  diferentToolsetMode = 0;
    bool instanceReport      = false;

    // Modelo (Gurobi ILP)
    int timeLimit = 0;

    // Practitioner heuristic
    int sequenceBy = 0;

    // Cost parameters (shared across methods)
    int costSwitch         = 1;
    int costSwitchInstance = 10;
    int costPriority       = 30;
    int profitFinished     = 30;
};

// ─────────────────────────────────────────────────────────────────────────────
// Parses argv into a RunConfig. Prints usage and exit(1)s on:
//   - missing/too few positional arguments
//   - an unrecognized --FLAG
//   - a flag that is missing its value
//   - --HELP / -h
// ─────────────────────────────────────────────────────────────────────────────
class ArgParser {
public:
    static RunConfig parse(int argc, char* argv[]);
    static void printUsage(const char* progName);
};
