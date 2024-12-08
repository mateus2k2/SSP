#include <vector>
#include <iostream>
#include <atomic>
#include <thread>
#include <fstream>
#include <sstream>
#include <string>
#include <algorithm> 
#include <cstdlib>

#include "headers/GlobalVars.h"
#include "headers/SSP.h"

#ifdef DEBUG
#include <fmt/ranges.h>
#include <fmt/core.h>
#endif

#include "../../PTAPI/include/ExecTime.h"
#include "../../PTAPI/include/PT.h"

using namespace std;


// ------------------------------------------------------------------------------------------------------------------------------------------------------
// MAIN
// ------------------------------------------------------------------------------------------------------------------------------------------------------

solSSP makeTestSol(int length){
	solSSP sol;
	return sol;
}

int main(int argc, char* argv[]){
	// srand(time(0));
	// cout << rand() % 10000 + 100;
	// return 0;

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
	int movementType = 0;
	int initSolType = 0;	
	vector<string> arguments(argv + 1, argv + argc);	

    //problem varibles
	int capacityMagazine = 8;
    int numberMachines   = 2;
    int planingHorizon   = 2;
    int unsupervised     = 0.5*DAY;
	int result_report    = 0;
	int instance_report  = 0;
	int instance_mode	 = 0;
	
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
		else if(arguments[i]== "--CAPACITY")
			capacityMagazine = stoi(arguments[i+1]);
		else if(arguments[i]== "--MACHINES")
			numberMachines = stoi(arguments[i+1]);
		else if(arguments[i]== "--DAYS")
			planingHorizon = stoi(arguments[i+1]);
		else if(arguments[i]== "--UNSUPERVISED_MINUTS")
			unsupervised = stoi(arguments[i+1]);
		else if(arguments[i]== "--RESULT_REPORT")
			result_report = stoi(arguments[i+1]);
		else if(arguments[i]== "--INSTANCE_REPORT")
			instance_report = stoi(arguments[i+1]);
		else if(arguments[i]== "--INSTANCE_MODE")
			instance_mode = stoi(arguments[i+1]);	
		else if(arguments[i]== "--MOVEMENT_TYPE")
			movementType = stoi(arguments[i+1]);
		else if(arguments[i]== "--INIT_SOL_TYPE")
			initSolType = stoi(arguments[i+1]);
    }
	tempUp = PTL/5;
	SSP* prob = new SSP(filenameJobs,filenameTools);
	prob->setParans(capacityMagazine, numberMachines, planingHorizon, unsupervised, movementType, initSolType);

	// ------------------------------------------------------------------------------
	// TEST
	// ------------------------------------------------------------------------------

	// for (int i = 0; i < 10; i++){
	// 	string filenameoutputCur = "./output/Exemplo/output" + to_string(i) + ".txt";
	// 	solSSP sol = prob->construction();
	// 	for(int i=0; i<sol.sol.size(); i++) cout << sol.sol[i] << " ";
	// 	cout << endl;
	// 	int valor = prob->evaluateReportKTNS(sol, filenameJobs, filenameTools, filenameoutputCur, 1);
	// }

	// solSSP sol = makeTestSol(-1);
	// int valor = prob->evaluateReportKTNS(sol, filenameJobs, filenameTools, filenameoutput, 1);

	// ------------------------------------------------------------------------------
	// REAL
	// ------------------------------------------------------------------------------

	prob->loadInstanceParans(filenameJobs);

	prob->setupRelaseDate();

	if (instance_mode == 1) prob->groupJobs();
	if (instance_report) prob->printDataReport();
	
	PT<solSSP> algo(tempIni,tempfim,tempN,MCL,PTL,tempD,uType,tempUp);
	ExecTime et;
	solSSP sol = algo.start(thN, prob);
	
	if (result_report)	prob->evaluateReportKTNS(sol, filenameJobs, filenameTools, filenameoutput, et.getTimeMs());
	cout << (-1) * sol.evalSol << endl;
	cout << et.getTimeMs() << endl;

	return 0;
}

