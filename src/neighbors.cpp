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
    #if defined(RANDSEED)
        std::mt19937 mersenne_engine{rnd_device()};
    #else
        std::mt19937 mersenne_engine{42};
    #endif
    std::uniform_int_distribution<int> dist{0, (numberJobs - 1)};

    if(diferent_toolset_mode == 1){ 

    }
    else{
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

    int countErros = 0;
    int validSwap1 = false;
    int validSwap2 = false;

    if(diferent_toolset_mode == 1) {
        do {
            do {
                first = dist(mersenne_engine);
                last = dist(mersenne_engine);
            } while (first == last);
            if (first > last) std::swap(first, last);
            
            int lastBKP = max(0, last - s.dueDates[originalJobs[s.sol[first]].indexJob]);
            last = lastBKP <= first ? last : lastBKP;

            int firstBKP = min(numberJobs - 1, first + s.releaseDates[originalJobs[s.sol[last]].indexJob]);
            first = firstBKP >= last ? first : firstBKP; 
            
            validSwap1 = false;
            validSwap2 = false;

            if (originalJobs[s.sol[first]].indexOperation == 0) {
                if (s.dueDates[originalJobs[s.sol[first]].indexJob] > last) {
                    validSwap1 = true;
                }
            }
            else {
                validSwap1 = true;
            }

            if (originalJobs[s.sol[last]].indexOperation == 0) {
                validSwap1 = true;
            }
            else {
                if (s.releaseDates[originalJobs[s.sol[last]].indexJob] < first) {
                    validSwap2 = true;
                }
            }
            if(countErros > 10000){
                cout << "ERRO" << endl;
                s.Nup = sol.Nup;
                s.Ndown = sol.Ndown;
                return s;
            }
            countErros++;

        } while (validSwap1 == false || validSwap2 == false);

        if (originalJobs[s.sol[first]].indexOperation == 0) {
            s.releaseDates[originalJobs[s.sol[first]].indexJob] = last;
        }
        else if (originalJobs[s.sol[first]].indexOperation == 1) {
            s.dueDates[originalJobs[s.sol[first]].indexJob] = last;
        }
        if (originalJobs[s.sol[last]].indexOperation == 0) {
            s.releaseDates[originalJobs[s.sol[last]].indexJob] = first;
        }
        else if (originalJobs[s.sol[last]].indexOperation == 1) {
            s.dueDates[originalJobs[s.sol[last]].indexJob] = first;
        }
    }
    else{
        do {
            first = dist(mersenne_engine);
            last = dist(mersenne_engine);
        } while (first == last);
    }

    cout << "countErros: " << countErros << endl;

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

    if(diferent_toolset_mode == 1){ 
        
    }
    else{
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
    }

    // Copia os dados auxiliares
    s.Nup = sol.Nup;
    s.Ndown = sol.Ndown;

    return s;
}
