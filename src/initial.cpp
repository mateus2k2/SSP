#include "headers/SSP.h"

#ifdef FMT
#include <fmt/core.h>
#include <fmt/ranges.h>
#endif

solSSP SSP::construction() { return constructionFunc(); }

solSSP SSP::rand() {
    solSSP ss;
    std::random_device rnd_device;
#if defined(RANDSEED)
    std::mt19937 mersenne_engine{rnd_device()};
#else
    std::mt19937 mersenne_engine{42};
#endif
    std::vector<int> solIndexes;

    for (size_t i = 0; i < groupedJobs.size(); i++) {
        solIndexes.push_back(i);
    }

    std::shuffle(begin(solIndexes), end(solIndexes), mersenne_engine);

    for (size_t i = 0; i < solIndexes.size(); ++i) {
        Job job = groupedJobs[solIndexes[i]];
        // Find the original job index
        auto indexOperation1 = mapJobsToOriginalIndex.find(std::make_tuple(job.indexJob, 0));
        auto indexOperation2 = mapJobsToOriginalIndex.find(std::make_tuple(job.indexJob, 1));
        if(indexOperation1 != mapJobsToOriginalIndex.end()) {
            ss.sol.push_back(indexOperation1->second);
        }
        if(indexOperation2 != mapJobsToOriginalIndex.end()) {
            ss.sol.push_back(indexOperation2->second);
        }
    }

    ss.evalSol = evaluate(ss);
    ss.Nup = false;
    ss.Ndown = false;

    return ss;
}

solSSP SSP::randPriority() {
    solSSP ss;
    std::random_device rnd_device;
    #if defined(RANDSEED)
        std::mt19937 mersenne_engine{rnd_device()};
    #else
        std::mt19937 mersenne_engine{42};
    #endif

    // put the priority jobs first
    std::vector<int> priorityJobIndices;
    std::vector<int> nonPriorityJobIndices;

    for (size_t i = 0; i < groupedJobs.size(); ++i) {
        if (groupedJobs[i].priority) {
            priorityJobIndices.push_back(i);
        } 
        else {
            nonPriorityJobIndices.push_back(i);
        }
    }

    std::shuffle(begin(priorityJobIndices), end(priorityJobIndices), mersenne_engine);
    std::shuffle(begin(nonPriorityJobIndices), end(nonPriorityJobIndices), mersenne_engine);

    for (size_t i = 0; i < priorityJobIndices.size(); ++i) {
        // Job job = groupedJobs[priorityJobIndices[i]];
        // // Find the original job index
        // auto indexOperation1 = mapJobsToOriginalIndex.find(std::make_tuple(job.indexJob, 0));
        // auto indexOperation2 = mapJobsToOriginalIndex.find(std::make_tuple(job.indexJob, 1));
        // if(indexOperation1 != mapJobsToOriginalIndex.end()) {
        //     ss.sol.push_back(indexOperation1->second);
        // }
        // if(indexOperation2 != mapJobsToOriginalIndex.end()) {
        //     ss.sol.push_back(indexOperation2->second);
        // }
        ss.sol.push_back(priorityJobIndices[i]);
    }

    for (size_t i = 0; i < nonPriorityJobIndices.size(); ++i) {
        // Job job = groupedJobs[nonPriorityJobIndices[i]];
        // // Find the original job index
        // auto indexOperation1 = mapJobsToOriginalIndex.find(std::make_tuple(job.indexJob, 0));
        // auto indexOperation2 = mapJobsToOriginalIndex.find(std::make_tuple(job.indexJob, 1));
        // if(indexOperation1 != mapJobsToOriginalIndex.end()) {
        //     ss.sol.push_back(indexOperation1->second);
        // }
        // if(indexOperation2 != mapJobsToOriginalIndex.end()) {
        //     ss.sol.push_back(indexOperation2->second);
        // }
        ss.sol.push_back(nonPriorityJobIndices[i]);
    }


    ss.evalSol = evaluate(ss);
    ss.Nup = false;
    ss.Ndown = false;

    return ss;
}
