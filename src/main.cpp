#include <vector>
#include <iostream>
#include <atomic>
#include <thread>
#include <fstream>
#include <sstream>
#include <string>
#include <algorithm> 
#include <cstdlib>
#include <fmt/ranges.h>
#include <fmt/core.h>

#include "headers/GlobalVars.h"
#include "headers/SSP.h"

#include "../../PTAPI/include/ExecTime.h"
#include "../../PTAPI/include/PT.h"

using namespace std;


// ------------------------------------------------------------------------------------------------------------------------------------------------------
// MAIN
// ------------------------------------------------------------------------------------------------------------------------------------------------------

int main(int argc, char* argv[]){
    cout << endl;

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
	vector<string> arguments(argv + 1, argv + argc);	

    //problem varibles
	int capacityMagazine = 8;
    int numberMachines = 2;
    int planingHorizon = 2;   
    int unsupervised   = 0.5*DAY;
	
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
		else if(arguments[i]== "--UNSUPERVISEDMINUTS")
			unsupervised = stoi(arguments[i+1]);
    }
	
	// Create SSP object
	SSP* prob = new SSP(filenameJobs,filenameTools);
	prob->setParans(capacityMagazine, numberMachines, planingHorizon, unsupervised);

	// prob->printDataReport();
	// solSSP sol = prob->construction();
	// fmt::print("Initial solution: {}\n", sol.sol);
	// prob->evaluateReport(sol, "filenameJobs", "filenameTools", filenameoutput, 1);

	// Create and start PT
	PT<solSSP> algo(tempIni,tempfim,tempN,MCL,PTL,tempD,uType,tempUp);
	ExecTime et;
	solSSP sol = algo.start(thN, prob);
	prob->evaluateReport(sol, "filenameJobs", "filenameTools", filenameoutput, et.getTimeMs());

	return 0;
}
