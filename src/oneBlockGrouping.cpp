#include <algorithm>
#include <atomic>
#include <bitset>
#include <fstream>
#include <iostream>
#include <numeric>
#include <sstream>
#include <string>
#include <thread>
#include <vector>

#include "headers/GlobalVars.h"
#include "headers/SSP.h"

#ifdef FMT
#include <fmt/core.h>
#include <fmt/ranges.h>
#endif

using namespace std;

#define MAXBIT 1000
vector<bitset<MAXBIT>> bitMatrix;
unsigned n;
unsigned m;
unsigned t;
std::vector<std::vector<int>> matrixFerramentas;

void SSP::oneBlockGrouping(solSSP& s) {
    solSSP sol = expandSolution(s);
    vector<vector<int>> machines = splitSolutionIntoMachines(sol.sol, numberMachines);

    // Inicializar vars
    m = numberMachines;
    t = numberTools;
    n = sol.sol.size();

    // iterate over all the jobs in sol and fill matrixFerramentas
    matrixFerramentas.resize(t, vector<int>(n, 0));
    for (unsigned i = 0; i < n; ++i) {
        for (int tool : originalJobs[sol.sol[i]].toolSetNormalized.tools) {
            matrixFerramentas[tool][i] = 1;
        }
    }

    // initialize bitMatrix
    bitMatrix.resize(n + 5);
    for (unsigned i = 0; i < n + 2; ++i) { // <= CORRETO
        bitMatrix[i].reset();
    }

    ONB(machines);

}

// ------------------------------------------------------------------------------------------------
// UTEIS
// ------------------------------------------------------------------------------------------------

int deltaBitwise(int a, int b, int c) { return ((~bitMatrix[a] & bitMatrix[b] & ~bitMatrix[c]) | (bitMatrix[a] & ~bitMatrix[b] & bitMatrix[c])).count(); }

int deltaShift(int i, int j) { return -deltaBitwise(i - 1, i, i + 1) + deltaBitwise(j - 1, i, j); }

// ------------------------------------------------------------------------------------------------
// BUSCAS
// ------------------------------------------------------------------------------------------------

double SSP::ONB(vector<vector<int>> maquinas) {

    std::vector<std::pair<double, int>>::iterator critica;
    std::pair<int, int> ONB1, ONB2;
    int deltad, deltae;
    bool melhorou = true;
    double cTimeCritica = 0;

    for (vector<int> maquina : maquinas) {
        melhorou = true;
        while (melhorou) {
            melhorou = false;

            auto [fineshedJobsCount, switchs, switchsInstances, fineshedPriorityCount, _] = KTNS(maquina, 0);
            cTimeCritica = -((PROFITYFINISHED * fineshedJobsCount) - (COSTSWITCH * switchs) - (COSTSWITCHINSTANCE * switchsInstances) - (COSTPRIORITY * fineshedPriorityCount));
            std::vector<int> mCritica = maquina;
            std::vector<int> mAux1, mAux2;
            for (unsigned i = 0; i <= n + 2; ++i) bitMatrix[i].reset();

            for (unsigned j = 0; j < t; ++j) {
                for (unsigned i = 0; i < mCritica.size(); ++i) {
                    if (matrixFerramentas[j][mCritica[i]] == 1) {
                        bitMatrix[i + 1].set(t - 1 - j);
                    }
                }
            }

            std::vector<int> linhas;
            for (unsigned int i = 0; i < t; ++i) linhas.push_back(i);
            unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
            shuffle(linhas.begin(), linhas.end(), std::default_random_engine(seed));

            for (vector<int>::const_iterator i = linhas.begin(); i != linhas.end(); ++i) {
                ONB1 = std::make_pair(-1, -1);
                ONB2 = std::make_pair(-1, -1);
                for (unsigned j = 0; j < mCritica.size(); ++j) {
                    if (matrixFerramentas[*i][mCritica[j]] == 1) {
                        if (ONB1.first == -1) {
                            ONB1.first = j;
                            while (j < mCritica.size() && matrixFerramentas[*i][mCritica[j]] == 1) ++j;
                            ONB1.second = j - 1;
                        } else {
                            if (ONB2.first == -1) {
                                ONB2.first = j;
                                while (j < mCritica.size() && matrixFerramentas[*i][mCritica[j]] == 1) ++j;
                                ONB2.second = j - 1;
                            }
                        }
                        if (ONB2.first != -1) {
                            int nMovimentos = ONB1.first - ONB1.second + 1;
                            int pivo = ONB1.first;
                            int TPivo = 0;
                            mAux1 = mCritica;
                            mAux2 = mCritica;
                            double c1, c2;
                            for (int p = 0; p < nMovimentos; ++p) {
                                // delta avalicao indice la começa em 1
                                deltae = deltaShift(pivo + 1, ONB2.first + 1);
                                deltad = deltaShift(pivo + 1, ONB2.second + 2);
                                if (deltae <= 0 || deltad <= 0) {
                                    // Insiro a esquerda do 2 ONB
                                    TPivo = mAux1[pivo];
                                    for (int pe = pivo; pe < ONB2.first - 1; ++pe) mAux1[pe] = mAux1[pe + 1];
                                    mAux1[ONB2.first - 1] = TPivo;
                                    // c1 = completionTime(tProcessamento, mAux1, maquinaAtual->second - 1);
                                    auto [fineshedJobsCount2, switch2, switchsInstance2, fineshedPriorityCount2, _2] = KTNS(mAux1, 0);
                                    c1 = -((PROFITYFINISHED * fineshedJobsCount2) - (COSTSWITCH * switch2) - (COSTSWITCHINSTANCE * switchsInstance2) - (COSTPRIORITY * fineshedPriorityCount2));

                                    // std::cout << "\nDelta Esquerda " << deltae << "\n";
                                    //  Insiro a direita do 2 ONB
                                    TPivo = mAux2[pivo];
                                    for (int pd = pivo; pd < ONB2.second; ++pd) mAux2[pd] = mAux2[pd + 1];
                                    mAux2[ONB2.second] = TPivo;
                                    // c2 = completionTime(tProcessamento, mAux2, maquinaAtual->second - 1);
                                    auto [fineshedJobsCount3, switchs3, switchsInstances3, fineshedPriorityCount3, _3] = KTNS(mAux2, 0);
                                    c2 = -((PROFITYFINISHED * fineshedJobsCount3) - (COSTSWITCH * switchs3) - (COSTSWITCHINSTANCE * switchsInstances3) - (COSTPRIORITY * fineshedPriorityCount3));

                                    // std::cout << "\nDelta Direita " << deltad << "\n";
                                    if (c1 < cTimeCritica || c2 < cTimeCritica) {
                                        cout << "Melhorou: " << c1 << " " << c2 << " " << cTimeCritica << endl;
                                        melhorou = true;
                                        if (c1 < c2) {
                                            // Fica à esquerda
                                            //  std::cout << "\nFicou a esquerda\n";
                                            mCritica = mAux1;
                                            ONB2.first = ONB2.first - 1;
                                            cTimeCritica = c1;
                                        } else {
                                            // Fica à direita
                                            //  std::cout << "\nFicou a direita\n";
                                            mCritica = mAux2;
                                            cTimeCritica = c2;
                                        }
                                    }
                                }  // fim da delta avaliacao <= 0
                                if (!melhorou) ++pivo;  // se tiver melhorado, a tarefa andou, então o pivo fica.
                                mAux1 = mCritica;
                                mAux2 = mCritica;
                            }  // Fim dos movimentos ONB1->ONB2
                            // Procura-se o proximo ONB
                            ONB1.first = ONB2.first;
                            ONB1.second = ONB2.second;
                            ONB2 = make_pair(-1, -1);
                        }
                    }
                }
            }  // fim das linhas
            if (melhorou) {
                maquina = mCritica;
            }

        }  // wend
    }
    solSSP sol;
    for (const auto& maquina : maquinas) {
        sol.sol.insert(sol.sol.end(), maquina.begin(), maquina.end());
    }
    cout << "ONB: " << evaluate(sol) << endl;
    return evaluate(sol);
}

