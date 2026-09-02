#include <algorithm>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include "headers/Args.h"
#include "headers/GlobalVars.h"
#include "headers/SSP.h"

#ifdef FMT
#include <fmt/core.h>
#include <fmt/ranges.h>
#endif

#include "ExecTime.h"
#include "PT.h"
#include "headers/GA.h"

using namespace std;

// ------------------------------------------------------------------------------------------------------------------------------------------------------
// Method runners — one per --METHOD value. Each opens/writes solutionReportFile itself so the
// report format can differ per method (modelo writes its own file directly and ignores it).
// ------------------------------------------------------------------------------------------------------------------------------------------------------

static void runModelo(SSP* prob, const RunConfig& cfg) {
    prob->modelo(cfg.outputFile, cfg.timeLimit);
}

static void runPractitioner(SSP* prob, const RunConfig& cfg, fstream& report) {
    prob->practitioner(report, cfg.sequenceBy);
}

static void runPT(SSP* prob, const RunConfig& cfg, fstream& report) {
    prob->setParans(cfg.typeUpdate, cfg.initSolType);

    PT<solSSP> algo(cfg.tempInit, cfg.tempFim, cfg.nReplicas, cfg.MCL, cfg.PTL,
                     cfg.passoGatilho, cfg.tempDist, cfg.typeUpdate, cfg.tempUpdate);

    ExecTime et;
    solSSP best = algo.start(cfg.threadsUsed, prob);
    double cost = prob->evaluateReport(best, report);

    vector<solSSP> initAll = algo.getInitAll();
    sort(initAll.begin(), initAll.end(), [](const solSSP& a, const solSSP& b) { return a.evalSol < b.evalSol; });
    double meanInitial = 0.0;
    for (const auto& s : initAll) meanInitial += s.evalSol;
    if (!initAll.empty()) meanInitial /= initAll.size();

    report << "Final Solution: " << cost << endl;
    report << "Time: " << et.getTimeMs() << endl;
    report << "PTL: " << best.ptl << endl;   // PTL where the best solution was generated
    report << "MCMC: " << best.mcmc << endl; // Markov-chain index where the best solution was generated
    if (!initAll.empty()) report << "Best Initial: " << -initAll[0].evalSol << endl;
    report << "Mean Initial: " << -meanInitial << endl;

    cout << cost << endl;
}

static void runONB(SSP* prob, const RunConfig& cfg, fstream& report) {
    prob->setParans(cfg.typeUpdate, cfg.initSolType);

    solSSP sol = prob->randPriority();
    double initialCost = prob->evaluate(sol);
    prob->oneBlockGrouping(sol);
    double finalCost = prob->evaluate(sol);

    report << "Initial Solution: " << initialCost << endl;
    report << "Final Solution: " << finalCost << endl;

    cout << finalCost << endl;
}

static void runGA(SSP* prob, const RunConfig& cfg, fstream& report) {
    prob->setParans(cfg.typeUpdate, cfg.initSolType);

    GAParams gaParams;
    gaParams.numMachines = prob->getNumberMachines();
    gaParams.magazineCap = prob->getCapacityMagazine();
    gaParams.horizon     = prob->getPlaningHorizon();     // already in minutes
    gaParams.unsupHours  = prob->getUnsupervised() / 60;  // convert min -> hours
    gaParams.revenue     = cfg.profitFinished;
    gaParams.penaltyCost = cfg.costPriority;
    gaParams.fixedSwitch = cfg.costSwitchInstance;
    gaParams.varSwitch   = cfg.costSwitch;
    gaParams.maxTimeSec  = 3600.0;

    // Wrap SSP::evaluate as a profit function (evaluate() returns -profit)
    GeneticAlgorithm::EvalFn evalFn = [&](const vector<int>& perm) -> double {
        solSSP s;
        s.sol = perm;
        return -prob->evaluate(s);
    };

    GeneticAlgorithm ga(prob->getGroupedJobs(), gaParams, evalFn, /*seed=*/42);

    ExecTime et;
    Chromosome best = ga.run();

    double cost = best.fitness;
    solSSP bestSol;
    bestSol.sol = best.perm;
    prob->evaluateReport(bestSol, report);

    report << "Final Solution: " << cost << endl;
    report << "Time: " << et.getTimeMs() << endl;

    cout << cost << endl;
}

// ------------------------------------------------------------------------------------------------------------------------------------------------------
// MAIN
// ------------------------------------------------------------------------------------------------------------------------------------------------------

int main(int argc, char* argv[]) {
    RunConfig cfg = ArgParser::parse(argc, argv);

    SSP* prob = new SSP(cfg.jobsFile, cfg.toolsFile, cfg.diferentToolsetMode,
                         cfg.costSwitch, cfg.costSwitchInstance, cfg.costPriority, cfg.profitFinished);

    // ------------------------------------------------------------------------------
    // DATA LOADING AND PRE-PROCESSING
    // ------------------------------------------------------------------------------

    fstream solutionReportFile;
    solutionReportFile.open(cfg.outputFile, ios::out);
    if (!solutionReportFile.is_open() && cfg.method != Method::MODELO) {
        cerr << "Error: Could not open solution report file: " << cfg.outputFile << endl;
        exit(1);
    }

    if (cfg.instanceReport) prob->printDataReport();
    prob->groupJobs();

    // ------------------------------------------------------------------------------
    // DISPATCH
    // ------------------------------------------------------------------------------

    switch (cfg.method) {
        case Method::MODELO:       runModelo(prob, cfg); break;
        case Method::PRACTITIONER: runPractitioner(prob, cfg, solutionReportFile); break;
        case Method::PT:           runPT(prob, cfg, solutionReportFile); break;
        case Method::ONB:          runONB(prob, cfg, solutionReportFile); break;
        case Method::GA:           runGA(prob, cfg, solutionReportFile); break;
    }

    solutionReportFile.close();
    return 0;
}
