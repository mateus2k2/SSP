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
#include <string>

#include "gurobi_c++.h"
#include "headers/SSP.h"

using namespace std;

#define N 2
#define M 3
#define O 4

int SSP::modelo(string folderOutput) {
    cout << "Modelo de exemplo" << endl;
    cout << "Teste: " << capacityMagazine << endl;

    // Tenta construir e resolver o modelo, capturando eventuais exceções
    try {
        // Cria um ambiente do Gurobi
        GRBEnv env = GRBEnv();

        // Cria um modelo do Gurobi
        GRBModel model = GRBModel(env);

        // Não permite que o Gurobi imprima as informações automaticamente (i.e., verbose mode off)
        model.getEnv().set(GRB_IntParam_OutputFlag, 0);

        // Cria variáveis com três índices (e.g. x(i, j, k))
        // Determina o domínio do primeiro índice
        for (int i = 0; i < N; i++)
            // Determina o domínio do segundo índice
            for (int j = 0; j < M; j++)
                // Determina o domínio do terceiro índice
                for (int k = 0; k < O; k++) {
                    char varname[100];
                    // Escreve em varname o nome da variável que será inserida no modelo (e.g. x(1, 2, 3)
                    sprintf(varname, "x(%d,%d,%d)", i, j, k);
                    // Adiciona a variável ao modelo, indicando que é binária
                    model.addVar(0.0, 1.0, 1.0, GRB_BINARY, varname);
                }

        // Atualiza o conteúdo do modelo
        model.update();

        // Cria a expressão linear que representa a função objetivo
        GRBLinExpr obj = 0;

        // Monta o nome das variáveis, as recupera de dentro do modelo e as inclui na função objetivo
        for (int i = 0; i < N; i++)
            for (int j = 0; j < M; j++)
                for (int k = 0; k < O; k++) {
                    char varname[100];
                    // Escreve o nome das variáveis
                    sprintf(varname, "x(%d,%d,%d)", i, j, k);
                    // Acessa as variáveis do modelo pelo nome
                    // Multiplica as variáveis por um peso fictício e as adiciona ao somatório
                    // e.g. 100000*x(0, 0, 0) + 100000*x(0, 0, 1) + ... + 100000*x(1, 2, 3) + ... + 100000*x(N-1, M-1, O-1)
                    obj += 100000 * model.getVarByName(varname);
                }
        // Define a função objetivo como sendo de minimização e a adiciona ao modelo
        model.setObjective(obj, GRB_MINIMIZE);

        // Atualiza o conteúdo do modelo
        model.update();

        // Monta as restrições e as adiciona ao modelo

        // Cria o lado direto das expressões do primeiro conjunto de restrições
        // Monta o nome das variáveis, as recupera de dentro do modelo e as inclui na expressão que representa as restrições
        // Nesse caso específico, agrupa as variáveis pelo primeiro indice em cada restrição, e.g. x(0, 0, 0), x(0, 1, 1), ..., x(0, M-1, O-1)
        for (int i = 0; i < N; i++) {
            // Cria uma expressão linear
            GRBLinExpr lhs = 0;
            for (int j = 0; j < M; j++) {
                for (int k = 0; k < O; k++) {
                    char varname[100];
                    // Escreve o nome da variável
                    sprintf(varname, "x(%d,%d,%d)", i, j, k);
                    // Acessa a variável pelo nome e a adiciona ao somatório da restrição
                    // e.g. x(0, 0, 0)+x(0, 1, 1)+ ...+x(0, M-1, O-1)
                    // e.g. x(1, 0, 0)+x(1, 1, 1)+ ...+x(1, M-1, O-1)
                    lhs += model.getVarByName(varname);
                }
            }
            // Cria o lado direito das expressões, com valor 1
            GRBLinExpr rhs = 1;
            // Escreve o nome das restrições
            char constraint[100];
            sprintf(constraint, "RestricaoFixaPrimeiroElemento%d", i);
            // Indica que a restrição é uma desigualdade e adiciona os dois lados da expressão
            // e.g. x(0, 0, 0)+x(0, 1, 1)+ ...+x(0, M-1, O-1) <= 1
            // e.g. x(1, 0, 0)+x(1, 1, 1)+ ...+x(1, M-1, O-1) <= 1
            model.addConstr(lhs >= rhs, constraint);
        }

        // Atualiza o conteúdo do modelo
        model.update();

        // Cria o lado direto das expressões do segundo conjunto de restrições em um processo semelhanto ao anterior
        // Monta o nome das variáveis, as recupera de dentro do modelo e as inclui na expressão que representa as restrições
        // Nesse caso específico, agrupa as variáveis pelo segundo indice em cada restrição, e.g. x(0, 0, 0), ..., x(1, 0, 1), ..., x(N-1, 0, O-1)
        for (int j = 0; j < M; j++) {
            GRBLinExpr lhs = 0;
            for (int i = 0; i < N; i++) {
                for (int k = 0; k < O; k++) {
                    char varname[100];
                    sprintf(varname, "x(%d,%d,%d)", i, j, k);
                    lhs += model.getVarByName(varname);
                }
            }

            GRBLinExpr rhs = 1;
            char constraint[100];
            sprintf(constraint, "RestricaoFixaSegundoElemento%d", j);
            // Indica que a restrição é uma igualdade e adiciona os dois lados da expressão
            // e.g. x(0, 0, 0)+x(0, 0, 1)+ ...+x(N-1, 0, O-1) = 1
            // e.g. x(0, 1, 0)+x(0, 1, 1)+ ...+x(N-1, 1, O-1) = 1
            model.addConstr(lhs, '=', rhs, constraint);
        }

        // Atualiza o conteúdo do modelo
        model.update();

        // Cria o lado direto das expressões do terceiro conjunto de restrições em um processo semelhanto aos anteriores
        // Monta o nome das variáveis, as recupera de dentro do modelo e as inclui na expressão que representa as restrições
        // Nesse caso específico, agrupa as variáveis pelo terceiro indice em cada restrição, e.g. x(0, 0, 0), ..., x(0, 1, 1), ..., x(N-1, M-1, 0)
        for (int k = 0; k < O; k++) {
            GRBLinExpr lhs = 0;
            for (int i = 0; i < N; i++) {
                for (int j = 0; j < M; j++) {
                    char varname[100];
                    sprintf(varname, "x(%d,%d,%d)", i, j, k);
                    lhs += model.getVarByName(varname);
                }
            }

            GRBLinExpr rhs = 1;
            char constraint[100];
            sprintf(constraint, "RestricaoFixaTerceiroElemento%d", k);

            // Indica que a restrição é uma desigualdade e adiciona os dois lados da expressão
            // e.g. x(0, 0, 0)+x(0, 1, 0)+ ...+x(N-1, M-1, 0) >= 1
            // e.g. x(0, 0, 1)+x(0, 1, 1)+ ...+x(N-1, M-1, 1) >= 1
            model.addConstr(lhs >= rhs, constraint);
        }

        // Atualiza o conteúdo do modelo
        model.update();

        // Escreve o modelo em um arquivo
        model.write(folderOutput + "/testModel.lp");

        // Resolve o modelo
        model.optimize();

        // Verifica o status do modelo
        int status = model.get(GRB_IntAttr_Status);

        // Imprime o status do modelo
        if (status == GRB_UNBOUNDED) {
            cout << "O modelo nao pode ser resolvido porque e ilimitado" << endl;
            return 0;
        }
        if (status == GRB_OPTIMAL) {
            cout << "Solucao otima encontrada!" << endl;
            // Acessa e imprime o valor da funcao objetivo
            cout << "O valor da solucao otima e: " << model.get(GRB_DoubleAttr_ObjVal) << endl;
        }
        if (status == GRB_INFEASIBLE) {
            cout << "O modelo nao pode ser resolvido porque e inviavel. Verifique o arquivo InfeasibilityCheck.ilp" << endl;
            model.computeIIS();
            model.write(folderOutput + "/InfeasibilityCheck.ilp");
            return 0;
        }

        // Acessa as variáveis do modelo depois de resolvido
        GRBVar* v = model.getVars();

        int i, j, k;
        // Extrai a solução. Neste caso, verifica quais variáveis possuem valor diferente de zero, com precisão de 7 casas decimais
        for (int index = 0; index < model.get(GRB_IntAttr_NumVars); ++index) {
            double sol = v[index].get(GRB_DoubleAttr_X);

            if (sol > 0.0000001) {
                sscanf(v[index].get(GRB_StringAttr_VarName).c_str(), "x(%d,%d,%d)", &i, &j, &k);
                printf("A variavel x(%d,%d,%d) faz parte da solucao\n", i, j, k);
            }
        }

        // De maneira alternativa, imprime o valor de cada uma das variáveis
        for (int index = 0; index < model.get(GRB_IntAttr_NumVars); ++index) {
            double sol = v[index].get(GRB_DoubleAttr_X);

            sscanf(v[index].get(GRB_StringAttr_VarName).c_str(), "x(%d,%d,%d)", &i, &j, &k);
            printf("x(%d,%d,%d) = %.2lf\n", i, j, k, sol);
        }

        // Imprime a solução em um arquivo
        model.write(folderOutput + "/modelSolution.sol");
        // captura eventuais excecoes
    } catch (GRBException e) {
        cout << "Erro numero: " << e.getErrorCode() << endl;
        cout << e.getMessage() << endl;
    } catch (...) {
        cout << "Erro durante a construcao ou solucao do modelo" << endl;
    }
}