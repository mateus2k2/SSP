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
// VARIVEIS
// ------------------------------------------------------------------------------------------------------------------------------------------------------

fstream solutionReportFile;

// ------------------------------------------------------------------------------------------------------------------------------------------------------
// MAIN
// ------------------------------------------------------------------------------------------------------------------------------------------------------

int main(int argc, char* argv[]){
    cout << endl;

    //varibles
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
	
	// Instance file name
	string filenameJobs = arguments[0];
	string filenameTools = arguments[1];
	
	// Read arguments
	for(unsigned int i=2; i<arguments.size(); i+=2)
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
    }
	

	// Create SSP object
	int capacityMagazine = 8;
    int numberMachines = 2;
    int planingHorizon = 2;   
    int unsupervised   = 0.5*DAY;
	SSP* prob = new SSP(filenameJobs,filenameTools);
	prob->setParans(capacityMagazine, numberMachines, planingHorizon, unsupervised);
	// prob->printDataReport();
	
	
	// Create and start PT 
	PT<solSSP> algo(tempIni,tempfim,tempN,MCL,PTL,tempD,uType,tempUp);
	ExecTime et;
	solSSP sol = algo.start(thN, prob);
	et.printTime();
	cout << sol.evalSol << "\n";

	return 0;
}

void exemploArtigo(){
    // solutionReportFile.open("/home/mateus/WSL/IC/SSP/output/exemploArtigo.txt", ios::out);
    
    // // ---------------------------------------------------------------------

    // laodToolSet("/home/mateus/WSL/IC/SSP/input/ExemploArtigo/ToolSets.csv");
	// laodInstance("/home/mateus/WSL/IC/SSP/input/ExemploArtigo/Jobs.csv");
	// solutionReportFile << "/home/mateus/WSL/IC/SSP/input/ExemploArtigo/Jobs.csv" << ";" << "/home/mateus/WSL/IC/SSP/input/ExemploArtigo/ToolSets.csv" << endl;

    // // ---------------------------------------------------------------------

    // capacityMagazine = 8;
    // numberMachines = 2;
    // planingHorizon = 2;   
    // unsupervised   = 0.5*DAY;

    // vector<int> sol(numberJobs);
    // for (int i = 0; i < numberJobs; ++i) {
    //     sol[i] = i;
    // }

    // // ---------------------------------------------------------------------

    // printDataReport();
    // evaluate(sol);
}

void randonSolution(){
    // solutionReportFile.open("/home/mateus/WSL/IC/SSP/output/solutionReport.txt", ios::out);
    
    // // ---------------------------------------------------------------------

    // string jobsFileName = "MyInstances/n=75,p=0.24,r=0.5,t=650,v0.csv";
    // string toolSetFileName = "ToolSetInt.csv";

    // laodToolSet("/home/mateus/WSL/IC/SSP/input/" + toolSetFileName);
	// laodInstance("/home/mateus/WSL/IC/SSP/input/" + jobsFileName);
	// solutionReportFile << jobsFileName << ";" << toolSetFileName << endl;

    // // ---------------------------------------------------------------------

    // numberMachines = 2;
    // vector<int> sol(numberJobs);
    // for (int i = 0; i < numberJobs; ++i) {
    //     sol[i] = i;
    // }

    // // ---------------------------------------------------------------------
    
    // //fazer divisao entre as maquinas baseado no contador de tempo

    // // printDataReport();
    // evaluate(sol);
   
}
