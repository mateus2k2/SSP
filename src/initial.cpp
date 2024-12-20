#include "headers/SSP.h"

solSSP SSP::construction(){
    return constructionFunc();
}


solSSP SSP::rand(){
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

solSSP SSP::randPriority(){
	solSSP ss;
    std::random_device rnd_device;
    std::mt19937 mersenne_engine {rnd_device()}; 
    
    // put the priority jobs first
    std::vector<int> priorityJobIndices;
    std::vector<int> nonPriorityJobIndices;

    for (size_t i = 0; i < originalJobs.size(); ++i) {
        if (originalJobs[i].priority) {
            priorityJobIndices.push_back(i);
        } else {
            nonPriorityJobIndices.push_back(i);
        }
    }

    std::shuffle(begin(priorityJobIndices), end(priorityJobIndices), mersenne_engine);
    std::shuffle(begin(nonPriorityJobIndices), end(nonPriorityJobIndices), mersenne_engine);

    for (size_t i = 0; i < priorityJobIndices.size(); ++i) {
        ss.sol.push_back(priorityJobIndices[i]);
    }

    for (size_t i = 0; i < nonPriorityJobIndices.size(); ++i) {
        ss.sol.push_back(nonPriorityJobIndices[i]);
    }

    // iterate over the solution
    for (size_t i = 0; i < ss.sol.size(); ++i) {
        int index = ss.sol[i];
        Job job = originalJobs[index];
        if(job.indexOperation == 2){
            for (size_t j = i; j < ss.sol.size(); j++){
                int index2 = ss.sol[j];
                Job job2 = originalJobs[index2];
                if((job2.indexJob == job.indexJob) && (job2.indexOperation == 1)){
                    std::swap(ss.sol[i], ss.sol[j]);
                    break;
                }
            }
        }
    }
    
	ss.evalSol = evaluate(ss);
	ss.Nup = false;
	ss.Ndown = false;
	 
	return ss;
}
