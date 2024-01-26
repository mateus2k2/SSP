#include <vector>
#include <iostream>
#include <atomic>
#include <thread>
#include <fstream>
#include <sstream>
#include <string>
#include <algorithm> 

#include "headers/GlobalVars.h"
#include "headers/loadData.h"
#include "headers/cost.h"
#include "headers/super.h"
#include "headers/solutionGenerator.h"

using namespace std;

void exempleOriginal(){

    fstream solutionReportFile;
    solutionReportFile.open("/home/mateus/WSL/IC/data/solutionReport.txt", ios::out);
    
	laodToolSet("/home/mateus/WSL/IC/data/Exemplo/ToolSets.csv");
	laodInstance("/home/mateus/WSL/IC/data/Exemplo/Jobs.csv");
    
    priority        = {1,1,0,0,0,1,1,0,0,1};
    numberMachines  = 2;
    planingHorizon = 7;   		  
    unsupervised   = 0.5*TIMESCALE;

    loadDataTypes();
    makeSuper();
    makePriorityIndex();

    vector<int> sol     = {2,5,6,3,4,7,8,0,1,9};
    vector<int> machine = {0,4};

    printDataReport();
    costReport(sol, machine, solutionReportFile);
}

void exempleDupe(){
    fstream solutionReportFile;
    solutionReportFile.open("/home/mateus/WSL/IC/data/solutionReport.txt", ios::out);
    
	laodToolSet("/home/mateus/WSL/IC/data/Exemplo24/ToolSets2.csv");
	laodInstance("/home/mateus/WSL/IC/data/Exemplo24/Jobs2.csv");

	laodToolSet("/home/mateus/WSL/IC/data/Exemplo1440/ToolSets2.csv");
	laodInstance("/home/mateus/WSL/IC/data/Exemplo1440/Jobs2.csv");

    priority       = {1,1,0,0,0,1,1,0,0,1,1};
    numberMachines = 2;
    planingHorizon = 7;   		  
    unsupervised   = 0.5*TIMESCALE;

    loadDataTypes();
    makeSuper();
    makePriorityIndex();

    vector<int> sol             = {0,1,2,3,4};
    vector<int> machine         = {0,3};

    printDataReport();
    costReport(sol, machine, solutionReportFile);
}

void full250(){
    fstream solutionReportFile;
    solutionReportFile.open("/home/mateus/WSL/IC/data/solutionReport.txt", ios::out);
    
    // ---------------------------------------------------------------------

    // laodToolSet("/home/mateus/WSL/IC/data/1000ToolSet.csv");
	// laodInstance("/home/mateus/WSL/IC/data/1000.csv");

    // laodToolSet("/home/mateus/WSL/IC/data/750ToolSet.csv");
	// laodInstance("/home/mateus/WSL/IC/data/750.csv");

    laodToolSet("/home/mateus/WSL/IC/data/250ToolSet.csv");
	laodInstance("/home/mateus/WSL/IC/data/250.csv");

    priority.resize(numberJobs);
    for (int i = 0; i < numberJobs; ++i) {
        priority[i] = 1;
    }
    numberMachines = 1;
    planingHorizon = 7;   		  
    unsupervised   = 0.5*TIMESCALE;

    loadDataTypes();
    makeSuper();
    makePriorityIndex();

    vector<int> sol(numberJobs);
    for (int i = 0; i < numberJobs; ++i) {
        sol[i] = i;
    }

    vector<int> machine(numberMachines);
    int step = numberJobs/numberMachines;
    for (int i = 0; i < numberMachines; ++i) {
        machine[i] = i*step;
    }
    

    // ---------------------------------------------------------------------

    printDataReport();
    costReport(sol, machine, solutionReportFile);
   
}

void full750(){
    fstream solutionReportFile;
    solutionReportFile.open("/home/mateus/WSL/IC/data/solutionReport.txt", ios::out);
    
    // ---------------------------------------------------------------------

    laodToolSet("/home/mateus/WSL/IC/data/750ToolSet.csv");
	laodInstance("/home/mateus/WSL/IC/data/750.csv");

    priority.resize(numberJobs);
    for (int i = 0; i < numberJobs; ++i) {
        priority[i] = 1;
    }
    numberMachines = 1;
    planingHorizon = 7;   		  
    unsupervised   = 0.5*TIMESCALE;

    loadDataTypes();
    makeSuper();
    makePriorityIndex();

    vector<int> sol(numberJobs);
    for (int i = 0; i < numberJobs; ++i) {
        sol[i] = i;
    }

    vector<int> machine(numberMachines);
    int step = numberJobs/numberMachines;
    for (int i = 0; i < numberMachines; ++i) {
        machine[i] = i*step;
    }
    

    // ---------------------------------------------------------------------

    printDataReport();
    costReport(sol, machine, solutionReportFile);
   
}

void full1000(){
    fstream solutionReportFile;
    solutionReportFile.open("/home/mateus/WSL/IC/data/solutionReport.txt", ios::out);
    
    // ---------------------------------------------------------------------

    laodToolSet("/home/mateus/WSL/IC/data/1000ToolSet.csv");
	laodInstance("/home/mateus/WSL/IC/data/1000.csv");

    priority.resize(numberJobs);
    for (int i = 0; i < numberJobs; ++i) {
        priority[i] = 1;
    }
    numberMachines = 1;
    planingHorizon = 7;   		  
    unsupervised   = 0.5*TIMESCALE;

    loadDataTypes();
    makeSuper();
    makePriorityIndex();

    vector<int> sol(numberJobs);
    for (int i = 0; i < numberJobs; ++i) {
        sol[i] = i;
    }

    vector<int> machine(numberMachines);
    int step = numberJobs/numberMachines;
    for (int i = 0; i < numberMachines; ++i) {
        machine[i] = i*step;
    }
    

    // ---------------------------------------------------------------------

    printDataReport();
    costReport(sol, machine, solutionReportFile);
   
}

void randonSolution(){
    fstream solutionReportFile;
    solutionReportFile.open("/home/mateus/WSL/IC/data/SolutionReports/solutionReport.txt", ios::out);
    
    // ---------------------------------------------------------------------

    // laodToolSet("/home/mateus/WSL/IC/data/1000ToolSet.csv");
	// laodInstance("/home/mateus/WSL/IC/data/1000.csv");

    // laodToolSet("/home/mateus/WSL/IC/data/750ToolSet.csv");
	// laodInstance("/home/mateus/WSL/IC/data/750.csv");

    laodToolSet("/home/mateus/WSL/IC/data/1000ToolSet.csv");
	laodInstance("/home/mateus/WSL/IC/data/1000.csv");

    priority.resize(numberJobs);
    for (int i = 0; i < numberJobs; ++i) {
        priority[i] = 1;
    }
    numberMachines = 2;
    planingHorizon = 7;   		  
    unsupervised   = 0.5*TIMESCALE;

    loadDataTypes();
    makeSuper();
    makePriorityIndex();

    vector<int> sol(numberJobs);
    for (int i = 0; i < numberJobs; ++i) {
        sol[i] = i;
    }

    vector<int> machine(numberMachines);
    int step = numberJobs/numberMachines;
    for (int i = 0; i < numberMachines; ++i) {
        machine[i] = i*step;
    }

    // ---------------------------------------------------------------------

    printDataReport();
    // costReport(sol, machine, solutionReportFile);
   
}
