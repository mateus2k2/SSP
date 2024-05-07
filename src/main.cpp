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

    randonSolution();
    // exemploArtigo();


	return 0;
}
