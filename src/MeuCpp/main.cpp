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

using namespace std;

// ------------------------------------------------------------------------------------------------------------------------------------------------------
// VARIVEIS DADOS
// ------------------------------------------------------------------------------------------------------------------------------------------------------

vector<int> operation;		    		
vector<int> job;			    	
vector<int> toolSet;		    	
vector<int> processingTime;	    	
vector<vector<int>> toolSets;   
vector<int> machine;		   		
vector<int> priority;		   	
int planingHorizon;
int unsupervised;		   

int numberMachines = 1;		   
int numberTools = 20;      	    
int numberJobs = 10; 	   	   
int capacityMagazine = 8;     
vector<vector<int>> JobTools;  
vector<vector<bool>> toolJob;  

unsigned int sum = 0;   	   

// ------------------------------------------------------------------------------------------------------------------------------------------------------
// VARIVEIS GLOBAIS
// ------------------------------------------------------------------------------------------------------------------------------------------------------

int meio = 0 ; 				   
vector<int> neigh; 			   
atomic_bool fase{true}; 	   
atomic_bool endD{false};	   
atomic_bool stop{false};	   
vector<bool> magazineD;		   
int evalSolD;				   
thread tKT;					   
int last_fullD;				   
vector<int> last_seenD;		   
vector<vector<int>> MD;		   
int pipes_countD = 0;		   


// ------------------------------------------------------------------------------------------------------------------------------------------------------
// MAIN
// ------------------------------------------------------------------------------------------------------------------------------------------------------


int main(){

	// vector<int> s(numberJobs);
	// s = {0,1,2,3,4,5,6,7,8,9};
    // makeJobTools();
	// makeToolJob();

	// int KTNSSum = KTNS(s);
    // cout << "Solucao KTNS: "    << KTNSSum 	  << endl;

	// int GPCASum = GPCA(s);
	// cout << "Solucao GPCA: "    << GPCASum 	  << endl;
	
	// tKT = thread(threadRunGPCA);
	// int GPCAParSum = GPCAPar(s);
	// stop = true;
	// tKT.join();
	// cout << "Solucao GPSAPar: " << GPCAParSum << endl;
  

	// ------------------------------------------------------------------------------------------------------------------------

    // loadInstance("/home/mateus/WSL/IC/data/Example.txt");
    
    // cout << "numberJobs: " << numberJobs << "\n";
    // cout << "numberTools: " << numberTools << "\n";
    // cout << "capacityMagazine: " << capacityMagazine << "\n";
    // cout << "sum: " << sum << "\n";
    
    // for(unsigned int i = 0; i < numberJobs; ++i){
    //     cout << "JobTools[" << i << "]: ";
    //     for (auto t = JobTools[i].begin(); t != JobTools[i].end(); ++t){
    //         cout << *t << " ";
    //     }
    //     cout << "\n";
    // }
    
    // for(unsigned int i = 0; i < numberTools; ++i){
    //     cout << "toolJob[" << i << "]: ";
    //     for (auto t = toolJob[i].begin(); t != toolJob[i].end(); ++t){
    //         cout << *t << " ";
    //     }
    //     cout << "\n";
    // }

	// ------------------------------------------------------------------------------------------------------------------------

	// laodRealInstance("/home/mateus/WSL/IC/data/Data Jobs 250.csv");

	// // Print the vectors
    // cout << "Operation: ";
    // for (const auto &op : operation) {
    //     cout << op << " ";
    // }
    // cout << endl;

    // cout << "Job: ";
    // for (const auto &j : job) {
    //     cout << j << " ";
    // }
    // cout << endl;

    // cout << "ToolSet: ";
    // for (const auto &ts : toolSet) {
    //     cout << ts << " ";
    // }
    // cout << endl;

    // cout << "Processing Time: ";
    // for (const auto &pt : processingTime) {
    //     cout << pt << " ";
    // }
    // cout << endl;

	// ------------------------------------------------------------------------------------------------------------------------

	// laodToolSet("/home/mateus/WSL/IC/data/Data Tool Set Compositions.csv");
	
	// print toolSets
	// for (const auto &line : toolSets) {
	//     for (const auto &value : line) {
	//         cout << value << " ";
	//     }
	//     cout << endl;
	// }

    // auto max_itr = max_element(toolSets.begin(), toolSets.end(),
    //     [](const vector<int>& a, const vector<int>& b) {
    //         return a.size() < b.size();
    //     });

    // cout << (*max_itr).size() << endl;

    // for (int element : *max_itr) {
    //     cout << element << " ";
    // }

	// return 0;

	// ------------------------------------------------------------------------------------------------------------------------

	return 0;
}