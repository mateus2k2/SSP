# --------------------------------------------------------
# Meus Codigos cpp
# --------------------------------------------------------

OUTPUT = src/out/mainCpp

DEBUG_MODE=1
GATILHO_MODE=0
FAST_MODE=1
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

ifeq ($(FAST_MODE), 1)
	FAST_MACRO = 
else
	FAST_MACRO = -O3
endif

ifeq ($(RAND_MODE), 1)
	RAND_MACRO = -DRANDSEED
else
	RAND_MACRO =
endif

debugCompilePT:
	clear 
	g++ $(DEBUG_MACRO) $(GATILHO_MACRO) $(RAND_MACRO) src/*.cpp -std=c++2a -Wshadow -pg -Wall -o src/out/mainCppDebug -Wno-unused-result -lpthread -g -march=native -lstdc++ $(USE_FTM)
	echo "\n" 
	clear

compilePT:
	clear
	g++ $(DEBUG_MACRO) $(GATILHO_MACRO) $(RAND_MACRO) ../PTAPI/include/*.h src/*.cpp -std=c++2a -Wshadow -Wall -o src/out/mainCpp -Wno-unused-result -lpthread $(FAST_MACRO) -march=native -lstdc++ $(USE_FTM)
	echo "\n" 

runPT:
	./src/out/mainCpp "./input/MyInstancesSameToolSets/n=75,p=0.24,r=0.5,t=650,v0.csv" "./input/Processed/ToolSetInt.csv" "./output/Exemplo/exemplo.txt" \
		--TEMP_INIT 0.1 \
		--TEMP_FIM 5 \
		--N_REPLICAS 2 \
		--MCL 5 \
		--PTL 10 \
		--PASSO_GATILHO 10 \
		--TEMP_DIST 3 \
		--TYPE_UPDATE 1 \
		--INIT_SOL_TYPE 0 \
		--TEMP_UPDATE 3500 \
		--PTL_TEMP_UPDATE_PROPORTION 3 \
		--RESULT_REPORT 1 \
		--DIFERENT_TOOLSETS_MODE 0 \

realPTGo:
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