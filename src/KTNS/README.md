# KTNS
Implementações do KTNS<br> 
<br>
Para compilar utilizar o comando abaixo:<br>
g++ "%f" ./src/*.cpp  -std=c++20 -Wshadow -Wall  -o "%e" -Wno-unused-result  -lpthread -O3<br>
<br>
<br>
Template de execução:<br>
./main [instância de teste] [qual algoritmo utilizar] [arquivo com as permutações] <br>
[instância de teste] : Caminho para a instância a ser utilizada.<br>
[qual algoritmo utilizar] : Qual versão do algoritmo será executado. <br>
[arquivo com as permutações] : Arquivo com as permutações. <br> 
1 : KTNS sequencial proposto pelos autores. <br>
2 : KTNS sequencial proposto por Mecler et al. [2021]. <br>
3 : GPCA sequencial proposto por Cherniavskii e Goldengorin [2022]. <br>
4 : ToFullMag sequencial proposto por Cherniavskii e Goldengorin [2022]. <br>
5 : KTNS proposto paralelo. <br>
6 : GPCA paralelo. <br>
<br>
Exemplo:<br>
./main ./Instances/Mecler/Tabela1/F1001.txt 6 ./SolList/sollist50.txt <br>
