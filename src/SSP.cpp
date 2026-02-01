#include "headers/SSP.h"

#ifdef FMT
#include <fmt/ranges.h>
#include <fmt/core.h>
#endif

SSP::SSP(std::string filenameJobs, std::string filenameTools, int diferent_toolset_mode_, int COSTSWITCH_, int COSTSWITCHINSTANCE_, int COSTPRIORITY_, int PROFITYFINISHED_){
	if (filenameJobs.find("Beezao") != string::npos) {
        loadInstanceBeezao(filenameJobs);
    }
	else{
		laodToolSet(filenameTools);
		laodInstance(filenameJobs);
		loadInstanceParans(filenameJobs);
	}

	this->diferent_toolset_mode = diferent_toolset_mode_;
	this->inputJobsFile = filenameJobs;
	this->inputToolsetsFile = filenameTools;

	this->COSTSWITCH = COSTSWITCH_;
	this->COSTSWITCHINSTANCE = COSTSWITCHINSTANCE_;
	this->COSTPRIORITY = COSTPRIORITY_;
	this->PROFITYFINISHED = PROFITYFINISHED_;
}

SSP::~SSP(){	
}

void SSP::setParans(int movementType1, int initSolType1){
	// two_opt = 0
	// two_swap = 1
	// insertion = 2
	if (movementType1 == 0) this->neighborFunc = std::bind(&SSP::two_opt, this, std::placeholders::_1);
	if (movementType1 == 1) this->neighborFunc = std::bind(&SSP::two_swap, this, std::placeholders::_1);
	if (movementType1 == 2) this->neighborFunc = std::bind(&SSP::insertion, this, std::placeholders::_1);
	if (movementType1 == 3) this->neighborFunc = std::bind(&SSP::total_rand, this, std::placeholders::_1);

	// rand = 0
	// randPriority = 1
	if (initSolType1 == 0) this->constructionFunc = std::bind(&SSP::randPriority, this);
	if (initSolType1 == 1) this->constructionFunc = std::bind(&SSP::rand, this);
}
