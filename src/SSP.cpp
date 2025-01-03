#include "headers/SSP.h"

#ifdef DEBUG
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

void SSP::setupPermutations() {
    for (int i = 0; i < numberJobs; i++) {
        for (int j = i + 1; j < numberJobs; j++) {
            switchPermutation.push_back(std::make_tuple(i, j));
        }
    }
    numberOfPairs = switchPermutation.size();
}