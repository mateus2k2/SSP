
#ifndef SSP_H
#define SSP_H

#include <string.h>
#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>
#include <queue>
#include <utility>
#include <forward_list>
#include <list>
#include <set>
#include <map>
#include <omp.h>
#include <limits.h>
#include <random>
#include <cmath>
#include <algorithm>
#include <iterator>
#include <iostream>
#include <thread>
#include <atomic>
#include <functional> 
#include "gurobi_c++.h"

#include "../../../PTAPI/include/Problem.h"
#include "GlobalVars.h"

struct solSSP: public solution{ 
	std::vector<int> sol; 
	std::vector<int> releaseDates;
	std::vector<int> dueDates;
};

class SSP: public Problem<solSSP>{
	private:
		int planingHorizon;
		int unsupervised;
		int numberMachines;
		int numberTools;
		int numberToolsReal;
		int numberJobs;
		int numberJobsUngrouped;
		int numberOfPriorityJobs;
		int capacityMagazine;
		int diferent_toolset_mode;
		
		string inputJobsFile;
		string inputToolsetsFile;

		int numberOfTools;
    	map<int, int> ferramentas;
    	map<int, int> ferramentNormalizadaXFerramentaReal;
		vector<vector<bool>> toolJob;
		
		vector<Job> originalJobs;
		map<int, ToolSet> originalToolSets;
		map<int, ToolSet> normalizedToolSets;

		std::function<solSSP(solSSP)> neighborFunc;
    	std::function<solSSP()> constructionFunc;
		
	public:
		SSP(std::string filenameJobs, std::string filenameTools, int diferent_toolset_mode);
		~SSP();

		int laodInstance(string filename);
		int laodToolSet(string filename);
		int loadInstanceParans(string filename);
		void printDataReport();
		void setParans(int movementType, int initSolType);

		solSSP construction();
		solSSP randPriority();
		solSSP rand();

		solSSP neighbor(solSSP sol);
		solSSP two_opt(solSSP sol);
		solSSP two_swap(solSSP sol);
		solSSP insertion(solSSP sol);
		solSSP total_rand(solSSP sol);

		void groupJobs();
		double evaluate(solSSP& s);
		double evaluateReportKTNS(solSSP &sol, string filenameJobs, string filenameTools, fstream& solutionReportFile);
		tuple<int, int, int, int> evaluateReportKTNSMachine(vector<int> s, fstream& solutionReportFile, int machine);
		solSSP postProcessDifferent(solSSP &sol);
		
		int modelo(string filenameoutput, int timeLimit);
		void loadModelData();
		void convertModelData(string& folderOutput, GRBModel& model);

		vector<Machine> practitioner(fstream& solutionReportFile, int condition);
		void reportDataPractitioner(fstream& solutionReportFile, string filenameJobs, string filenameTools);
		void allocateOperationsToMachines(int numMachines);
		void createSchedules(int condition);
};

#endif 
