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

#define MAXBIT 200
vector<bitset<MAXBIT>> bitMatrix;
unsigned n;  // tarefas
unsigned m;   // number of machines
std::vector<unsigned> tempoTroca;
vector<unsigned> capacidade;  // capacidade do magazine
std::vector<int> ferramentasJob;
std::vector<std::vector<int>> matrixFerramentas;
unsigned t;  // ferramentas

void SSP::oneBlockGrouping(solSSP& s) {
    // matrix
    // linha = toolsets
    // coluna = jobs
}

// ------------------------------------------------------------------------------------------------
// UTEIS
// ------------------------------------------------------------------------------------------------

int deltaBitwise(int a, int b, int c) { return ((~bitMatrix[a] & bitMatrix[b] & ~bitMatrix[c]) | (bitMatrix[a] & ~bitMatrix[b] & bitMatrix[c])).count(); }

int deltaShift(int i, int j) { return -deltaBitwise(i - 1, i, i + 1) + deltaBitwise(j - 1, i, j); }

long KTNS(const vector<int> processos, int maquina, bool debug = false) { return 0; }

double completionTime(std::vector<std::vector<unsigned>> tProcessamento, std::vector<int>& tarefas, int maquina) {
    
    double tPr = 0;
    for (std::vector<int>::const_iterator i = tarefas.begin(); i != tarefas.end(); ++i) {
        if ((int)ferramentasJob[*i] > (int)capacidade[maquina]) return 1000000;  // inválido
        tPr += tProcessamento[maquina][*i];
    }
    long nTrocas = KTNS(tarefas, maquina);
    double tTrocas = nTrocas * tempoTroca[maquina];
    return (tPr + tTrocas);
}

double sumCompletionTime(std::vector<std::vector<int>>& maquinas, std::vector<std::vector<unsigned>> tProcessamento) {
    double tot = 0;
    int maq = 0;
    std::vector<int> maquinaAux;
    for (std::vector<std::vector<int>>::const_iterator i = maquinas.begin(); i != maquinas.end(); ++i) {
        maquinaAux = *i;
        tot += completionTime(tProcessamento, maquinaAux, maq);
        maq++;
    }
    return tot;
}

// ------------------------------------------------------------------------------------------------
// BUSCAS
// ------------------------------------------------------------------------------------------------

void ONB_noCritical(std::vector<std::vector<int>>& maquinas, std::vector<std::pair<double, int>>& idx_maquinas, std::vector<std::vector<unsigned>> tProcessamento, double& makespan) {
    // maquinas - Todas as Máquinas
    // idx_maquinas - completionTime, idMaquina
    // tProcessamento - tempo de processamento das tarefas (carregado na decodificacao)
    extern std::vector<std::vector<int>> matrixFerramentas;
    extern unsigned t;  // ferramentas
    extern unsigned n;  // tarefas
    std::vector<std::pair<double, int>>::iterator critica;
    std::pair<int, int> ONB1, ONB2;
    int deltad, deltae;
    if (idx_maquinas.size() < m) return;
    bool melhorou = true;
    double cTimeCritica = 0;
    std::sort(idx_maquinas.rbegin(), idx_maquinas.rend());
    for (std::vector<std::pair<double, int>>::iterator maquinaAtual = idx_maquinas.begin() + 1; maquinaAtual != idx_maquinas.end(); ++maquinaAtual) {
        melhorou = true;
        while (melhorou) {
            melhorou = false;

            cTimeCritica = maquinaAtual->first;
            std::vector<int> mCritica = maquinas.at(maquinaAtual->second - 1);
            std::vector<int> mAux1, mAux2;
            if (KTNS(mCritica, maquinaAtual->second - 1) == 0) continue;
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
                                    c1 = completionTime(tProcessamento, mAux1, maquinaAtual->second - 1);

                                    // std::cout << "\nDelta Esquerda " << deltae << "\n";
                                    //  Insiro a direita do 2 ONB
                                    TPivo = mAux2[pivo];
                                    for (int pd = pivo; pd < ONB2.second; ++pd) mAux2[pd] = mAux2[pd + 1];
                                    mAux2[ONB2.second] = TPivo;
                                    c2 = completionTime(tProcessamento, mAux2, maquinaAtual->second - 1);

                                    // std::cout << "\nDelta Direita " << deltad << "\n";
                                    if (c1 < cTimeCritica || c2 < cTimeCritica) {
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
                maquinaAtual->first = cTimeCritica;
                maquinas.at(maquinaAtual->second - 1) = mCritica;
            }

        }  // wend
    }
    makespan = sumCompletionTime(maquinas, tProcessamento);
}

void ONB(std::vector<std::vector<int>>& maquinas, std::vector<std::pair<double, int>>& idx_maquinas, double& makespan, std::vector<std::vector<unsigned>> tProcessamento) {
    // maquinas - Todas as Máquinas
    // idx_maquinas - completionTime, idMaquina
    // makespan
    // tProcessamento - tempo de processamento das tarefas (carregado na decodificacao)
    extern std::vector<std::vector<int>> matrixFerramentas;
    extern unsigned t;  // ferramentas
    extern unsigned n;  // tarefas
    std::vector<std::pair<double, int>>::iterator critica;
    std::pair<int, int> ONB1, ONB2;
    int deltad, deltae;
    if (idx_maquinas.size() < m) return;
    bool melhorou = true;
    double cTimeCritica = 0;
    while (melhorou) {
        melhorou = false;
        std::sort(idx_maquinas.rbegin(), idx_maquinas.rend());
        critica = idx_maquinas.begin();
        cTimeCritica = critica->first;
        std::vector<int> mCritica = maquinas.at(critica->second - 1);
        std::vector<int> mAux1, mAux2;
        if (KTNS(mCritica, critica->second - 1) == 0) return;
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
                                c1 = completionTime(tProcessamento, mAux1, critica->second - 1);

                                // std::cout << "\nDelta Esquerda " << deltae << "\n";
                                //  Insiro a direita do 2 ONB
                                TPivo = mAux2[pivo];
                                for (int pd = pivo; pd < ONB2.second; ++pd) mAux2[pd] = mAux2[pd + 1];
                                mAux2[ONB2.second] = TPivo;
                                c2 = completionTime(tProcessamento, mAux2, critica->second - 1);

                                // std::cout << "\nDelta Direita " << deltad << "\n";
                                if (c1 < cTimeCritica || c2 < cTimeCritica) {
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
            critica->first = cTimeCritica;
            maquinas.at(critica->second - 1) = mCritica;
        }

    }  // wend
    ONB_noCritical(maquinas, idx_maquinas, tProcessamento, makespan);
}
