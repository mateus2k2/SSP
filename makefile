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

testPTGo:
	make compilePT
	src/out/mainCpp ./input/Exemplo/Jobs.csv ./input/Exemplo/ToolSets.csv ./output/Exemplo/exemplo.txt  \
		--TEMP_INIT 0.2 \
		--TEMP_FIM 1 \
		--N_REPLICAS 16 \
		--MCL 400 \
		--PTL 1000 \
		--TEMP_DIST 1 \
		--TYPE_UPDATE 1 \
		--TEMP_UPDATE 35000 \
		--PTL_TEMP_UPDATE_PROPORTION 1 \
		--CAPACITY 8 \
		--MACHINES 2 \
		--DAYS 2 \
		--UNSUPERVISED_MINUTS 720 \
		--INSTANCE_REPORT 1 \
		--DIFERENT_TOOLSETS_MODE 0 \

realPTGo:
	make compilePT
	./src/out/mainCpp "./input/MyInstancesSameToolSets/n=75,p=0.24,r=0.5,t=650,v0.csv" "./input/Processed/ToolSetInt.csv" "./output/Exemplo/exemplo.txt" \
		--TEMP_INIT 0.1 \
		--TEMP_FIM 0.5 \
		--N_REPLICAS 11 \
		--MCL 400 \
		--PTL 100 \
		--TEMP_DIST 1 \
		--TYPE_UPDATE 1 \
		--INIT_SOL_TYPE 1 \
		--TEMP_UPDATE 20 \
		--PTL_TEMP_UPDATE_PROPORTION 1 \
		--RESULT_REPORT 1 \
		--DIFERENT_TOOLSETS_MODE 0 \

# --------------------------------------------------------
# Meus python
# --------------------------------------------------------

# TODO
analisyFolder:
	clear
	python ./scripts/reportAnalises.py ./output/Exemplo 3
	echo "\n"

# --------------------------------------------------------
# Update Git
# --------------------------------------------------------

m ?= update

git:
	clear && git add . && git commit -m "$(m)" && git push origin master