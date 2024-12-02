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

void SSP::setParans(int capacityMagazine1, int numberMachines1, int planingHorizon1, int unsupervised1){
	this->capacityMagazine = capacityMagazine1;
	this->numberMachines = numberMachines1;
	this->planingHorizon = planingHorizon1;
	this->unsupervised = unsupervised1;
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