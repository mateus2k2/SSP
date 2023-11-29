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
// VARIVEIS DADOS
// ------------------------------------------------------------------------------------------------------------------------------------------------------

map<int, vector<int>>  mapToolSets;

vector<int> operation;		 	
vector<int> job;			  
vector<int> processingTime;	 
vector<int> machine;		 
vector<int> priority;		 
int planingHorizon;  		 
int unsupervised; 			 

int numberMachines;		   
int numberTools;      	    
int numberJobs; 	   	   
int capacityMagazine;     
vector<int> JobToolsIndex;
vector<vector<int>> JobTools;  
vector<vector<bool>> toolJob;  

unsigned int sum;

// ------------------------------------------------------------------------------------------------------------------------------------------------------
// MAIN
// ------------------------------------------------------------------------------------------------------------------------------------------------------


int main(){ 

    // ---------------------------------------------------------------------

	laodToolSet("/home/mateus/WSL/IC/data/Exemplo/ToolSets.csv");
	laodRealInstance("/home/mateus/WSL/IC/data/Exemplo/Jobs.csv");

    vector<int> sol = {2,5,6,3,4,7,8,0,1,9};
    machine         = {1,1,0,0,0,0,0,1,1,1};
    priority        = {1,1,0,0,0,1,1,0,0,1};
    numberMachines  = 2;

    loadDataTypes();

    // ---------------------------------------------------------------------

    // laodToolSet("/home/mateus/WSL/IC/data/250ToolSet.csv");
	// laodRealInstance("/home/mateus/WSL/IC/data/250.csv");

    // vector<int> sol(numberJobs);
    // for (int i = 0; i < numberJobs; ++i) {
    //     sol[i] = i;
    // }
    // priority.resize(numberJobs);
    // for (int i = 0; i < numberJobs; ++i) {
    //     priority[i] = 1;
    // }
    // machine.resize(numberJobs);
    // for (int i = 0; i < numberJobs; ++i) {
    //     machine[i] = 0;
    // }
    // numberMachines = 1;jh

    // ---------------------------------------------------------------------

    // printDataReport();
    // printSolutionReport(sol);

	return 0;
}