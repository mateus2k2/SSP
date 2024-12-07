# --------------------------------------------------------
# Meus Codigos cpp
# --------------------------------------------------------

USE_FTM = -lfmt
# USE_FTM =

# valgrind --tool=callgrind ./src/out/mainCppDebug ./input/MyInstancesSameToolSets/n=75,p=0.24,r=0.5,t=650,v0.csv ./input/Processed/ToolSetInt.csv ./output/Exemplo/output1.txt --TEMP_INIT 0.2 --TEMP_FIM 1 --N_REPLICAS 16 --MCL 400 --PTL 10 --TEMP_DIST 1 --TYPE_UPDATE 2 --TEMP_UPDATE 1000 --CAPACITY 80 --MACHINES 2 --DAYS 2 --UNSUPERVISED_MINUTS 720
# https://waterprogramming.wordpress.com/2017/06/08/profiling-c-code-with-callgrind/

CompilePT:
	clear
	g++ ../PTAPI/include/*.h src/*.cpp -std=c++2a -Wshadow -Wall -o src/out/mainCpp -Wno-unused-result -lpthread -O3 -march=native -lstdc++ $(USE_FTM)
	echo "\n" 

DebugGo:
	clear 
	g++ src/*.cpp -std=c++2a -Wshadow -pg -Wall -o src/out/mainCppDebug -Wno-unused-result -lpthread -O3 -g -march=native -lstdc++ $(USE_FTM)
	echo "\n" 
	clear

TestPTGo:
	clear
	g++ ../PTAPI/include/*.h src/*.cpp -std=c++2a -Wshadow -Wall -o src/out/mainCpp -Wno-unused-result -lpthread -O3 -march=native -lstdc++ $(USE_FTM)
	echo "\n"
	src/out/mainCpp ./input/Exemplo/Jobs.csv ./input/Exemplo/ToolSets.csv ./output/Exemplo/exemplo.txt  \
		--TEMP_INIT 0.2  			\
		--TEMP_FIM 1  				\
		--N_REPLICAS 16  			\
		--MCL 400  					\
		--PTL 1000  				\
		--TEMP_DIST 1  				\
		--TYPE_UPDATE 2  			\
		--TEMP_UPDATE 35000  		\
		--CAPACITY 8  				\
		--MACHINES 2  				\
		--DAYS 2  					\
		--UNSUPERVISED_MINUTS 720  	\
		--RESULT_REPORT 1  			\
		--INSTANCE_REPORT 1  		\
		--INSTANCE_MODE 0 			\

RealPTGo:
	clear 
	g++ ../PTAPI/include/*.h src/*.cpp -std=c++2a -Wshadow -Wall -o src/out/mainCpp -Wno-unused-result -lpthread -O3 -march=native -lstdc++ $(USE_FTM)
	echo "\n" 
	./src/out/mainCpp "./input/MyInstancesDiferentToolSets/n=1236,p=0.75,r=0.6,t=1265,v0.csv" "./input/Processed/ToolSetInt.csv" "./output/Exemplo/exemplo.txt" \
		--TEMP_INIT 0.1  			\
		--TEMP_FIM 0.5  			\
		--N_REPLICAS 11  			\
		--MCL 400  					\
		--PTL 100  					\
		--TEMP_DIST 1  				\
		--TYPE_UPDATE 2  			\
		--TEMP_UPDATE 20000  		\
		--CAPACITY 80  				\
		--MACHINES 6  				\
		--DAYS 11  					\
		--UNSUPERVISED_MINUTS 720  	\
		--RESULT_REPORT 1  			\
		--INSTANCE_MODE 0  			\

# --------------------------------------------------------
# Meus python
# --------------------------------------------------------

# TODO
analisyFolder:
	clear
	# python3 ./scripts/reportAnalises.py ./output/Ajuste1-PTL100/MyInstancesDiferentToolSets > out1
	# python3 ./scripts/reportAnalises.py ./output/Ajuste1-PTL100/MyInstancesSameToolSets > out2
	
	# python3 ./scripts/reportAnalises.py ./output/Ajuste2-PTL100/MyInstancesDiferentToolSets > out3
	# python3 ./scripts/reportAnalises.py ./output/Ajuste2-PTL100/MyInstancesSameToolSets > out4

	# python3 ./scripts/reportAnalises.py ./output/PTL100/MyInstancesDiferentToolSets > out5
	# python3 ./scripts/reportAnalises.py ./output/PTL100/MyInstancesSameToolSets > out6

	# python3 ./scripts/reportAnalises.py ./output/PTL1000/MyInstancesDiferentToolSets > out7
	# python3 ./scripts/reportAnalises.py ./output/PTL1000/MyInstancesSameToolSets > out8

	# python3 ./scripts/reportAnalises.py ./output/TCC1/MyInstancesDiferentToolSets > out9
	# python3 ./scripts/reportAnalises.py ./output/TCC1/MyInstancesSameToolSets > out10

	echo "\n"

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