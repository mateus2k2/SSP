# Meus Codigos

mainCpp:
	clear && g++ src/*.cpp -std=c++2a -Wshadow -o src/out/mainCpp -Wno-unused-result -lpthread -O3 -march=native && echo "\n" && clear && src/out/mainCpp

mainCppDebug:
	clear && g++ src/*.cpp -std=c++2a -Wshadow -o src/out/mainCppDebug -Wno-unused-result -lpthread -O3 -march=native -g && echo "\n" && clear

filterToolSets:
	clear && echo "\n" && python uteis/filterToolSets.py 

filterJobs:
	clear && echo "\n" && python uteis/filterJobs.py 

myInstanceGenerator:
	clear && echo "\n" && python uteis/myInstanceGenerator.py 

gantt:
	clear && echo "\n" && python uteis/gantt.py 

# make validador arg1=single arg2=/home/mateus/WSL/IC/SSP/output/exemploArtigoBoa.txt
validador: 
	clear && echo "\n" && python uteis/validador.py $(arg1) $(arg2)

uteis:
	clear && echo "\n" && python uteis/uteis.py 

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