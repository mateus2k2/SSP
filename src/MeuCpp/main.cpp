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

map<int, vector<int>>  mapToolSets;

vector<int> operation;		 	
vector<int> job;			  
vector<int> processingTime;	 
vector<int> machine;		 
vector<int> priority;		 
int planingHorizon;  		 
int unsupervised; 			 
int currantHour;  		      
int currantDay; 			 

int numberMachines;		   
int numberTools;      	    
int numberJobs; 	   	   
int capacityMagazine;     
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

	laodToolSet("/home/mateus/WSL/IC/data/Exemple Tool Sets.csv");
	cout << "Map Too Sets: " << endl;
	for (const auto& pair : mapToolSets) {
        cout << "Key: " << pair.first << ", Value: ";
		for (const auto &t : pair.second) {
			cout << t << " ";
		}
		cout << endl;
    }
    cout << endl << endl;

	// ------------------------------------------------------------------------------------------------------------------------

	laodRealInstance("/home/mateus/WSL/IC/data/Exemple Jobs.csv");

    cout << "Operation: " << endl;
    for (const auto &op : operation) {
        cout << op << " ";
    }
    cout << endl << endl;

    cout << "Job: " << endl;
    for (const auto &j : job) {
        cout << j << " ";
    }
    cout << endl << endl;

    cout << "JobTools: " << endl;
    for (const auto &ts : JobTools) {
        for (const auto &t : ts) {
			cout << t << " ";
		}
		cout << endl;
    }
    cout << endl << endl;

	cout << "toolJob: " << endl;
    for (const auto &ts : toolJob) {
        for (const auto &t : ts) {
			cout << t << " ";
		}
		cout << endl;
    }
    cout << endl << endl;

    cout << "Processing Time: " << endl;
    for (const auto &pt : processingTime) {
        cout << pt << " ";
    }
    cout << endl << endl;

	// ------------------------------------------------------------------------------------------------------------------------

	vector<int> s(numberJobs);
	s = {0,1,2,3,4,5,6,7,8,9};
	// makeJobTools();
	// makeToolJob();

	int KTNSSum = KTNS(s);
    cout << "Solucao KTNS: "    << KTNSSum 	  << endl;

	int GPCASum = GPCA(s);
	cout << "Solucao GPCA: "    << GPCASum 	  << endl;
	
	tKT = thread(threadRunGPCA);
	int GPCAParSum = GPCAPar(s);
	stop = true;
	tKT.join();
	cout << "Solucao GPSAPar: " << GPCAParSum << endl;

	return 0;
}