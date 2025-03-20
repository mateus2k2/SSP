# --------------------------------------------------------
# Meus Codigos cpp
# --------------------------------------------------------

# make DEBUG_MODE=0 GATILHO_MODE=0 FAST_COMPILE_MODE=0 RAND_MODE=1 PTGo

OUTPUT = src/out/mainCpp

DEBUG_MODE=1
GATILHO_MODE=0
FAST_COMPILE_MODE=1
RAND_MODE=1

ifeq ($(DEBUG_MODE), 1)
    USE_FTM = -lfmt
    DEBUG_MACRO = -DDEBUG
else
    USE_FTM =
    DEBUG_MACRO =
endif

ifeq ($(GATILHO_MODE), 1)
	GATILHO_MACRO = -DGATILHO
else
	GATILHO_MACRO =
endif

ifeq ($(FAST_COMPILE_MODE), 1)
	FAST_COMPILE_MACRO = 
else
	FAST_COMPILE_MACRO = -O3
endif

ifeq ($(RAND_MODE), 1)
	RAND_MACRO = -DRANDSEED
else
	RAND_MACRO =
endif

debugCompilePT:
	clear 
	g++ $(DEBUG_MACRO) $(GATILHO_MACRO) $(RAND_MACRO) src/*.cpp -std=c++2a -Wshadow -pg -Wall -m64 -g -o src/out/mainCppDebug -I${GUROBI_HOME}/include -L${GUROBI_HOME}/lib -lgurobi_c++ -lgurobi120 -lm -Wno-unused-result -lpthread -g -march=native -lstdc++ $(USE_FTM)
	echo "\n" 
	clear

compilePT:
	clear
	g++ $(DEBUG_MACRO) $(GATILHO_MACRO) $(RAND_MACRO) ../PTAPI/include/*.h src/*.cpp -std=c++2a -Wshadow -Wall -m64 -g -o src/out/mainCpp -I${GUROBI_HOME}/include -L${GUROBI_HOME}/lib -lgurobi_c++ -lgurobi120 -lm -Wno-unused-result -lpthread $(FAST_COMPILE_MACRO) -march=native -lstdc++ $(USE_FTM)
	echo "\n" 

runPT:
	./src/out/mainCpp "./input/MyInstancesSameToolSets/n=212,p=0.75,r=0.4,t=1390,v8.csv" "./input/Processed/ToolSetInt.csv" "./output/Exemplo/exemplo.txt" \
		--TEMP_INIT 0.1 \
		--TEMP_FIM 5 \
		--N_REPLICAS 11 \
		--MCL 500 \
		--PTL 600 \
		--PASSO_GATILHO 10 \
		--TEMP_DIST 3 \
		--TYPE_UPDATE 1 \
		--INIT_SOL_TYPE 0 \
		--TEMP_UPDATE 3500 \
		--PTL_TEMP_UPDATE_PROPORTION 3 \
		--DIFERENT_TOOLSETS_MODE 0 \

runModelo:
	./src/out/mainCpp "./input/MyInstancesSameToolSets/n=212,p=0.75,r=0.4,t=1390,v8.csv" "./input/Processed/ToolSetInt.csv" "./output/Modelo" 

modeloGo:
	make compilePT
	make runModelo

PTGo:
	make compilePT
	make runPT

# --------------------------------------------------------
# Meus python
# --------------------------------------------------------

# TODO
analisyFolder:
	clear
	python ./scripts/reportAnalises.py ./output/TCC2V2/ 6
	echo "\n"

# --------------------------------------------------------
# Update Git
# --------------------------------------------------------

m ?= update

git:
	clear && git add . && git commit -m "$(m)" && git push origin master