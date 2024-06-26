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

void exemploArtigo(){
    fstream solutionReportFile;
    solutionReportFile.open("/home/mateus/WSL/IC/SSP/output/exemploArtigo.txt", ios::out);
    
    // ---------------------------------------------------------------------

    laodToolSet("/home/mateus/WSL/IC/SSP/input/ExemploArtigo/ToolSets.csv");
	laodInstance("/home/mateus/WSL/IC/SSP/input/ExemploArtigo/Jobs.csv");
	solutionReportFile << "/home/mateus/WSL/IC/SSP/input/ExemploArtigo/Jobs.csv" << ";" << "/home/mateus/WSL/IC/SSP/input/ExemploArtigo/ToolSets.csv" << endl;

    // ---------------------------------------------------------------------

    capacityMagazine = 8;
    numberMachines = 2;
    planingHorizon = 2;   
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
    solutionReportFile.open("/home/mateus/WSL/IC/SSP/output/solutionReport.txt", ios::out);
    
    // ---------------------------------------------------------------------

    string jobsFileName = "MyInstances/n=75,p=0.24,r=0.5,t=650,v0.csv";
    string toolSetFileName = "ToolSetInt.csv";

    laodToolSet("/home/mateus/WSL/IC/SSP/input/" + toolSetFileName);
	laodInstance("/home/mateus/WSL/IC/SSP/input/" + jobsFileName);
	solutionReportFile << jobsFileName << ";" << toolSetFileName << endl;

    // ---------------------------------------------------------------------

    numberMachines = 2;

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

    // printDataReport();
    costReport(sol, machine, solutionReportFile);
   
}
