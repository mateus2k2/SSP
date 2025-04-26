#include <atomic>
#include <algorithm>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <thread>
#include <vector>

#include "headers/GlobalVars.h"
#include "headers/SSP.h"

#ifdef DEBUG
#include <fmt/core.h>
#include <fmt/ranges.h>
#endif

#include "../../PTAPI/include/ExecTime.h"
#include "../../PTAPI/include/PT.h"

using namespace std;

// ------------------------------------------------------------------------------------------------------------------------------------------------------
// MAIN
// ------------------------------------------------------------------------------------------------------------------------------------------------------

int main(int argc, char* argv[]) {
    //pt varibles
	float tempIni = 0.01;
	float tempfim = 2.0;
	int tempN = 10;
	int MCL = 0;
	int PTL = 2;
    int passoGatilho = 0;	
	int tempUp = 50;
	int tempD = 1;
	int uType = 0;
	int thN = thread::hardware_concurrency();
	int initSolType = 0;
	int ptlTempUpProportion = 1;
	vector<string> arguments(argv + 1, argv + argc);	

    //problem varibles
	int capacityMagazine       = 8;
    int numberMachines         = 2;
    int planingHorizon         = 2;
    int unsupervised           = 0.5*DAY;
	int diferent_toolset_mode  = 0;
	int instance_report        = 0;

    // modelo
    int modelo = 0;
	
	// Instance file name
	string filenameJobs = arguments[0];
	string filenameTools = arguments[1];
	string filenameoutput = arguments[2];
	
	// Read arguments
	for(unsigned int i=3; i<arguments.size(); i+=2)
	{
        if(arguments[i]== "--TEMP_FIM")
            tempfim = stof(arguments[i+1]);
        else if(arguments[i]== "--TEMP_INIT")
            tempIni = stof(arguments[i+1]);
        else if(arguments[i]== "--N_REPLICAS")
            tempN = stoi(arguments[i+1]);
        else if(arguments[i]== "--MCL")
            MCL  = stoi(arguments[i+1]);
        else if(arguments[i]== "--PTL")
            PTL = stoi(arguments[i+1]);
        else if(arguments[i]== "--TEMP_DIST")
            tempD = stoi(arguments[i+1]);
        else if(arguments[i]== "--TYPE_UPDATE")
            uType = stoi(arguments[i+1]);
        else if(arguments[i]== "--TEMP_UPDATE")
            tempUp = stoi(arguments[i+1]);
        else if(arguments[i]== "--THREAD_USED")
            thN = stoi(arguments[i+1]);
		else if(arguments[i]== "--INIT_SOL_TYPE")
			initSolType = stoi(arguments[i+1]);
		else if(arguments[i]== "--PTL_TEMP_UPDATE_PROPORTION")
			ptlTempUpProportion = stoi(arguments[i+1]);
		else if(arguments[i]== "--CAPACITY")
			capacityMagazine = stoi(arguments[i+1]);
		else if(arguments[i]== "--MACHINES")
			numberMachines = stoi(arguments[i+1]);
		else if(arguments[i]== "--DAYS")
			planingHorizon = stoi(arguments[i+1]);
		else if(arguments[i]== "--UNSUPERVISED_MINUTS")
			unsupervised = stoi(arguments[i+1]);
		else if(arguments[i]== "--INSTANCE_REPORT")
			instance_report = stoi(arguments[i+1]);
		else if(arguments[i]== "--DIFERENT_TOOLSETS_MODE")
			diferent_toolset_mode = stoi(arguments[i+1]);
        else if(arguments[i]== "--PASSO_GATILHO")
            passoGatilho = stoi(arguments[i+1]);
        else if(arguments[i]== "--MODELO")
            modelo = stoi(arguments[i+1]);
    }
	tempUp = PTL/ptlTempUpProportion;

	SSP* prob = new SSP(filenameJobs,filenameTools,diferent_toolset_mode);

    // ------------------------------------------------------------------------------
    // TEST
    // ------------------------------------------------------------------------------

    // prob->loadInstanceParans(filenameJobs);
    // solSSP sol = prob->randPriority();
    // sol = prob->neighbor(sol);
    // sol = prob->neighbor(sol);
    // sol = prob->neighbor(sol);
    // sol = prob->neighbor(sol);
    // sol = prob->neighbor(sol);
    // sol = prob->neighbor(sol);
    // sol = prob->neighbor(sol);
    // sol = prob->neighbor(sol);
    // sol = prob->neighbor(sol);
    // sol = prob->neighbor(sol);
    // sol = prob->neighbor(sol);
    // sol = prob->neighbor(sol);
    // sol = prob->neighbor(sol);
    // sol = prob->neighbor(sol);
    // sol = prob->neighbor(sol);
    // sol = prob->neighbor(sol);
    // sol = prob->neighbor(sol);
    // sol = prob->neighbor(sol);
    // sol = prob->neighbor(sol);
    // sol = prob->neighbor(sol);
    // sol = prob->neighbor(sol);
    // sol = prob->neighbor(sol);
    // sol = prob->neighbor(sol);
    // prob->evaluateReportKTNS(sol, filenameJobs, filenameTools, filenameoutput, 0, false);
    // solSSP finalSolution = prob->ajustFinalSolution(sol);
    // prob->evaluateReportKTNS(finalSolution, filenameJobs, filenameTools, filenameoutput, 0, true);

    // ------------------------------------------------------------------------------
    // DATA LOADIDNG AND PRE PROCESSING
    // ------------------------------------------------------------------------------

    fstream solutionReportFile;
    solutionReportFile.open(filenameoutput, ios::out);
    if (!solutionReportFile.is_open() && !modelo) {
        cerr << "Error: Could not open solution report file: " << filenameoutput << endl;
        exit(1);
    }

    if (instance_report) prob->printDataReport();
    if (diferent_toolset_mode == 0) prob->groupJobs();

    // ------------------------------------------------------------------------------
    // MODELO
    // ------------------------------------------------------------------------------

    if(modelo){ 
        prob->modelo(filenameoutput);
        return 0;
    }

    // ------------------------------------------------------------------------------
    // SSP
    // ------------------------------------------------------------------------------

	prob->setParans(capacityMagazine, numberMachines, planingHorizon, unsupervised, uType, initSolType);
    PT<solSSP> algo(tempIni, tempfim, tempN, MCL, PTL, passoGatilho, tempD, uType, tempUp);
    ExecTime et;
    solSSP sol = algo.start(thN, prob);
    solSSP finalSolution = sol;

    if (diferent_toolset_mode == 1) finalSolution = prob->postProcessDifferent(sol);
    double cost = prob->evaluateReportKTNS(finalSolution, filenameJobs, filenameTools, solutionReportFile);

    solSSP initFromBest = algo.getInitFromBest();
    vector<solSSP> initAll = algo.getInitAll();

    double meanInitial = 0; 
    std::sort(initAll.begin(), initAll.end(), [](auto a, auto b) { return a.evalSol < b.evalSol; });
    for (size_t i = 0; i < initAll.size(); i++) meanInitial += initAll[i].evalSol;
    meanInitial = meanInitial/initAll.size();

    solutionReportFile << "Final Solution: " << cost << endl;
    solutionReportFile << "Time: " << et.getTimeMs() << endl;
    solutionReportFile << "PTL: " << sol.ptl << endl; //PTL onde a melhor solucao foi gerada
    solutionReportFile << "MCMC: " << sol.mcmc << endl; //Index da cadeia de markove onde a melhor solucao foi gerada
    solutionReportFile << "Best Initial: " << -initAll[0].evalSol << endl;
    solutionReportFile << "Mean Initial: " << -meanInitial << endl;

    solutionReportFile.close();

    cout << "Final Solution: " << cost << endl;
    cout << "Time: " << et.getTimeMs() << endl;

    // fstream solutionReportFileInit;
    // solutionReportFileInit.open("/home/mateus/WSL/IC/SSP/output/Exemplo/exemploInit.txt", ios::out);
    // prob->evaluateReportKTNS(initAll[0], filenameJobs, filenameTools, solutionReportFileInit);

    return 0;
}
