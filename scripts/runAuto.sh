#!/usr/bin/env bash
# Runs ./src/out/mainCpp over every instance file in an input folder, in order,
# writing one report per instance under outputFolder.
#
# Usage:
#   ./scripts/runAuto.sh <outputFolder> <runMode> <method> [head] [compileProfile] [licenseFile]
#
#   outputFolder     Where to write reports (default ./output/Exemplo)
#   runMode          same | diferent | both | beezao   (which instance set(s) to run; default both)
#   method           ga | pt | onb | modelo | practitioner   (--METHOD passed to mainCpp; default pt)
#   head             Max number of instances to run per folder, in filename order (default 9999 = all)
#   compileProfile   normal | tesla | skip   (which `make ...Compile` target to run first; default normal)
#                    "tesla" is NOT a generic environment — it's the exact Gurobi 91 /
#                    old-ABI build used on one specific lab machine. Use "skip" to reuse
#                    whatever binary is already built.
#   licenseFile      Optional path exported as GRB_LICENSE_FILE before compiling/running
#
# Examples:
#   ./scripts/runAuto.sh ./output/TESTE same pt 9
#   ./scripts/runAuto.sh ./output/practitionerFinal both practitioner 9999
#   ./scripts/runAuto.sh ./output/GATeste1 beezao pt 9999
#   ./scripts/runAuto.sh ./output/BeezaoAuto beezao practitioner 9999

set -euo pipefail

if [ "${1:-}" = "-h" ] || [ "${1:-}" = "--help" ]; then
    sed -n '2,20p' "$0" | sed 's/^# \{0,1\}//'
    exit 0
fi

outputFolder=${1:-"./output/Exemplo"}
runMode=${2:-"both"}
method=${3:-"pt"}
head=${4:-"9999"}
compileProfile=${5:-"normal"}
licenseFile=${6:-""}

# normalize case so old callers used to "PT"/"Beezao"/etc. still work
runMode="${runMode,,}"
method="${method,,}"
compileProfile="${compileProfile,,}"

case "$runMode" in
    same|diferent|both|beezao) ;;
    *)
        echo "Error: invalid runMode '$runMode' (expected: same | diferent | both | beezao)" >&2
        exit 1
        ;;
esac

case "$method" in
    ga|pt|onb|modelo|practitioner) ;;
    *)
        echo "Error: invalid method '$method' (expected: ga | pt | onb | modelo | practitioner)" >&2
        exit 1
        ;;
esac

case "$compileProfile" in
    normal|tesla|skip) ;;
    *)
        echo "Error: invalid compileProfile '$compileProfile' (expected: normal | tesla | skip)" >&2
        exit 1
        ;;
esac

[ ! -d "$outputFolder" ] && mkdir -p "$outputFolder"
[ ! -d "$outputFolder/MyInstancesSameToolSets" ] && mkdir -p "$outputFolder/MyInstancesSameToolSets"
[ ! -d "$outputFolder/MyInstancesDiferentToolSets" ] && mkdir -p "$outputFolder/MyInstancesDiferentToolSets"

instaceExtention="csv"
if [ "$runMode" = "beezao" ]; then
    instaceExtention="PMTC"
fi

# If licenseFile is provided, set the environment variable
if [ -n "$licenseFile" ]; then
    export GRB_LICENSE_FILE="$licenseFile"
fi

case "$compileProfile" in
    normal) make normalCompile ;;
    tesla)  make teslaCompile ;;
    skip)   echo "Skipping compile, reusing existing ./src/out/mainCpp" ;;
esac

run_instances() {
    local instancesFolder=$1
    local outputFolder=$2
    local instanceMode=$3

    echo "RODANDO INSTANCIAS DE $instancesFolder"
    local counter=1
    for entry in $(ls -v "$instancesFolder"/*."$instaceExtention" | head -n "$head");
    do
        filename=$(basename "$entry")
        local timestamp
        timestamp=$(TZ="America/Sao_Paulo" date "+%Y-%m-%d %H:%M:%S.%3N")
        echo "$timestamp - $counter ./src/out/mainCpp $instancesFolder/$filename $toolSetsFile $outputFolder/$filename"
        ./src/out/mainCpp "$instancesFolder/$filename" "$toolSetsFile" "$outputFolder/$filename" --DIFERENT_TOOLSETS_MODE "$instanceMode" $extraArgs
        counter=$((counter+1))
    done
}

toolSetsFile=./input/Processed/ToolSetInt.csv
extraArgs="--METHOD $method"

case "$method" in
    pt)
        extraArgs+=" \
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
        "
        ;;
    modelo)
        extraArgs+=" \
            --INSTANCE_REPORT 0 \
            --TIME_LIMIT 120 \
        "
        ;;
    practitioner)
        extraArgs+=" \
            --INSTANCE_REPORT 0 \
            --SEQUENCE_BY 1 \
        "
        ;;
    ga|onb)
        ;; # no extra params, defaults are fine
esac

if [ "$runMode" = "beezao" ]; then
    extraArgs+=" \
        --COSTSWITCH 1 \
        --COSTSWITCHINSTANCE 0 \
        --COSTPRIORITY 30 \
        --PROFITYFINISHED 0 \
    "
fi

echo "$extraArgs"

case "$runMode" in
    both)
        run_instances ./input/MyInstancesSameToolSets      "$outputFolder/MyInstancesSameToolSets"      0
        run_instances ./input/MyInstancesDiferentToolSets  "$outputFolder/MyInstancesDiferentToolSets"  1
        ;;
    same)
        run_instances ./input/MyInstancesSameToolSets "$outputFolder/MyInstancesSameToolSets" 0
        ;;
    diferent)
        run_instances ./input/MyInstancesDiferentToolSets "$outputFolder/MyInstancesDiferentToolSets" 1
        ;;
    beezao)
        run_instances ./input/BeezaoRaw/IPMTC-II "$outputFolder" 0
        ;;
esac
