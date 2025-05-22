# --------------------------------------------------------
# Meus Codigos cpp
# --------------------------------------------------------

# make DEBUG_MODE=0 GATILHO_MODE=0 FAST_COMPILE_MODE=0 RAND_MODE=1 goPT

# PT
# ./scripts/runMultiple.sh ./output/SBPOPequenas same PT 9
# make tabelaResultadosPT > out 

# Modelo
# ./scripts/runAuto.sh ./output/Modelo same modelo 9999
# make tabelaResultadosModelo > out

# Practitioner
# ./scripts/runAuto.sh ./output/Practitioner same practitioner 9999
# make tabelaResultadosPractitioner > out

OUTPUT = src/out/mainCpp

DEBUG_MODE=1
GATILHO_MODE=0
FAST_COMPILE_MODE=1
RAND_MODE=1

GUROBI_VERSION=120

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

debugCompile:
	clear 
	g++ $(DEBUG_MACRO) $(GATILHO_MACRO) $(RAND_MACRO) src/*.cpp -std=c++2a -Wshadow -pg -Wall -m64 -g -o src/out/mainCppDebug -I${GUROBI_HOME}/include -L${GUROBI_HOME}/lib -lgurobi_c++ -lgurobi${GUROBI_VERSION} -lm -Wno-unused-result -lpthread -g -march=native -lstdc++ $(USE_FTM)
	echo "\n" 
	clear

compile:
	clear
	g++ $(DEBUG_MACRO) $(GATILHO_MACRO) $(RAND_MACRO) ../PTAPI/include/*.h src/*.cpp -std=c++2a -Wshadow -Wall -m64 -g -o src/out/mainCpp -I${GUROBI_HOME}/include -L${GUROBI_HOME}/lib -lgurobi_c++ -lgurobi${GUROBI_VERSION} -lm -Wno-unused-result -lpthread $(FAST_COMPILE_MACRO) -march=native -lstdc++ $(USE_FTM)
	echo "\n" 


runPT:
	./src/out/mainCpp \
		"./input/MyInstancesSameToolSets/n=212,p=0.75,r=0.4,t=1390,v8.csv" \
		"./input/Processed/ToolSetInt.csv" \
		"./output/Exemplo/pt.csv" \
		--TEMP_INIT 0.1 \
		--TEMP_FIM 5 \
		--N_REPLICAS 11 \
		--MCL 500 \
		--PTL 100 \
		--PASSO_GATILHO 10 \
		--TEMP_DIST 3 \
		--TYPE_UPDATE 1 \
		--INIT_SOL_TYPE 1 \
		--TEMP_UPDATE 3500 \
		--PTL_TEMP_UPDATE_PROPORTION 3 \
		--DIFERENT_TOOLSETS_MODE 0 \

runModelo:
	./src/out/mainCpp \
		"./input/Exemplo/Jobs.csv" \
		"./input/Exemplo/ToolSets.csv" \
		"./output/Modelo/Exemplo.csv" \
		--DIFERENT_TOOLSETS_MODE 0 \
		--INSTANCE_REPORT 0 \
		--TIME_LIMIT 5 \
		--MODELO 1 \

runPractitioner:
	./src/out/mainCpp \
		"./input/Exemplo/Jobs.csv" \
		"./input/Exemplo/ToolSets.csv" \
		"./output/Practitioner/practitioner.txt" \
		--DIFERENT_TOOLSETS_MODE 0 \
		--INSTANCE_REPORT 0 \
		--PRACTITIONER 1 \
		--SEQUENCE_BY 1 \


# "./input/Exemplo/Jobs.csv" 
# "./input/Exemplo/ToolSets.csv" 

# "./input/MyInstancesSameToolSets/n=15,p=0.5,r=0.5,t=0,v0.csv" 
# "./input/MyInstancesSameToolSets/n=212,p=0.75,r=0.4,t=1390,v8.csv" 
# "./input/Processed/ToolSetInt.csv" 


goPractitioner:
	make compile
	make runPractitioner

goModelo:
	make compile
	make runModelo

goPT:
	make compile
	make runPT

# --------------------------------------------------------
# Meus python
# --------------------------------------------------------

validarFile:
	clear
	python ./scripts/reportAnalises.py ./output/Modelo/MyInstancesSameToolSets/n=15,p=0.25,r=0.5,t=0,v0.csv 0
	echo "\n"

tabelaResultadosPractitioner:
	clear
	python ./scripts/reportAnalises.py ./output/Practitioner/MyInstancesSameToolSets/ 2
	echo "\n"

tabelaResultadosModelo:
	clear
	python ./scripts/reportAnalises.py ./output/Modelo/MyInstancesSameToolSets 3
	echo "\n"

tabelaResultadosPT:
	clear
	python ./scripts/reportAnalises.py ./output/TCC2V2 4
	echo "\n"

tabelaResultadosComparativa:
	clear
	python ./scripts/reportAnalises.py ./output/TCC2V2 5
	echo "\n"

# --------------------------------------------------------
# Update Git
# --------------------------------------------------------

m ?= update

git:
	clear && git add . && git commit -m "$(m)" && git push origin master