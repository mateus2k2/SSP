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

using namespace std;

// ------------------------------------------------------------------------------------------------------------------------------------------------------
// VARIVEIS PARA CARREGAR DADOS
// ------------------------------------------------------------------------------------------------------------------------------------------------------

map<int, vector<int>>  mapToolSets;
vector<int> JobToolsIndex;
vector<vector<int>> JobTools;

vector<int> operation;		 	
vector<int> job;			  
vector<int> processingTime;	 
vector<int> priority;		 

// ------------------------------------------------------------------------------------------------------------------------------------------------------
// VARIVEIS RESOLVER O PROBLEMA
// ------------------------------------------------------------------------------------------------------------------------------------------------------

int planingHorizon;  		 
int unsupervised; 			 
int numberMachines;		   
int numberTools;      	    
int numberJobs; 	   	   
int capacityMagazine;     

vector<Job> originalJobs;
vector<ToolSet> originalToolSets;

vector<SuperJob> superJobs;
vector<SuperToolSet> superToolSet;

set<pair<int,int>> priorityIndex;

// ------------------------------------------------------------------------------------------------------------------------------------------------------
// MAIN
// ------------------------------------------------------------------------------------------------------------------------------------------------------

int main(){
    cout << endl;

    fstream solutionReportFile;
    solutionReportFile.open("/home/mateus/WSL/IC/data/solutionReport.txt", ios::out);
    
    // ---------------------------------------------------------------------

	// laodToolSet("/home/mateus/WSL/IC/data/Exemplo/ToolSets.csv");
	// laodInstance("/home/mateus/WSL/IC/data/Exemplo/Jobs.csv");
    
    // priority        = {1,1,0,0,0,1,1,0,0,1};
    // numberMachines  = 2;
    // planingHorizon = 7;   		  
    // unsupervised   = 0.5*TIMESCALE;

    // loadDataTypes();
    // makeSuper();
    // makePriorityIndex();

    // vector<int> sol     = {2,5,6,3,4,7,8,0,1,9};
    // vector<int> machine = {1,1,0,0,0,0,0,1,1,1};


    // ---------------------------------------------------------------------

	// laodToolSet("/home/mateus/WSL/IC/data/Exemplo24/ToolSets2.csv");
	// laodInstance("/home/mateus/WSL/IC/data/Exemplo24/Jobs2.csv");

	// // laodToolSet("/home/mateus/WSL/IC/data/Exemplo1440/ToolSets2.csv");
	// // laodInstance("/home/mateus/WSL/IC/data/Exemplo1440/Jobs2.csv");

    // priority       = {1,1,0,0,0,1,1,0,0,1,1};
    // numberMachines = 2;
    // planingHorizon = 7;   		  
    // unsupervised   = 0.5*TIMESCALE;

    // loadDataTypes();
    // makeSuper();
    // makePriorityIndex();

    // vector<int> sol             = {0,1,2,3,4};
    // vector<int> machine         = {1,1,0,0,0};

    
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

    vector<int> machine(numberJobs);
    for (int i = 0; i < numberJobs; ++i) {
        machine[i] = 0;
    }
    

    // ---------------------------------------------------------------------

    printDataReport();
    costReport(sol, machine, solutionReportFile);

	return 0;
}