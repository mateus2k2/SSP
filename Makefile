# --------------------------------------------------------
# Meus Codigos cpp
# --------------------------------------------------------

# PT
# ./scripts/runMultiple.sh ./output/TeslaPT same PT 9999 tesla /home/marco/mateus/Gurobi/gurobi.lic > out
# make tabelaResultadosPT > out 

# Modelo
# ./scripts/runAuto.sh ./output/TeslaModelo same modelo 9999 tesla /home/marco/mateus/Gurobi/gurobi.lic > out
# make tabelaResultadosModelo > out

# Practitioner
# ./scripts/runAuto.sh ./output/Practitioner same practitioner 9999 tesla /home/marco/mateus/Gurobi/gurobi.lic > out
# make tabelaResultadosPractitioner > out

PRINT_MODE=0
FMT_MODE=0
GATILHO_MODE=0
RAND_MODE=1
TESLA_MODE=0
GUROBI_VERSION=130
OPTIMIZATION=-O3
SANITAZE=

ifeq ($(FMT_MODE), 1)
    USE_FMT = -lfmt
	FMT_MACRO = -DFMT
else
	FMT_MACRO = 
    USE_FMT =
endif

ifeq ($(PRINT_MODE), 1)
    PRINT_MACRO = -DDEBUG
else
    PRINT_MACRO =
endif

ifeq ($(RAND_MODE), 1)
	RAND_MACRO = -DRANDSEED
else
	RAND_MACRO =
endif

ifeq ($(GATILHO_MODE), 1)
	GATILHO_MACRO = -DGATILHO
else
	GATILHO_MACRO =
endif

ifeq ($(TESLA_MODE), 1)
	TESLA_MACRO = -D_GLIBCXX_USE_CXX11_ABI=0
else
	TESLA_MACRO =
endif

# --------------------------------------------------------
# COMPILACAO CPP
# --------------------------------------------------------

PTAPI_HOME := ../PTAPI
SRC_DIR := src
OBJ_DIR := build
BIN_DIR := $(SRC_DIR)/out
EXEC := $(BIN_DIR)/mainCpp

CPP_FILES := $(wildcard $(SRC_DIR)/*.cpp)
OBJ_FILES := $(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(CPP_FILES))

CXX := g++
CXXFLAGS := -std=c++2a -Wall -Wshadow -m64 -march=native $(DEBUG_MACRO_OPTS) $(FMT_MACRO) ${PRINT_MACRO} $(GATILHO_MACRO) $(RAND_MACRO) $(TESLA_MACRO)
LDFLAGS := -L${GUROBI_HOME}/lib -lgurobi_c++ -lgurobi${GUROBI_VERSION} -lpthread ${DEBUG_MACRO_OPTS} -lm -lstdc++ $(USE_FMT)
INCLUDES := -I${GUROBI_HOME}/include -I$(PTAPI_HOME)/include

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) ${OPTIMIZATION} $(SANITAZE) -c $< -o $@ $(INCLUDES)

compile: $(OBJ_FILES)
	@echo "Compilando..."
	@mkdir -p $(BIN_DIR)
	$(CXX) $^ -o $(EXEC) $(SANITAZE) $(LDFLAGS)
	@echo "Compilado com sucesso em $(EXEC)"

# SANITAZE="-fsanitize=address -fno-omit-frame-pointer"
debug:
	make clean
	make -j$(nproc) DEBUG_MACRO_OPTS=-g OPTIMIZATION=-O0 compile

teslaCompile:
	make TESLA_MODE=1 GUROBI_VERSION=91 FMT_MODE=0 GATILHO_MODE=0 RAND_MODE=1 compile

normalCompile:
	make -j$(nproc) FMT_MODE=0 GATILHO_MODE=0 RAND_MODE=1 compile

devCompile:
	make -j$(nproc) FMT_MODE=1 GATILHO_MODE=0 RAND_MODE=0 compile

clean:
	rm -rf $(OBJ_DIR) $(BIN_DIR)/mainCpp

# --------------------------------------------------------
# RUN CPP
# --------------------------------------------------------

runPTDiff:
	./src/out/mainCpp \
		"./input/MyInstancesDiferentToolSets/n=75,p=0.24,r=0.5,t=112,v0.csv" \
		"./input/Processed/ToolSetInt.csv" \
		"./output/Exemplo/pt.csv" \
		--TEMP_INIT 0.1 \
		--TEMP_FIM 5 \
		--N_REPLICAS 2 \
		--MCL 500 \
		--PTL 600 \
		--PASSO_GATILHO 10 \
		--TEMP_DIST 3 \
		--TYPE_UPDATE 1 \
		--INIT_SOL_TYPE 0 \
		--TEMP_UPDATE 3500 \
		--PTL_TEMP_UPDATE_PROPORTION 3 \
		--DIFERENT_TOOLSETS_MODE 1 \

runPT:
	./src/out/mainCpp \
		"./input/MyInstancesSameToolSets/n=499,p=0.25,r=0.5,t=2690,v3.csv" \
		"./input/Processed/ToolSetInt.csv" \
		"./output/Exemplo/pt.csv" \
		--PTL_TEMP_UPDATE_PROPORTION 3 \
		--DIFERENT_TOOLSETS_MODE 0 \
		--TEMP_INIT 0.1 \
        --TEMP_FIM 5 \
        --N_REPLICAS 11 \
        --MCL 500 \
        --PTL 100 \
        --PASSO_GATILHO 10 \
        --TEMP_DIST 3 \
        --TYPE_UPDATE 1 \
        --INIT_SOL_TYPE 0 \
        --TEMP_UPDATE 3500 \
        --PTL_TEMP_UPDATE_PROPORTION 3 \

runModelo:
	./src/out/mainCpp \
		"./input/MyInstancesSameToolSets/n=75,p=0.24,r=0.5,t=650,v0.csv" \
		"./input/Processed/ToolSetInt.csv" \
		"./output/Modelo/Exemplo.csv" \
		--DIFERENT_TOOLSETS_MODE 0 \
		--INSTANCE_REPORT 0 \
		--TIME_LIMIT 5 \
		--MODELO 1 \

# "./input/BeezaoRaw/IPMTC-I/instance931_m=2_n=25_l=20_c=15_s=0.PMTC" \
# "./input/BeezaoRaw/IPMTC-I/instance1_m=2_n=8_l=15_c=5_s=0.PMTC" \
# "./input/BeezaoRaw/IPMTC-II/instanceLarge932_m=6_n=200_l=40_c=30_s=1.PMTC" \
# "./input/BeezaoRaw/IPMTC-II/instanceLarge931_m=6_n=200_l=40_c=30_s=0.PMTC" \

runBeezao:
	./src/out/mainCpp \
		"./input/BeezaoRaw/IPMTC-II/instanceLarge960_m=6_n=200_l=40_c=30_s=2.PMTC" \
		"" \
		"./output/Beezao/teste.txt" \
		--PTL_TEMP_UPDATE_PROPORTION 3 \
		--DIFERENT_TOOLSETS_MODE 0 \
		--TEMP_INIT 0.1 \
        --TEMP_FIM 5 \
        --N_REPLICAS 11 \
        --MCL 500 \
        --PTL 500 \
        --PASSO_GATILHO 10 \
        --TEMP_DIST 3 \
        --TYPE_UPDATE 1 \
        --INIT_SOL_TYPE 0 \
        --TEMP_UPDATE 3500 \
        --PTL_TEMP_UPDATE_PROPORTION 3 \
		--COSTSWITCH 1 \
		--COSTSWITCHINSTANCE 0 \
		--COSTPRIORITY 30 \
		--PROFITYFINISHED 0

runBeezaoPratica:
	./src/out/mainCpp \
		"./input/BeezaoRaw/IPMTC-II/instanceLarge931_m=6_n=200_l=40_c=30_s=0.PMTC" \
		"" \
		"./output/Beezao/teste.txt" \
		--INSTANCE_REPORT 0 \
		--PRACTITIONER 1 \
		--SEQUENCE_BY 1 \

runPractitioner:
	./src/out/mainCpp \
		"./input/MyInstancesDiferentToolSets/n=15,p=0.5,r=0.5,t=0,v0.csv" \
		"./input/Processed/ToolSetInt.csv" \
		"./output/PractitionerFinal/practitioner.txt" \
		--DIFERENT_TOOLSETS_MODE 1 \
		--INSTANCE_REPORT 0 \
		--PRACTITIONER 1 \
		--SEQUENCE_BY 1 \


goPractitioner:
	@clear
	make devCompile
	make runPractitioner

goModelo:
	@clear
	make normalCompile
	make runModelo

goBeezao:
	@clear
	make compile
# 	make devCompile
	make runBeezao

goPT:
	@clear
# 	make devCompile
	make normalCompile
# 	make runPT
	make runPTDiff

# --------------------------------------------------------
# Meus python
# --------------------------------------------------------

validarFile:
	clear
	python3 ./scripts/reportAnalises.py ./output/Beezao/teste.txt 0
	echo "\n"

tabelaResultadosPractitioner:
	clear
	python3 ./scripts/reportAnalises.py ./output/practitionerFinal/MyInstancesDiferentToolSets 2
	echo "\n"

tabelaResultadosModelo:
	clear
	python3 ./scripts/reportAnalises.py ./output/modeloFinal/MyInstancesDiferentToolSets 3
	echo "\n"

tabelaResultadosPT:
	clear
	python3 ./scripts/reportAnalises.py ./output/diffTesla 4
	echo "\n"

tabelaResultadosComparativa:
	clear
	python3 ./scripts/reportAnalises.py ./output/TCC2V2 5
	echo "\n"

# --------------------------------------------------------
# Update Git
# --------------------------------------------------------

m ?= update

git:
	clear && git add . && git commit -m "$(m)" && git push origin master

