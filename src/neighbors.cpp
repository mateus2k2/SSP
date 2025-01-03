#include "headers/SSP.h"

#ifdef DEBUG
#include <fmt/core.h>
#include <fmt/ranges.h>
#endif

solSSP SSP::neighbor(solSSP sol) { return neighborFunc(sol); }

// 2-opt
solSSP SSP::two_opt(solSSP sol) {
    solSSP s;
    s.sol = sol.sol;
    int first = 0;
    int last = 0;
    std::random_device rnd_device;
    std::mt19937 mersenne_engine{rnd_device()};
    std::uniform_int_distribution<int> dist{0, (numberJobs - 1)};

    do {
        first = dist(mersenne_engine);
        last = dist(mersenne_engine);
    } while (first == last);

    if (first > last) std::swap(first, last);

    while (first < last) {
        std::swap(s.sol[first], s.sol[last]);
        first++;
        last--;
    }

    s.Nup = sol.Nup;
    s.Ndown = sol.Ndown;

    return s;
}

// 2-swap
solSSP SSP::two_swap(solSSP sol) {
    solSSP s;
    s.sol = sol.sol;
    int first = 0;
    int last = 0;
    bool validSwap = false;
    std::random_device rnd_device;
    std::mt19937 mersenne_engine{42}; // rnd_device()
    std::uniform_int_distribution<int> dist {0, (numberJobs-1)};

    if(diferent_toolset_mode == 1) {    
        std::shuffle(begin(switchPermutation), end(switchPermutation), mersenne_engine);
        sol.currantPermutationIndex = 0;

        do {
            if (sol.currantPermutationIndex == switchPermutation.size()) {
                s.Nup = sol.Nup;
                s.Ndown = sol.Ndown;
                return s;
            }

            first = std::get<0>(switchPermutation[sol.currantPermutationIndex]);
            last = std::get<1>(switchPermutation[sol.currantPermutationIndex]);
            sol.currantPermutationIndex++;

            if (sol.releaseDates[originalJobs[sol.sol[first]].indexJob] < last && 
                sol.dueDates[originalJobs[sol.sol[first]].indexJob] > last &&
                sol.releaseDates[originalJobs[sol.sol[last]].indexJob] < first &&
                sol.dueDates[originalJobs[sol.sol[last]].indexJob] > first) {
                validSwap = true;
            }
        } while (validSwap == false);
    }
    else{
        do {
            first = dist(mersenne_engine);
            last = dist(mersenne_engine);
        } while (first == last);
    }

    std::swap(s.sol[first], s.sol[last]);
    s.Nup = sol.Nup;
    s.Ndown = sol.Ndown;
    
    return s;
}

// Inserção
solSSP SSP::insertion(solSSP sol) {
    solSSP s;
    s.sol = sol.sol;
    int from = 0;
    int to = 0;
    std::random_device rnd_device;
    std::mt19937 mersenne_engine{rnd_device()};
    std::uniform_int_distribution<int> dist{0, (numberJobs - 1)};

    // Seleciona duas posições distintas
    do {
        from = dist(mersenne_engine);
        to = dist(mersenne_engine);
    } while (from == to);

    // Remove o elemento da posição `from`
    auto element = s.sol[from];
    s.sol.erase(s.sol.begin() + from);

    // Insere o elemento na posição `to`
    if (to > from) {
        to--;  // Ajusta a posição devido à remoção
    }
    s.sol.insert(s.sol.begin() + to, element);

    // Copia os dados auxiliares
    s.Nup = sol.Nup;
    s.Ndown = sol.Ndown;

    return s;
}
