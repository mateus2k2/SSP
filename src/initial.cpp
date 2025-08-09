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

    // for (size_t i = 0; i < originalJobs.size(); i++) {
    for (size_t i = 0; i < groupedJobs.size(); i++) {
        solIndexes.push_back(i);
    }

    std::shuffle(begin(solIndexes), end(solIndexes), mersenne_engine);

    for (size_t i = 0; i < solIndexes.size(); ++i) {
        ss.sol.push_back(solIndexes[i]);
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

    std::vector<int> priorityJobIndices;
    std::vector<int> nonPriorityJobIndices;

    // for (size_t i = 0; i < originalJobs.size(); ++i) {
    for (size_t i = 0; i < groupedJobs.size(); ++i) {
        if (originalJobs[i].priority) {
            priorityJobIndices.push_back(i);
        }
        else {
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


    ss.evalSol = evaluate(ss);
    ss.Nup = false;
    ss.Ndown = false;

    // cout << "Agrupados: " << groupedJobs.size() << endl;
    // cout << "Original: " << originalJobs.size() << endl;

    return ss;
}
