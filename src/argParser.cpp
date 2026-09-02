#include "headers/Args.h"

#include <cstdlib>
#include <iostream>
#include <vector>

using namespace std;

void ArgParser::printUsage(const char* progName) {
    cerr <<
        "Usage: " << progName << " <jobsFile> <toolsFile> <outputFile> [options]\n"
        "\n"
        "Required positional arguments:\n"
        "  jobsFile     Path to the jobs/instance file\n"
        "  toolsFile    Path to the tool-set file\n"
        "  outputFile   Path to write the solution report to\n"
        "\n"
        "Method selection:\n"
        "  --METHOD <ga|pt|onb|modelo|practitioner>   Solving method (default: ga)\n"
        "\n"
        "Parallel Tempering (--METHOD pt) options:\n"
        "  --TEMP_INIT <float>                   Initial temperature (default 0.01)\n"
        "  --TEMP_FIM <float>                    Final temperature (default 2.0)\n"
        "  --N_REPLICAS <int>                    Number of temperature replicas (default 10)\n"
        "  --MCL <int>                           Markov chain length (default 0)\n"
        "  --PTL <int>                           Number of swap proposals (default 2)\n"
        "  --PASSO_GATILHO <int>                 Trigger step (default 0)\n"
        "  --TEMP_DIST <int>                     Temperature distribution: 1=linear 2=ilinear 3=exp 4=pg (default 1)\n"
        "  --TYPE_UPDATE <int>                   Temperature update type (default 0)\n"
        "  --TEMP_UPDATE <int>                   Swaps between temp updates (default 50; overridden by --PTL_TEMP_UPDATE_PROPORTION)\n"
        "  --PTL_TEMP_UPDATE_PROPORTION <int>    tempUpdate = PTL / this (default 1)\n"
        "  --THREAD_USED <int>                   Threads to use (default: hardware concurrency)\n"
        "  --INIT_SOL_TYPE <int>                 Initial-solution construction type (default 0)\n"
        "\n"
        "Instance / problem options:\n"
        "  --DIFERENT_TOOLSETS_MODE <0|1>        Instance uses per-operation tool sets (default 0)\n"
        "  --INSTANCE_REPORT <0|1>               Print the instance report before solving (default 0)\n"
        "\n"
        "--METHOD modelo options:\n"
        "  --TIME_LIMIT <int>                    Gurobi time limit in seconds (default 0 = unlimited)\n"
        "\n"
        "--METHOD practitioner options:\n"
        "  --SEQUENCE_BY <int>                   Sequencing rule (default 0)\n"
        "\n"
        "Cost parameters (all methods):\n"
        "  --COSTSWITCH <int>                    Cost per tool switch (default 1)\n"
        "  --COSTSWITCHINSTANCE <int>            Fixed cost per switch instance (default 10)\n"
        "  --COSTPRIORITY <int>                  Penalty for unfinished priority jobs (default 30)\n"
        "  --PROFITYFINISHED <int>               Revenue per finished job (default 30)\n"
        "\n"
        "  --HELP, -h                            Show this message\n";
}

namespace {

Method parseMethod(const string& progName, const string& value) {
    if (value == "ga")           return Method::GA;
    if (value == "pt")           return Method::PT;
    if (value == "onb")          return Method::ONB;
    if (value == "modelo")       return Method::MODELO;
    if (value == "practitioner") return Method::PRACTITIONER;

    cerr << "Error: unrecognized --METHOD value '" << value
         << "' (expected ga|pt|onb|modelo|practitioner)\n\n";
    ArgParser::printUsage(progName.c_str());
    exit(1);
}

} // namespace

RunConfig ArgParser::parse(int argc, char* argv[]) {
    vector<string> args(argv + 1, argv + argc);

    for (const auto& a : args) {
        if (a == "--HELP" || a == "-h") {
            printUsage(argv[0]);
            exit(0);
        }
    }

    if (args.size() < 3) {
        cerr << "Error: missing required arguments (jobsFile, toolsFile, outputFile).\n\n";
        printUsage(argv[0]);
        exit(1);
    }

    RunConfig cfg;
    cfg.jobsFile   = args[0];
    cfg.toolsFile  = args[1];
    cfg.outputFile = args[2];

    for (size_t i = 3; i < args.size(); i += 2) {
        const string& flag = args[i];
        if (i + 1 >= args.size()) {
            cerr << "Error: flag '" << flag << "' is missing its value.\n\n";
            printUsage(argv[0]);
            exit(1);
        }
        const string& value = args[i + 1];

        if      (flag == "--METHOD")                        cfg.method = parseMethod(argv[0], value);
        else if (flag == "--MODELO"       && value == "1")  cfg.method = Method::MODELO;       // legacy alias
        else if (flag == "--PRACTITIONER" && value == "1")  cfg.method = Method::PRACTITIONER;  // legacy alias

        else if (flag == "--TEMP_FIM")                       cfg.tempFim = stof(value);
        else if (flag == "--TEMP_INIT")                      cfg.tempInit = stof(value);
        else if (flag == "--N_REPLICAS")                     cfg.nReplicas = stoi(value);
        else if (flag == "--MCL")                            cfg.MCL = stoi(value);
        else if (flag == "--PTL")                            cfg.PTL = stoi(value);
        else if (flag == "--TEMP_DIST")                      cfg.tempDist = stoi(value);
        else if (flag == "--TYPE_UPDATE")                    cfg.typeUpdate = stoi(value);
        else if (flag == "--TEMP_UPDATE")                    cfg.tempUpdate = stoi(value);
        else if (flag == "--THREAD_USED")                    cfg.threadsUsed = stoi(value);
        else if (flag == "--INIT_SOL_TYPE")                  cfg.initSolType = stoi(value);
        else if (flag == "--PTL_TEMP_UPDATE_PROPORTION")     cfg.ptlTempUpdateProportion = stoi(value);
        else if (flag == "--INSTANCE_REPORT")                cfg.instanceReport = stoi(value) != 0;
        else if (flag == "--DIFERENT_TOOLSETS_MODE")         cfg.diferentToolsetMode = stoi(value);
        else if (flag == "--PASSO_GATILHO")                  cfg.passoGatilho = stoi(value);
        else if (flag == "--TIME_LIMIT")                     cfg.timeLimit = stoi(value);
        else if (flag == "--SEQUENCE_BY")                    cfg.sequenceBy = stoi(value);

        else if (flag == "--COSTSWITCH")                     cfg.costSwitch = stoi(value);
        else if (flag == "--COSTSWITCHINSTANCE")             cfg.costSwitchInstance = stoi(value);
        else if (flag == "--COSTPRIORITY")                   cfg.costPriority = stoi(value);
        else if (flag == "--PROFITYFINISHED")                cfg.profitFinished = stoi(value);

        else {
            cerr << "Error: unrecognized flag '" << flag << "'.\n\n";
            printUsage(argv[0]);
            exit(1);
        }
    }

    cfg.tempUpdate = cfg.PTL / cfg.ptlTempUpdateProportion;

    return cfg;
}
