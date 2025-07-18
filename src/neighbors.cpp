#include "headers/SSP.h"

#ifdef FMT
#include <fmt/core.h>
#include <fmt/ranges.h>
#endif

solSSP SSP::neighbor(solSSP sol) { return neighborFunc(sol); }

solSSP SSP::total_rand(solSSP sol) {
    solSSP ss;
    std::random_device rnd_device;
    #if defined(RANDSEED)
        std::mt19937 mersenne_engine{rnd_device()};
    #else
        std::mt19937 mersenne_engine{42};
    #endif

    for (int i = 0; i < numberJobs; i++) {
        ss.sol.push_back(i);
    }

    std::shuffle(begin(ss.sol), end(ss.sol), mersenne_engine);

    ss.evalSol = evaluate(ss);
    ss.Nup = false;
    ss.Ndown = false;

    return ss;
}

// 2-opt
solSSP SSP::two_opt(solSSP sol) {
    solSSP s;
    s.sol = sol.sol;
    int first = 0;
    int last = 0;
    std::random_device rnd_device;
    #if defined(RANDSEED)
        std::mt19937 mersenne_engine{rnd_device()};
    #else
        std::mt19937 mersenne_engine{42};
    #endif
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
    s.releaseDates = sol.releaseDates;
    s.dueDates = sol.dueDates;
    int first = 0;
    int last = 0;
    std::random_device rnd_device;
    #if defined(RANDSEED)
        std::mt19937 mersenne_engine{rnd_device()};
    #else
        std::mt19937 mersenne_engine{42};
    #endif
    std::uniform_int_distribution<int> dist {0, (numberJobs-1)};

    do {
        first = dist(mersenne_engine);
        last = dist(mersenne_engine);
    } while (first == last);
    

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
    #if defined(RANDSEED)
        std::mt19937 mersenne_engine{rnd_device()};
    #else
        std::mt19937 mersenne_engine{42};
    #endif
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
