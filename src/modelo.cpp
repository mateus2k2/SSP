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

// -------------------------------------------------
// DEFINIÇÃO DOS PARAMS
// -------------------------------------------------
double r;   // recompensa por operação concluída
double cp;  // custo por operação perdida (em priorityOperations)
double cf;  // custo por troca supervisionada
double cv;  // custo por ferramenta trocada
int H;      // Horizonte de agendamento
int tU;     // Duração do turno não supervisionado
int TC;     // Capacidade total de ferramentas

// -------------------------------------------------
// DEFINIÇÃO DOS SETS
// -------------------------------------------------
vector<pair<int, int>> operationsModel;          // Lista de operações (j,k)
vector<int> machinesModel;                       // Lista de máquinas m
vector<int> toolsModel;                          // Lista de ferramentas t
map<int, int> jobOperationsCount;                // job → número de operações
map<pair<int, int>, vector<int>> requiredTools;  // (j,k) → lista de ferramentas requeridas
set<pair<int, int>> priorityOperations;          // subconjunto de operações penalizadas se não concluídas
map<pair<int, int>, int> processingTimes;        // (j,k) → tempo de processamento

// -------------------------------------------------
// DEFINIÇÃO DAS VARS
// -------------------------------------------------

// Variáveis de tempo
map<pair<int, int>, GRBVar> s;  // s(jk): tempo de início
map<pair<int, int>, GRBVar> e;  // e(jk): tempo de término

// Variáveis binárias
map<tuple<int, int, int, int, int>, GRBVar> x;  // x(m)(jk,j'k')
map<tuple<int, int, int>, GRBVar> betaVar;      // β(m)(jk)
map<pair<int, int>, GRBVar> alpha;              // α(jk)
map<tuple<int, int, int>, GRBVar> y;            // y(t)(jk)
map<tuple<int, int, int>, GRBVar> z;            // z(t)(jk)
map<pair<int, int>, GRBVar> l;                  // l(jk)

// Variáveis auxiliares para obj linear
map<pair<int, int>, GRBVar> delta;         // δ(jk)
map<tuple<int, int, int>, GRBVar> lambda;  // λ(t)(jk)

// -------------------------------------------------
// FUNÇÕES
// -------------------------------------------------

void SSP::convertModelData(string folderOutput, GRBModel model) {
    fstream solutionReportFile;
    string filename = folderOutput + "/report.txt";
    solutionReportFile.open(filename, ios::out);
    if (!solutionReportFile.is_open()) {
        cerr << "Error: Could not open solution report file: " << filename << endl;
        exit(1);
    }

    cout << "\n--- Variable Values ---\n";

    // // Print s(jk)
    // cout << "\ns(jk):\n";
    // for (const auto& [jk, var] : s) {
    //     cout << "s(" << jk.first << "," << jk.second << ") = " << var.get(GRB_DoubleAttr_X) << endl;
    // }

    // // Print e(jk)
    // cout << "\ne(jk):\n";
    // for (const auto& [jk, var] : e) {
    //     cout << "e(" << jk.first << "," << jk.second << ") = " << var.get(GRB_DoubleAttr_X) << endl;
    // }

    // // Print alpha(jk)
    // cout << "\nalpha(jk):\n";
    // for (const auto& [jk, var] : alpha) {
    //     cout << "alpha(" << jk.first << "," << jk.second << ") = " << var.get(GRB_DoubleAttr_X) << endl;
    // }

    // // Print l(jk)
    // cout << "\nl(jk):\n";
    // for (const auto& [jk, var] : l) {
    //     cout << "l(" << jk.first << "," << jk.second << ") = " << var.get(GRB_DoubleAttr_X) << endl;
    // }

    // // Print delta(jk)
    // cout << "\ndelta(jk):\n";
    // for (const auto& [jk, var] : delta) {
    //     cout << "delta(" << jk.first << "," << jk.second << ") = " << var.get(GRB_DoubleAttr_X) << endl;
    // }

    // // Print betaVar(m,j,k)
    // cout << "\nbeta(m,j,k):\n";
    // for (const auto& [mjk, var] : betaVar) {
    //     cout << "betaVar(" << get<0>(mjk) << "," << get<1>(mjk) << "," << get<2>(mjk) << ") = " << var.get(GRB_DoubleAttr_X) << endl;
    // }

    // // Print x(m,j,k,j',k')
    // cout << "\nx(m,j,k,j',k'):\n";
    // for (const auto& [mjkjk, var] : x) {
    //     cout << "x(" << get<0>(mjkjk) << "," << get<1>(mjkjk) << "," << get<2>(mjkjk) << "," << get<3>(mjkjk) << "," << get<4>(mjkjk) << ") = " << var.get(GRB_DoubleAttr_X) << endl;
    // }

    // // Print y(t,j,k)
    // cout << "\ny(t,j,k):\n";
    // for (const auto& [tjk, var] : y) {
    //     cout << "y(" << get<0>(tjk) << "," << get<1>(tjk) << "," << get<2>(tjk) << ") = " << var.get(GRB_DoubleAttr_X) << endl;
    // }

    // // Print z(t,j,k)
    // cout << "\nz(t,j,k):\n";
    // for (const auto& [tjk, var] : z) {
    //     cout << "z(" << get<0>(tjk) << "," << get<1>(tjk) << "," << get<2>(tjk) << ") = " << var.get(GRB_DoubleAttr_X) << endl;
    // }

    // // Print lambda(t,j,k)
    // cout << "\nlambda(t,j,k):\n";
    // for (const auto& [tjk, var] : lambda) {
    //     cout << "lambda(" << get<0>(tjk) << "," << get<1>(tjk) << "," << get<2>(tjk) << ") = " << var.get(GRB_DoubleAttr_X) << endl;
    // }

    // make a vector of tuples of the form (j,k) for the operations
    vector<pair<int, int>> operationsSorted;
    for (const auto& [j, k] : operationsModel) {
        operationsSorted.push_back({j, k});
    }
    // sort the operations by start time
    sort(operationsSorted.begin(), operationsSorted.end(), [&](const pair<int, int>& a, const pair<int, int>& b) { return s[a].get(GRB_DoubleAttr_X) < s[b].get(GRB_DoubleAttr_X); });

    // convert
    solutionReportFile << "arquivoJobs" << ";" << "arquivoTools" << endl;
    solutionReportFile << H << ";" << tU << ";" << 1440 << endl;

    // JOB;OPERATION;INICIO;FIM;TOOLS
    // iterate of the machines
    for (int m : machinesModel) {
        solutionReportFile << "Machine: " << m - 1 << endl;
        for (pair<int, int> op : operationsSorted) {
            int j = op.first;
            int k = op.second;

            // check if the operation is assigned to the machine
            if (betaVar[{m, j, k}].get(GRB_DoubleAttr_X) < 0.5) continue;

            double start = s[{j, k}].get(GRB_DoubleAttr_X);
            double end = e[{j, k}].get(GRB_DoubleAttr_X);

            int priority = 0;
            if (priorityOperations.count({j, k})) {
                priority = 1;
            } else {
                priority = 0;
            }

            vector<int> toolsUsed;
            for (int t : toolsModel) {
                if (y[{t, j, k}].get(GRB_DoubleAttr_X) > 0.5) {
                    toolsUsed.push_back(t);
                }
            }

            solutionReportFile << j << ";" << k << ";" << start << ";" << end << ";" << priority << ";";
            for (size_t i = 0; i < toolsUsed.size(); ++i) {
                solutionReportFile << toolsUsed[i];
                solutionReportFile << ",";
            }
            solutionReportFile << endl;
        }
    }

    // END
    // fineshedJobsCount: 613
    // switchs: 2649
    // switchsInstances: 270
    // unfineshedPriorityCount: 167
    // Final Solution: 8031
    // Time: 3723799
    // PTL: 596
    // MCMC: 492
    // Best Initial: -1687
    // Mean Initial: -945
    solutionReportFile << "END" << endl;
    solutionReportFile << "Final Solution " << model.get(GRB_DoubleAttr_ObjVal) << endl;
}

void SSP::loadModelData() {
    r = 30;
    cp = 30;
    cf = 10;
    cv = 1;
    // H = 2880; // 48 horas em minutos
    // tU = 720; // 12 horas em minutos
    // TC = 8;

    // operationsModel = {{1, 1}, {1, 2}, {2, 1}, {3, 1}, {3, 2}, {4, 1}, {4, 2}, {5, 1}, {6, 1}, {7, 1}};
    // machinesModel = {1, 2};
    // toolsModel = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20};
    // jobOperationsCount = {{1, 2}, {2, 1}, {3, 2}, {4, 2}, {5, 1}, {6, 1}, {7, 1}};

    // requiredTools = {{{1, 1}, {1, 2, 3, 4, 5}}, {{1, 2}, {1, 2, 3, 4, 5}}, {{2, 1}, {12, 13, 14, 15, 16, 17, 18}}, {{3, 1}, {4, 5, 8, 9, 10, 11, 12, 13}}, {{3, 2}, {4, 5, 8, 9, 10, 11, 12, 13}}, {{4, 1}, {12, 13, 14, 15, 16, 17, 18}}, {{4, 2}, {12, 13, 14, 15, 16, 17, 18}}, {{5, 1}, {5, 6, 7}}, {{6, 1}, {15, 16, 17, 18, 19, 20}}, {{7, 1}, {1, 2, 3, 4, 5}}};
    // priorityOperations = {{1, 1}, {1, 2}, {4, 1}, {4, 2}, {7, 1}};
    // processingTimes = {{{1, 1}, 3*60}, {{1, 2}, 5*60}, {{2, 1}, 7*60}, {{3, 1}, 6*60}, {{3, 2}, 8*60}, {{4, 1}, 4*60}, {{4, 2}, 9*60}, {{5, 1}, 6*60}, {{6, 1}, 10*60}, {{7, 1}, 5*60}};

    // LOADING REAL DATA
    H = planingHorizon * 24 * 60;  // converte de dias para minutos
    tU = unsupervised;             // ja esta em minutos
    TC = capacityMagazine;

    for (auto job : originalJobs) {
        int jobIndex = job.indexJob;
        int operationIndex = job.indexOperation;
        operationsModel.push_back({jobIndex, operationIndex});
        jobOperationsCount[jobIndex] = job.indexOperation;
        processingTimes[{jobIndex, operationIndex}] = job.processingTime;
        if (job.priority) priorityOperations.insert({jobIndex, operationIndex});

        for (int i = 0; i <= job.indexOperation; i++) {
            std::vector<int> incrementedTools;
            for (int tool : job.toolSetNormalized.tools) {
                incrementedTools.push_back(tool + 1);
            }
            requiredTools[{jobIndex, i}] = incrementedTools;
        }
    }

    for (int i = 0; i < numberMachines; i++) {
        machinesModel.push_back(i + 1);
    }

    for (int i = 0; i < numberTools; i++) {
        toolsModel.push_back(i + 1);
    }

    // print all variables
    cout << "H: " << H << endl;
    cout << "tU: " << tU << endl;
    cout << "TC: " << TC << endl;

    cout << "operationsModel: ";
    for (auto op : operationsModel) {
        cout << "(" << op.first << "," << op.second << ") ";
    }
    cout << endl;

    cout << "machinesModel: ";
    for (auto m : machinesModel) {
        cout << m << " ";
    }
    cout << endl;

    cout << "toolsModel: ";
    for (auto t : toolsModel) {
        cout << t << " ";
    }
    cout << endl;

    cout << "jobOperationsCount: ";
    for (auto [j, k] : jobOperationsCount) {
        cout << "(" << j << "," << k << ") ";
    }
    cout << endl;

    cout << "requiredTools: " << endl;
    for (auto [jk, tools] : requiredTools) {
        cout << "(" << jk.first << "," << jk.second << ") -> ";
        for (int t : tools) {
            cout << t << " ";
        }
        cout << endl;
    }

    cout << "priorityOperations: ";
    for (auto [j, k] : priorityOperations) {
        cout << "(" << j << "," << k << ") ";
    }
    cout << endl;

    exit(0);
}

int SSP::modelo(string folderOutput) {
    loadModelData();

    try {
        GRBEnv env = GRBEnv();
        GRBModel model = GRBModel(env);

        // -------------------------------------------------
        // CRIAÇÃO DAS VARS
        // -------------------------------------------------

        for (auto [j, k] : operationsModel) {
            s[{j, k}] = model.addVar(0.0, GRB_INFINITY, 0.0, GRB_CONTINUOUS, "s_" + to_string(j) + "_" + to_string(k));
            e[{j, k}] = model.addVar(0.0, GRB_INFINITY, 0.0, GRB_CONTINUOUS, "e_" + to_string(j) + "_" + to_string(k));

            alpha[{j, k}] = model.addVar(0.0, 1.0, 0.0, GRB_BINARY, "alpha_" + to_string(j) + "_" + to_string(k));
            l[{j, k}] = model.addVar(0.0, 1.0, 0.0, GRB_BINARY, "l_" + to_string(j) + "_" + to_string(k));

            delta[{j, k}] = model.addVar(0.0, 1.0, 0.0, GRB_BINARY, "delta_" + to_string(j) + "_" + to_string(k));

            for (int m : machinesModel) {
                betaVar[{m, j, k}] = model.addVar(0.0, 1.0, 0.0, GRB_BINARY, "beta_" + to_string(m) + "_" + to_string(j) + "_" + to_string(k));
                for (auto [j2, k2] : operationsModel) {
                    if (j != j2 || k != k2) x[{m, j, k, j2, k2}] = model.addVar(0.0, 1.0, 0.0, GRB_BINARY, "x_" + to_string(m) + "_" + to_string(j) + "_" + to_string(k) + "_" + to_string(j2) + "_" + to_string(k2));
                }
            }

            for (int t : toolsModel) {
                y[{t, j, k}] = model.addVar(0.0, 1.0, 0.0, GRB_BINARY, "y_" + to_string(t) + "_" + to_string(j) + "_" + to_string(k));
                z[{t, j, k}] = model.addVar(0.0, 1.0, 0.0, GRB_BINARY, "z_" + to_string(t) + "_" + to_string(j) + "_" + to_string(k));
                lambda[{t, j, k}] = model.addVar(0.0, 1.0, 0.0, GRB_BINARY, "lambda_" + to_string(t) + "_" + to_string(j) + "_" + to_string(k));
            }
        }

        // -------------------------------------------------
        // RESTRICTIONS
        // -------------------------------------------------

        int L = 0;
        for (auto [j, k] : operationsModel) {
            L += processingTimes[{j, k}];
        }
        L += ceil((double)H * tU / 24.0);

        // (2) Cada operação pode ser seguida por no máximo uma outra operação
        for (auto [j, k] : operationsModel) {
            GRBLinExpr expr = 0;
            for (int m : machinesModel) {
                for (auto [j2, k2] : operationsModel) {
                    if (j != j2 || k != k2) {
                        auto key = make_tuple(m, j, k, j2, k2);
                        if (x.count(key)) {
                            expr += x[key];
                        }
                    }
                }
            }
            model.addConstr(expr <= 1, "follow_once_" + to_string(j) + "_" + to_string(k));
        }

        // (3) Restrição: Cada operação pode ser precedida por no máximo uma outra operação
        for (auto [j2, k2] : operationsModel) {
            GRBLinExpr expr = 0;
            for (int m : machinesModel) {
                for (auto [j, k] : operationsModel) {
                    if (j != j2 || k != k2) {
                        auto key = make_tuple(m, j, k, j2, k2);
                        if (x.count(key)) {
                            expr += x[key];
                        }
                    }
                }
            }
            model.addConstr(expr <= 1, "precede_once_" + to_string(j2) + "_" + to_string(k2));
        }

        // (4) Restrição: cada operação (j,k) deve ser atribuída a exatamente uma máquina
        for (auto [j, k] : operationsModel) {
            GRBLinExpr expr = 0;
            for (int m : machinesModel) {
                expr += betaVar[{m, j, k}];
            }
            model.addConstr(expr == 1, "assign_once_" + to_string(j) + "_" + to_string(k));
        }

        // (5) Restrição: se (j,k) é seguido por (j',k') na máquina m, então ambos estão atribuídos à mesma máquina
        for (int m : machinesModel) {
            for (auto [j, k] : operationsModel) {
                for (auto [j2, k2] : operationsModel) {
                    if (j != j2 || k != k2) {
                        auto key_x = make_tuple(m, j, k, j2, k2);
                        if (x.count(key_x)) {
                            GRBLinExpr expr = 2 * x[key_x];
                            expr -= betaVar[{m, j, k}];
                            expr -= betaVar[{m, j2, k2}];
                            model.addConstr(expr <= 0, "same_machine_" + to_string(m) + "_" + to_string(j) + "_" + to_string(k) + "_" + to_string(j2) + "_" + to_string(k2));
                        }
                    }
                }
            }
        }

        // (6) Restrição: sequência válida de operações atribuídas à mesma máquina
        for (int m : machinesModel) {
            GRBLinExpr lhs = 0;
            GRBLinExpr rhs = 0;

            for (auto [j, k] : operationsModel) {
                rhs += betaVar[{m, j, k}];
                for (auto [j2, k2] : operationsModel) {
                    if (j != j2 || k != k2) {
                        auto key_x = make_tuple(m, j, k, j2, k2);
                        if (x.count(key_x)) {
                            lhs += x[key_x];
                        }
                    }
                }
            }

            model.addConstr(lhs + 1 >= rhs, "link_beta_x_" + to_string(m));
        }

        // (7) Restrição: e(jk) = s(jk) + processingTimes(jk)  para cada operação
        for (auto [j, k] : operationsModel) {
            model.addConstr(e[{j, k}] == s[{j, k}] + processingTimes[{j, k}], "end_equals_start_plus_p_" + to_string(j) + "_" + to_string(k));
        }

        // (8) Restrição: s(jk) + L(1 - x(m)(jk,j'k')) >= e(j'k')  para garantir sequência na mesma máquina
        for (int m : machinesModel) {
            for (auto [j, k] : operationsModel) {
                for (auto [j2, k2] : operationsModel) {
                    if (j != j2 || k != k2) {
                        auto key_x = make_tuple(m, j, k, j2, k2);
                        if (x.count(key_x)) {
                            GRBLinExpr lhs = s[{j, k}] + L * (1 - x[key_x]);
                            model.addConstr(lhs >= e[{j2, k2}], "precedence_on_machine_" + to_string(m) + "_" + to_string(j) + "_" + to_string(k) + "_" + to_string(j2) + "_" + to_string(k2));
                        }
                    }
                }
            }
        }

        // (9) Restrição de precedência entre operações do mesmo job
        for (const auto& [job, numOps] : jobOperationsCount) {
            if (numOps <= 1) continue;

            for (int k = 1; k < numOps; ++k) {  // operações vão de 1 até numOps
                pair<int, int> op_curr = {job, k};
                pair<int, int> op_next = {job, k + 1};

                if (e.count(op_curr) && s.count(op_next)) {
                    model.addConstr(e[op_curr] <= s[op_next], "precedence_job_" + to_string(job) + "_op_" + to_string(k));
                }
            }
        }

        // (10) Restrição para definir se uma operação foi concluída dentro do horizonte H
        const double eps = 1e-5;
        for (auto [j, k] : operationsModel) {
            model.addConstr(alpha[{j, k}] >= (H - e[{j, k}]) / (double)L + eps, "alpha_lb_" + to_string(j) + "_" + to_string(k));
            model.addConstr(alpha[{j, k}] <= 1 - (e[{j, k}] - H) / (double)L, "alpha_ub_" + to_string(j) + "_" + to_string(k));
        }

        // (11) Restrição: soma das ferramentas presentes no início da operação não pode ultrapassar T_C
        for (auto [j, k] : operationsModel) {
            GRBLinExpr expr = 0;
            for (int t : toolsModel) {
                expr += y[{t, j, k}];
            }
            model.addConstr(expr <= TC, "tool_capacity_" + to_string(j) + "_" + to_string(k));
        }

        // (12) states that a tool switch occurs (i.e., 𝑧(𝑡)(𝑗𝑘) = 1) when tool 𝑡 required for operation (𝑗, 𝑘) is not present in the tool magazine during the processing of preceding operation (𝑗′, 𝑘′).
        for (int m : machinesModel) {
            for (auto [j, k] : operationsModel) {
                for (auto [j2, k2] : operationsModel) {
                    if (j == j2 && k == k2) continue;

                    for (int t : toolsModel) {
                        auto key_x = make_tuple(m, j, k, j2, k2);
                        if (!x.count(key_x)) continue;

                        GRBLinExpr lhs = x[key_x] + y[{t, j, k}] - y[{t, j2, k2}];
                        GRBLinExpr rhs = z[{t, j, k}] + 1;

                        model.addConstr(lhs <= rhs, "tool_switch_" + to_string(m) + "_" + to_string(j) + "_" + to_string(k) + "_" + to_string(j2) + "_" + to_string(k2) + "_t" + to_string(t));
                    }
                }
            }
        }

        // (13) Ferramentas requeridas devem estar presentes no início da operação
        for (auto [op, toolList] : requiredTools) {
            int j = op.first;
            int k = op.second;

            for (int t : toolList) {
                model.addConstr(y[{t, j, k}] == 1, "required_tool_" + to_string(t) + "_" + to_string(j) + "_" + to_string(k));
            }
        }

        // (14) Restrição: z(t)(jk) <= l(jk)
        for (auto [j, k] : operationsModel) {
            for (int t : toolsModel) {
                model.addConstr(z[{t, j, k}] <= l[{j, k}], "tool_switch_implies_instance_" + to_string(t) + "_" + to_string(j) + "_" + to_string(k));
            }
        }

        // (15) Restrição: l(jk) <= soma de z(t)(jk)
        for (auto [j, k] : operationsModel) {
            GRBLinExpr sum_z = 0;
            for (int t : toolsModel) {
                sum_z += z[{t, j, k}];
            }
            model.addConstr(l[{j, k}] <= sum_z, "instance_implies_tool_switch_" + to_string(j) + "_" + to_string(k));
        }

        // (16) impose that tool switching instances can only occur during supervised shifts, i.e., during the first (24−𝑡𝑈 ) supervised hours of each day, where (𝑠𝑗𝑘 mod 24) is a modulo that determines the time (hour) of a day at which operation (𝑗, 𝑘) begins. For example, if 𝑠𝑗𝑘 = 30 and 𝑡𝑈 = 12, then 𝑙𝑗𝑘 ≤ 1.5, which indicates that a tool switching instance may happen at hour 30
        map<pair<int, int>, GRBVar> h;  // hora do dia (s mod 24)
        map<pair<int, int>, GRBVar> q;  // número do dia (s div 24)

        for (auto [j, k] : operationsModel) {
            h[{j, k}] = model.addVar(0.0, 24.0, 0.0, GRB_CONTINUOUS, "h_" + to_string(j) + "_" + to_string(k));
            q[{j, k}] = model.addVar(0.0, GRB_INFINITY, 0.0, GRB_INTEGER, "q_" + to_string(j) + "_" + to_string(k));

            // s = 24 * q + h  → modelar s mod 24
            model.addConstr(s[{j, k}] == 24 * q[{j, k}] + h[{j, k}], "mod_reconstruction_" + to_string(j) + "_" + to_string(k));

            // l <= 2 - (h / (24 - tU))
            double denom = 24.0 - tU;
            model.addConstr(l[{j, k}] <= 2 - h[{j, k}] / denom, "supervised_tool_switch_" + to_string(j) + "_" + to_string(k));
        }

        // (25)-(27) Linearização de delta(jk) = alpha(jk) * l(jk)
        for (auto [j, k] : operationsModel) {
            model.addConstr(delta[{j, k}] <= alpha[{j, k}], "delta_le_alpha_" + to_string(j) + "_" + to_string(k));

            model.addConstr(delta[{j, k}] <= l[{j, k}], "delta_le_l_" + to_string(j) + "_" + to_string(k));

            model.addConstr(delta[{j, k}] >= alpha[{j, k}] + l[{j, k}] - 1, "delta_ge_sum_minus1_" + to_string(j) + "_" + to_string(k));
        }

        // (28) since the parallel machines are identical, there exist many alternative solutions that are similar but mirror the allocation of operations over the machines. Hence, we exclude those alternatives by adding symmetry-breaking constraints (28)
        for (long unsigned int idx = 1; idx < machinesModel.size(); ++idx) {
            int m_prev = machinesModel[idx - 1];
            int m_curr = machinesModel[idx];

            GRBLinExpr sum_prev = 0;
            GRBLinExpr sum_curr = 0;

            for (auto [j, k] : operationsModel) {
                sum_prev += betaVar[{m_prev, j, k}];
                sum_curr += betaVar[{m_curr, j, k}];
            }

            model.addConstr(sum_prev >= sum_curr, "symmetry_break_" + to_string(m_prev) + "_ge_" + to_string(m_curr));
        }

        // -------------------------------------------------
        // OBJs
        // -------------------------------------------------

        GRBLinExpr obj = 0;

        // + r * sum(alpha)
        for (auto [j, k] : operationsModel) {
            obj += r * alpha[{j, k}];
        }

        // - c(processingTimes) * sum(1 - alpha) para operações em priorityOperations
        for (auto [j, k] : priorityOperations) {
            obj += -cp * (1 - alpha[{j, k}]);
        }

        // - c(f) * sum(delta)
        for (auto [j, k] : operationsModel) {
            obj += -cf * delta[{j, k}];
        }

        // - c(v) * sum(lambda)
        for (auto [j, k] : operationsModel) {
            for (int t : toolsModel) {
                obj += -cv * lambda[{t, j, k}];
            }
        }

        model.setObjective(obj, GRB_MAXIMIZE);  // Define a função objetivo (maximização)

        // -------------------------------------------------
        // SOLVING AND OUTPUT
        // -------------------------------------------------

        model.update();                               // Atualiza o conteúdo do modelo
        model.write(folderOutput + "/testModel.lp");  // Escreve o modelo em um arquivo
        model.optimize();                             // Resolve o modelo

        int status = model.get(GRB_IntAttr_Status);  // Verifica o status do modelo
        if (status == GRB_UNBOUNDED) {
            cout << "O modelo nao pode ser resolvido porque e ilimitado" << endl;
            return 0;
        }
        if (status == GRB_INFEASIBLE) {
            cout << "O modelo nao pode ser resolvido porque e inviavel. Verifique o arquivo InfeasibilityCheck.ilp" << endl;
            model.computeIIS();
            model.write(folderOutput + "/InfeasibilityCheck.ilp");
            return 0;
        }

        model.write(folderOutput + "/modelSolution.sol");
        convertModelData(folderOutput, model);

    } catch (GRBException error) {
        cout << "Erro numero: " << error.getErrorCode() << endl;
        cout << error.getMessage() << endl;
    } catch (...) {
        cout << "Erro durante a construcao ou solucao do modelo" << endl;
    }

    return 0;
}