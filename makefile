# Meus Codigos

mainCpp:
	clear && g++ src/MeuCpp/*.cpp -std=c++2a -Wshadow -o src/out/mainCpp -Wno-unused-result -lpthread -O3 -march=native && echo "\n" && clear && src/out/mainCpp

optmizeTollSetsPy:
	clear && echo "\n" && python src/MeuPython/optmizeToolSets.py 

toolSetsAnalysis:
	clear && echo "\n" && python src/MeuPython/toolSetsAnalysis.py 

testePy:
	clear && echo "\n" && python src/MeuPython/teste.py 

ganattPy:
	clear && echo "\n" && python src/MeuPython/ganatt.py 


# Codigos Andre

KTNS:
	clear && g++ ./src/KTNS/main.cpp ./src/KTNS/src/*.cpp -std=c++2a -Wshadow -o ./src/out/KTNS -Wno-unused-result -lpthread -O3 && echo "\n"

KTNSTest1:
	clear && ./src/out/KTNS ./dataRaw/KTNS/Instances/Mecler/Tabela1/F1001.txt 6 ./dataRaw/KTNS/SolList/Files/sollist50.txt && echo "\n"

KTNSTest2:
	clear && ./src/out/KTNS ./dataRaw/KTNS/Instances/Catanzaro/Tabela1/datA1 6 ./dataRaw/KTNS/SolList/Files/sollist10.txt && echo "\n"

# Update Git

git:
	clear && git add . && git commit -m "update" && git push origin master