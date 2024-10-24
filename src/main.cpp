#include <vector>
#include <iostream>
#include <atomic>
#include <thread>
#include <fstream>
#include <sstream>
#include <string>
#include <algorithm> 
#include <cstdlib>

#include "headers/GlobalVars.h"
#include "headers/SSP.h"

#ifndef IGNORE_FMT
#include <fmt/ranges.h>
#include <fmt/core.h>
#endif

#include "../../PTAPI/include/ExecTime.h"
#include "../../PTAPI/include/PT.h"

using namespace std;


// // ------------------------------------------------------------------------------------------------------------------------------------------------------
// // MAIN
// // ------------------------------------------------------------------------------------------------------------------------------------------------------

// solSSP makeTestSol(int length){
// 	solSSP sol;
// 	for(int i=0; i<length; i++)
// 		sol.sol.push_back(i);
// 	return sol;
// }

// int main(int argc, char* argv[]){
// 	// srand(time(0));
// 	// cout << rand() % 10000 + 100;
// 	// return 0;

//     //pt varibles
// 	float tempIni = 0.01;
// 	float tempfim = 2.0;
// 	int tempN = 10;
// 	int MCL = 0;
// 	int PTL = 2;	
// 	int tempUp = 50;
// 	int tempD = 1;
// 	int uType = 0;
// 	int thN = thread::hardware_concurrency();	
// 	vector<string> arguments(argv + 1, argv + argc);	

//     //problem varibles
// 	int capacityMagazine = 8;
//     int numberMachines   = 2;
//     int planingHorizon   = 2;   
//     int unsupervised     = 0.5*DAY;
// 	int result_report    = 0;
// 	int instance_report  = 0;
	
// 	// Instance file name
// 	string filenameJobs = arguments[0];
// 	string filenameTools = arguments[1];
// 	string filenameoutput = arguments[2];
	
// 	// Read arguments
// 	for(unsigned int i=3; i<arguments.size(); i+=2)
// 	{
		            
//         if(arguments[i]== "--TEMP_FIM")
//             tempfim = stof(arguments[i+1]);
//         else if(arguments[i]== "--TEMP_INIT")
//             tempIni = stof(arguments[i+1]);
//         else if(arguments[i]== "--N_REPLICAS")
//             tempN = stoi(arguments[i+1]);
//         else if(arguments[i]== "--MCL")
//             MCL  = stoi(arguments[i+1]);
//         else if(arguments[i]== "--PTL")
//             PTL = stoi(arguments[i+1]);
//         else if(arguments[i]== "--TEMP_DIST")
//             tempD = stoi(arguments[i+1]);
//         else if(arguments[i]== "--TYPE_UPDATE")
//             uType = stoi(arguments[i+1]);
//         else if(arguments[i]== "--TEMP_UPDATE")
//             tempUp = stoi(arguments[i+1]);
//         else if(arguments[i]== "--THREAD_USED")
//             thN = stoi(arguments[i+1]);
// 		else if(arguments[i]== "--THREAD_USED")
//             thN = stoi(arguments[i+1]);
// 		else if(arguments[i]== "--CAPACITY")
// 			capacityMagazine = stoi(arguments[i+1]);
// 		else if(arguments[i]== "--MACHINES")
// 			numberMachines = stoi(arguments[i+1]);
// 		else if(arguments[i]== "--DAYS")
// 			planingHorizon = stoi(arguments[i+1]);
// 		else if(arguments[i]== "--UNSUPERVISED_MINUTS")
// 			unsupervised = stoi(arguments[i+1]);
// 		else if(arguments[i]== "--RESULT_REPORT")
// 			result_report = stoi(arguments[i+1]);
// 		else if(arguments[i]== "--INSTANCE_REPORT")
// 			instance_report = stoi(arguments[i+1]);
//     }
	
// 	SSP* prob = new SSP(filenameJobs,filenameTools);
// 	prob->setParans(capacityMagazine, numberMachines, planingHorizon, unsupervised);

// 	// ------------------------------------------------------------------------------
// 	// TEST
// 	// ------------------------------------------------------------------------------

// 	prob->printDataReport();

// 	// solSSP sol = prob->construction();
// 	solSSP sol = makeTestSol(11);
	
// 	auto start = chrono::high_resolution_clock::now();
// 	cout << "Evaluation Real: " << -1 * prob->evaluate(sol) << endl;
// 	auto end = chrono::high_resolution_clock::now();
// 	chrono::duration<double> duration = end - start;
// 	cout << "Execution time: " << duration.count() << " seconds" << endl;

// 	cout << "\n\n\n";
// 	cout << "-----------------" << endl;
// 	int valor = prob->evaluateReportGPCA(sol, filenameJobs, filenameTools, filenameoutput, 1);
// 	cout << "-----------------" << endl;
// 	cout << "\n\n\n";

// 	cout << "Evaluation Report: " << valor;

// 	// ------------------------------------------------------------------------------
// 	// TEST
// 	// ------------------------------------------------------------------------------

// 	// if (instance_report)	prob->printDataReport();
// 	// PT<solSSP> algo(tempIni,tempfim,tempN,MCL,PTL,tempD,uType,tempUp);
// 	// ExecTime et;
// 	// solSSP sol = algo.start(thN, prob);
// 	// if (result_report)	prob->evaluateReportKTNS(sol, filenameJobs, filenameTools, filenameoutput, et.getTimeMs());
// 	// auto end = chrono::high_resolution_clock::now();
// 	// cout << (-1) * sol.evalSol << endl;
// 	// cout << et.getTimeMs() << endl;

// 	return 0;
// }










int capacityMagazine = 8;
int numberTools = 21;
int numberJobs = 11;
vector<std::vector<bool>>  toolJob;
vector<std::vector<int>>  JobTools; 

unsigned int GPCA(vector<int> s){	
	
	int pipes_count = 0;
	int last_full = 0;
	vector<int> last_seen(numberTools);
	vector<vector<int>> M;
	

	//Completa o last_seen
	for(unsigned int i = 0; i < numberTools; ++i){
		if(toolJob[i][s[0]]) last_seen[i] = 0;
		else last_seen[i] = -1;
	}
	M.push_back(JobTools[s[0]]);
	
	for(unsigned int e = 1; e < numberJobs; ++e){
		
		M.push_back(JobTools[s[e]]);
				
		for (auto t = JobTools[s[e]].begin(); t != JobTools[s[e]].end(); ++t){
			
			if(last_full <= last_seen[*t]){
				
				++pipes_count;
				
			//	cout<<"PIPE last_see:"<< last_seen[*t] <<" Atual:"<< e <<" last_full:"<<last_full<<"\n";
				
				for(unsigned int i = (last_seen[*t]+1); i < e; ++i){
					
					M[i].push_back(*t);					
					if(M[i].size() == capacityMagazine) last_full = i;
				}
			}
			last_seen[*t] = e; 	
		}
		
		if(M[e].size() == capacityMagazine) last_full = e;

	}

	cout << "Pipes count: " << pipes_count << endl;
	
 return (capacityMagazine - pipes_count);
}

unsigned int toFullMag(vector<int> s){	
	
	int pipes_count = 0;
	int last_full = 0;
	vector<int> last_seen(numberTools);
	vector<vector<int>> M;
	

	//Completa o last_seen
	for(unsigned int i = 0; i < numberTools; ++i){
		if(toolJob[i][s[0]]) last_seen[i] = 0;
		else last_seen[i] = -1;
	}
	M.push_back(JobTools[s[0]]);
	
	
	for(unsigned int e = 1; e < numberJobs; ++e){
		
		M.push_back(JobTools[s[e]]);
				
		for (auto t = JobTools[s[e]].begin(); t != JobTools[s[e]].end(); ++t){
			
			if(last_full <= last_seen[*t]){
				
				++pipes_count;
				
			//	cout<<"PIPE last_see:"<< last_seen[*t] <<" Atual:"<< e <<" last_full:"<<last_full<<"\n";
				
				for(unsigned int i = (last_seen[*t]+1); i < e; ++i){
					
					M[i].push_back(*t);					
					if(M[i].size() == capacityMagazine) last_full = i;
				}
			}
			last_seen[*t] = e; 	
		}
		
		if(M[e].size() == capacityMagazine) last_full = e;

	}
	
	vector<bool> usedG(numberTools, false);
	
	unsigned int i,j;
	for(j=1; j < numberJobs ; ++j){
		i = j - 1;
		
		for (auto t = M[j].begin(); t != M[j].end(); ++t){
			usedG[*t] = true;
		}
		
		for (auto t = M[i].begin(); t != M[i].end(); ++t){
			if((!usedG[*t]) && (M[j].size() < capacityMagazine)){
				M[j].push_back(*t);
			}
		}
		
		for (auto t = M[j].begin(); t != M[j].end(); ++t){
			usedG[*t] = false;
		}		 
	}
	
	for(i=(numberJobs-1); i > 0; --i){
		j = i - 1;
		
		for (auto t = M[j].begin(); t != M[j].end(); ++t){
			usedG[*t] = true;
		}
		
		for (auto t = M[i].begin(); t != M[i].end(); ++t){
			if((!usedG[*t]) && (M[j].size() < capacityMagazine)){
				M[j].push_back(*t);
			}
		}
		
		for (auto t = M[j].begin(); t != M[j].end(); ++t){
			usedG[*t] = false;
		}		 
	}
	
 return (capacityMagazine - pipes_count);
}

int main(){
    vector<vector<bool>> toolJob1(numberTools);
    vector<vector<int>> JobTools1(numberJobs);

    vector<vector<int>> jobToToolMappings = {
        {1,2,3,4,5}, 
        {1,2,3,4,5}, 
        {12,13,14,15,16,17,18}, 
        {12,13,14,15,16,17,18}, 
        {4,5,8,9,10,11,12,13}, 
        {4,5,8,9,10,11,12,13}, 
        {4,5,8,9,10,11,12,13}, 
        {12,13,14,15,16,17,18}, 
        {12,13,14,15,16,17,18}, 
        {5,6,7}, 
        {15,16,17,18,19,20}, 
    };

	for (int i = 0; i < numberTools; i++) {
		vector<bool> tmpVector(numberJobs, false);
		toolJob1[i] = tmpVector;
	}
    
    for (int job = 0; job < numberJobs; job++) {
        for (int tool : jobToToolMappings[job]) {
			toolJob1[tool-1][job] = true;
			JobTools1[job].push_back(tool);
        }
    }

	toolJob = toolJob1;
	JobTools = JobTools1;

	cout << "GPAC: " << GPCA({0,1,2,3,4,5,6,7,8,9,10}) << endl;
}