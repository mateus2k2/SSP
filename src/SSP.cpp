#include "headers/SSP.h"

#ifndef IGNORE_FMT
#include <fmt/ranges.h>
#include <fmt/core.h>
#endif

SSP::SSP(std::string filenameJobs, std::string filenameTools){
    laodToolSet(filenameTools);
    laodInstance(filenameJobs);
}

SSP::~SSP(){	
}

void SSP::setParans(int capacityMagazine1, int numberMachines1, int planingHorizon1, int unsupervised1, int movementType1, int initSolType1){
	this->capacityMagazine = capacityMagazine1;
	this->numberMachines = numberMachines1;
	this->planingHorizon = planingHorizon1;
	this->unsupervised = unsupervised1;

	// two_opt = 0
	// two_swap = 1
	// insertion = 2
	if (movementType1 == 0) this->neighborFunc = std::bind(&SSP::two_opt, this, std::placeholders::_1);
	if (movementType1 == 1) this->neighborFunc = std::bind(&SSP::two_swap, this, std::placeholders::_1);
	if (movementType1 == 2) this->neighborFunc = std::bind(&SSP::insertion, this, std::placeholders::_1);

	// rand = 0
	// randPriority = 1
	if (initSolType1 == 0) this->constructionFunc = std::bind(&SSP::rand, this);
	if (initSolType1 == 1) this->constructionFunc = std::bind(&SSP::randPriority, this);
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
		if(accumulatedTime > planingHorizon*DAY) break;
		finishedJobs++;
	}

	//itate over the remaning jobs and get the number of priority 1 jobs that didint fit in the planning horizon
	for (int j = finishedJobs; j < numberJobs; j++) {
		if (originalJobs[j].priority) {
			unfinesedPriority++;
		}
	}

	return (PROFITYFINISHED * finishedJobs) - (COSTSWITCH * 0) - (COSTSWITCHINSTANCE * 0) - (COSTPRIORITY * unfinesedPriority);

}

void SSP::destroyToolSets(){
	originalToolSets.clear();
}