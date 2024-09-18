#include "headers/SSP.h"

SSP::SSP(std::string filenameJobs, std::string filenameTools){
    laodToolSet(filenameTools);
    laodInstance(filenameJobs);
}

SSP::~SSP(){	
}

void SSP::setParans(int capacityMagazine1, int numberMachines1, int planingHorizon1, int unsupervised1){
	this->capacityMagazine = capacityMagazine1;
	this->numberMachines = numberMachines1;
	this->planingHorizon = planingHorizon1;
	this->unsupervised = unsupervised1;
}

solSSP SSP::construction(){
	solSSP ss;
    std::random_device rnd_device;
    std::mt19937 mersenne_engine {rnd_device()}; 
    	
	for(int i=0; i< numberJobs; i++){
		ss.sol.push_back(i);
	}
	

	std::shuffle(begin(ss.sol), end(ss.sol), mersenne_engine);
	
	ss.evalSol = evaluate(ss);
	ss.Nup = false;
	ss.Ndown = false;
	 
	return ss;
}

solSSP SSP::neighbor(solSSP sol){
	solSSP s;
	s.sol = sol.sol;
	int first = 0;
	int last = 0;
	std::random_device rnd_device;
	std::mt19937 mersenne_engine {rnd_device()};
	std::uniform_int_distribution<int> dist {0, (numberJobs-1)};
	
	do {		
		first = dist(mersenne_engine);
		last = dist(mersenne_engine);		
	} while(first == last);
	
	if (first > last)std::swap(first,last);
	
	while (first < last) {
		std::swap(s.sol[first], s.sol[last]);
		first++;
		last--;
	}
	
	s.Nup = sol.Nup;
	s.Ndown = sol.Ndown;

			
	return s;
}

int SSP::lowerBound(){
	int unfinesedPriority = 0;
	int finishedJobs = 0;

	//sort the jobs vector. put the jobs with priority 1 first
	std::sort(originalJobs.begin(), originalJobs.end(), [](const Job& a, const Job& b) {
		return a.priority > b.priority;
	});

	//start accumulating the processing time
	//stop when the accumulated time is greater than the planning horizon
	int accumulatedTime = 0;
	for(int i = 0; i < numberJobs; i++){
		accumulatedTime += originalJobs[i].processingTime;
		if(accumulatedTime > planingHorizon) break;
		finishedJobs++;
	}

	//itate over the remaning jobs and get the number of priority 1 jobs that didint fit in the planning horizon
	for (int j = finishedJobs; j < numberJobs; j++) {
		if (originalJobs[j].priority == 1) {
			unfinesedPriority++;
		}
	}

	return (PROFITYFINISHED * finishedJobs) - (COSTSWITCH * 0) - (COSTSWITCHINSTANCE * 0) - (COSTPRIORITY * unfinesedPriority);
}

void SSP::destroyToolSets(){
	originalToolSets.clear();
}