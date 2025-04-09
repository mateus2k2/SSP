/*
Minimimal Working Example do Gurobi. Um modelo linear de variáveis binárias.
Exemplifica a criação de função objetivo, variáveis e restrições.

Este código é fornecido “como foi criado” e “como está disponível”, sem garantia e sem suporte ou outros serviços.

Como compilar: g++ -m64 -g -o mwe_c++ mwe_c++.cpp -I /Library/gurobi752/mac64/include/ -L /Library/gurobi752/mac64/lib/ -lgurobi_c++ -lgurobi75 -lm

Personalize o caminho da instação e a versão do Gurobi de acordo com a sua configuração

Verifique o arquivo testModel.lp, gerado após a primeira execução, para conferir o modelo gerado.

Marco Antonio Moreira de Carvalho
Departamento de Computação
Universidade Federal de Ouro Preto
Ouro Preto, Minas Gerais, Brasil
*/

#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <map>
#include <string>
#include <tuple>
#include <vector>

#include "gurobi_c++.h"
#include "headers/SSP.h"

using namespace std;

int SSP::modelo(string folderOutput) {
    // -------------------------------------------------
    // SETS
    // -------------------------------------------------

    vector<int> J = {0, 1, 2};  // 3 jobs
    map<int, vector<int>> K;
    K[0] = {0, 1};
    K[1] = {0, 1};
    K[2] = {0, 1};
    vector<int> M = {0, 1};     // 2 machines
    vector<int> T = {0, 1, 2};  // 3 tools
    int H = 100;                // Total time available

    vector<int> jobs, ops, machines, tools;

    // Tenta construir e resolver o modelo, capturando eventuais exceções
    try {
        GRBEnv env = GRBEnv();
        GRBModel model = GRBModel(env);

        // -------------------------------------------------
        // VARS
        // -------------------------------------------------

        // Corrigir estrutura de laço para definir variáveis
        map<pair<int, int>, GRBVar> sjk;    // Starting time of operation (j,k)
        map<pair<int, int>, GRBVar> ejk;    // Ending time of operation (j,k)
        map<pair<int, int>, GRBVar> alpha;  // Completed within H
        map<pair<int, int>, GRBVar> l;      // Tool switching

        for (int j : J) {
            for (int k : K[j]) {
                sjk[{j, k}] = model.addVar(0, GRB_INFINITY, 0, GRB_CONTINUOUS, "s_" + to_string(j) + "_" + to_string(k));
                ejk[{j, k}] = model.addVar(0, GRB_INFINITY, 0, GRB_CONTINUOUS, "e_" + to_string(j) + "_" + to_string(k));
                alpha[{j, k}] = model.addVar(0, 1, 0, GRB_BINARY, "alpha_" + to_string(j) + "_" + to_string(k));
                l[{j, k}] = model.addVar(0, 1, 0, GRB_BINARY, "l_" + to_string(j) + "_" + to_string(k));
            }
        }

        // Variável x: (m, j, k, j', k') binária
        map<tuple<int, int, int, int, int>, GRBVar> x;
        for (int m : M) {
            for (int j : J) {
                for (int k : K[j]) {
                    for (int jp : J) {
                        for (int kp : K[jp]) {
                            x[{m, j, k, jp, kp}] = model.addVar(0, 1, 0, GRB_BINARY, "x_" + to_string(m) + "_" + to_string(j) + "_" + to_string(k) + "_" + to_string(jp) + "_" + to_string(kp));
                        }
                    }
                }
            }
        }

        // Variável beta: (m, j, k) binária
        map<tuple<int, int, int>, GRBVar> beta;
        for (int m : M) {
            for (int j : J) {
                for (int k : K[j]) {
                    beta[{m, j, k}] = model.addVar(0, 1, 0, GRB_BINARY, "beta_" + to_string(m) + "_" + to_string(j) + "_" + to_string(k));
                }
            }
        }

        // Variáveis y e z: (t, j, k)
        map<tuple<int, int, int>, GRBVar> y;
        map<tuple<int, int, int>, GRBVar> z;

        for (int t : T) {
            for (int j : J) {
                for (int k : K[j]) {
                    y[{t, j, k}] = model.addVar(0, 1, 0, GRB_BINARY, "y_" + to_string(t) + "_" + to_string(j) + "_" + to_string(k));
                    z[{t, j, k}] = model.addVar(0, 1, 0, GRB_BINARY, "z_" + to_string(t) + "_" + to_string(j) + "_" + to_string(k));
                }
            }
        }

        // -------------------------------------------------
        // RESTRICTIONS
        // -------------------------------------------------

        // -------------------------------------------------
        // OBJs
        // -------------------------------------------------

    } catch (GRBException e) {
        cout << "Erro numero: " << e.getErrorCode() << endl;
        cout << e.getMessage() << endl;
    } catch (...) {
        cout << "Erro durante a construcao ou solucao do modelo" << endl;
    }
}