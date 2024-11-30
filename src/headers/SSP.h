
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

#include "../../../PTAPI/include/Problem.h"
#include "GlobalVars.h"


struct solSSP: public solution{ 
  std::vector<int> sol; 
};


class SSP: public Problem<solSSP>{
	private:

		int planingHorizon;
		int unsupervised;
		int numberMachines;
		int numberTools;
		int numberToolsReal;
		int numberJobs;
		int capacityMagazine;

    	map<int, int> ferramentas;
		vector<vector<bool>> toolJob;
		
		vector<Job> originalJobs;
		map<int, ToolSet> originalToolSets;
		map<int, ToolSet> normalizedToolSets;
		
	public:
		SSP(std::string filenameJobs, std::string filenameTools);
		~SSP();

		int laodInstance(string filename);
		int laodToolSet(string filename);
		void printDataReport();
		void setParans(int capacityMagazine, int numberMachines, int planingHorizon, int unsupervised);
		int lowerBound();	
		void groupJobs();

		solSSP construction();
		solSSP neighbor(solSSP sol);
		
		double evaluate(solSSP sol);
		double evaluateReportKTNS(solSSP sol, string filenameJobs, string filenameTools, string solutionReportFileName, int time);
		// double evaluateReportGPCA(solSSP sol, string filenameJobs, string filenameTools, string solutionReportFileName, int time);
		
		void destroyToolSets();
};

#endif 
