#include <algorithm>
#include <atomic>
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
    int print_time             = 1;
    int print_cost             = 1;
	
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
        else if(arguments[i]== "--PRINT_TIME")
            print_time = stoi(arguments[i+1]);
        else if(arguments[i]== "--PRINT_COST")
            print_cost = stoi(arguments[i+1]);

    }
	tempUp = PTL/ptlTempUpProportion;

	SSP* prob = new SSP(filenameJobs,filenameTools);
	prob->setParans(capacityMagazine, numberMachines, planingHorizon, unsupervised, uType, initSolType, diferent_toolset_mode);

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
    // REAL
    // ------------------------------------------------------------------------------

    prob->loadInstanceParans(filenameJobs);
    if (instance_report) prob->printDataReport();
    if (diferent_toolset_mode == 0) prob->groupJobs();

    PT<solSSP> algo(tempIni, tempfim, tempN, MCL, PTL, tempD, uType, tempUp);
    ExecTime et;
    solSSP sol = algo.start(thN, prob);
    solSSP finalSolution = sol;

    if (diferent_toolset_mode == 1) finalSolution = prob->postProcessDifferent(sol);
    double cost = prob->evaluateReportKTNS(finalSolution, filenameJobs, filenameTools, filenameoutput, et.getTimeMs());

    if(print_cost) cout << (-1) * cost << endl;
    if(print_time) cout << et.getTimeMs() << endl;

    return 0;
}