#include "headers/SSP.h"

void SSP::setupPermutations() {
    for (int i = 0; i < numberJobs; i++) {
        for (int j = i + 1; j < numberJobs; j++) {
            switchPermutation.push_back(std::make_tuple(i, j));
        }
    }
    numberOfPairs = switchPermutation.size();
}

solSSP SSP::ajustFinalSolution(solSSP sol) {
    return sol;
}