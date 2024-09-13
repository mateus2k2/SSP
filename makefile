# --------------------------------------------------------
# Meus Codigos cpp
# --------------------------------------------------------

mainCpp:
	clear
	g++ ../PTAPI/include/*.h src/*.cpp -std=c++2a -Wshadow -Wall -o src/out/mainCpp -Wno-unused-result -lpthread -O3 -march=native -lfmt -lstdc++ 
	echo "\n" 
	clear 
	src/out/mainCpp

mainCppPTTest:
	clear
	g++ ../PTAPI/include/*.h src/*.cpp -std=c++2a -Wshadow -Wall -o src/out/mainCpp -Wno-unused-result -lpthread -O3 -march=native -lfmt -lstdc++
	echo "\n"
	src/out/mainCpp /home/mateus/WSL/IC/SSP/input/ExemploArtigo/Jobs.csv /home/mateus/WSL/IC/SSP/input/ExemploArtigo/ToolSets.csv /home/mateus/WSL/IC/SSP/output/exemploArtigo.txt --TEMP_INIT 0.2 --TEMP_FIM 1 --N_REPLICAS 16 --MCL 400 --PTL 1000 --TEMP_DIST 1 --TYPE_UPDATE 2 --TEMP_UPDATE 35000 --CAPACITY 8 --MACHINES 2 --DAYS 2 --UNSUPERVISEDMINUTS 720

mainCppPTReal:
	clear 
	g++ ../PTAPI/include/*.h src/*.cpp -std=c++2a -Wshadow -Wall -o src/out/mainCpp -Wno-unused-result -lpthread -O3 -march=native -lfmt -lstdc++ 
	echo "\n" 
	src/out/mainCpp /home/mateus/WSL/IC/SSP/input/MyInstancesSameToolSets/n=75,p=0.24,r=0.5,t=650,v0.csv /home/mateus/WSL/IC/SSP/input/ToolSetInt.csv /home/mateus/WSL/IC/SSP/output/output1.txt --TEMP_INIT 0.2 --TEMP_FIM 1 --N_REPLICAS 16 --MCL 400 --PTL 1000 --TEMP_DIST 1 --TYPE_UPDATE 2 --TEMP_UPDATE 35000 --CAPACITY 80 --MACHINES 2 --DAYS 2 --UNSUPERVISEDMINUTS 720

mainCppDebug:
	clear 
	g++ src/*.cpp -std=c++2a -Wshadow -Wall -o src/out/mainCppDebug -Wno-unused-result -lpthread -O3 -march=native -g -lfmt -lstdc++ 
	echo "\n" 
	clear

# --------------------------------------------------------
# Meus python
# --------------------------------------------------------

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

# --------------------------------------------------------
# Codigos Andre 
# --------------------------------------------------------

KTNS:
	clear && g++ ./src/KTNS/main.cpp ./src/KTNS/src/*.cpp -std=c++2a -Wshadow -o ./src/out/KTNS -Wno-unused-result -lpthread -O3 && echo "\n"

KTNSTest1:
	clear && ./src/out/KTNS ./dataRaw/KTNS/Instances/Mecler/Tabela1/F1001.txt 6 ./dataRaw/KTNS/SolList/Files/sollist50.txt && echo "\n"

KTNSTest2:
	clear && ./src/out/KTNS ./dataRaw/KTNS/Instances/Catanzaro/Tabela1/datA1 6 ./dataRaw/KTNS/SolList/Files/sollist10.txt && echo "\n"

# --------------------------------------------------------
# Update Git
# --------------------------------------------------------

git:
	clear && git add . && git commit -m "update" && git push origin master