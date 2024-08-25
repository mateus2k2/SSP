#include <vector>
#include <iostream>
#include <atomic>
#include <thread>
#include <fstream>
#include <sstream>
#include <string>
#include <algorithm> 
#include <fmt/ranges.h>
#include <fmt/core.h>

#include "headers/GlobalVars.h"
#include "headers/loadData.h"
#include "headers/cost.h"
#include "headers/solutionGenerator.h"

using namespace std;

fstream solutionReportFile;

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
map<int, ToolSet> originalToolSets;

// ------------------------------------------------------------------------------------------------------------------------------------------------------
// MAIN
// ------------------------------------------------------------------------------------------------------------------------------------------------------

int main(){
    cout << endl;
    
    // std::vector<int> vec = {1, 2, 3, 4, 5};
    // fmt::print("Vector elements: {}\n", vec);

    // randonSolution();
    exemploArtigo();
    

	return 0;
}
