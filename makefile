# --------------------------------------------------------
# Meus Codigos cpp
# --------------------------------------------------------

DEBUG_MODE = 1

ifeq ($(DEBUG_MODE), 1)
    USE_FTM = -lfmt
    DEBUG_MACRO = -DDEBUG
else
    USE_FTM =
    DEBUG_MACRO =
endif

compilePT:
	clear
	g++ $(DEBUG_MACRO) ../PTAPI/include/*.h src/*.cpp -std=c++2a -Wshadow -Wall -o src/out/mainCpp -Wno-unused-result -lpthread -O3 -march=native -lstdc++ $(USE_FTM)
	echo "\n" 

debugCompilePT:
	clear 
	g++ $(DEBUG_MACRO) src/*.cpp -std=c++2a -Wshadow -pg -Wall -o src/out/mainCppDebug -Wno-unused-result -lpthread -O3 -g -march=native -lstdc++ $(USE_FTM)
	echo "\n" 
	clear

testPTGo:
	clear
	g++ $(DEBUG_MACRO) ../PTAPI/include/*.h src/*.cpp -std=c++2a -Wshadow -Wall -o src/out/mainCpp -Wno-unused-result -lpthread -O3 -march=native -lstdc++ $(USE_FTM)
	echo "\n"
	src/out/mainCpp ./input/Exemplo/Jobs.csv ./input/Exemplo/ToolSets.csv ./output/Exemplo/exemplo.txt  \
		--TEMP_INIT 0.2 \
		--TEMP_FIM 1 \
		--N_REPLICAS 16 \
		--MCL 400 \
		--PTL 1000 \
		--TEMP_DIST 1 \
		--TYPE_UPDATE 2 \
		--TEMP_UPDATE 35000 \
		--CAPACITY 8 \
		--MACHINES 2 \
		--DAYS 2 \
		--UNSUPERVISED_MINUTS 720 \
		--RESULT_REPORT 1 \
		--INSTANCE_REPORT 1 \
		--INSTANCE_MODE 0 \

realPTGo:
	clear 
	g++ $(DEBUG_MACRO) ../PTAPI/include/*.h src/*.cpp -std=c++2a -Wshadow -Wall -o src/out/mainCpp -Wno-unused-result -lpthread -O3 -march=native -lstdc++ $(USE_FTM)
	echo "\n" 
	./src/out/mainCpp "./input/MyInstancesSameToolSets/n=1236,p=0.75,r=0.6,t=4431,v23.csv" "./input/Processed/ToolSetInt.csv" "./output/Exemplo/exemplo.txt" \
		--TEMP_INIT 0.1 \
		--TEMP_FIM 0.5 \
		--N_REPLICAS 11 \
		--MCL 400 \
		--PTL 10 \
		--TEMP_DIST 1 \
		--TYPE_UPDATE 2 \
		--TEMP_UPDATE 20000 \
		--MOVEMENT_TYPE 1 \
		--INIT_SOL_TYPE 1 \
		--RESULT_REPORT 1 \
		--INSTANCE_MODE 1 \

# --------------------------------------------------------
# Meus python
# --------------------------------------------------------

# TODO
analisyFolder:
	clear
	python3 ./scripts/reportAnalises.py ./output/Exemplo 2
	echo "\n"

# --------------------------------------------------------
# Update Git
# --------------------------------------------------------

message ?= update

git:
	clear && git add . && git commit -m "$(message)" && git push origin master