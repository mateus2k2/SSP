#!/usr/bin/env bash
# Runs ./src/out/mainCpp over every instance file in an input folder,
# writing one report per instance under outputFolder.
#
# Usage:
#   ./scripts/experiments/runExperiment.sh <outputFolder> <runMode> <method> [head] [compileProfile] [licenseFile]
#
#   outputFolder     Where to write reports (default ./output/Exemplo)
#   runMode          same | diferent | base | both | beezao   (which instance set(s) to run; default both)
#   method           ga | pt | onb | modelo | practitioner   (--METHOD passed to mainCpp; default pt)
#   head             Max number of instances to run per folder, in filename order (default 9999 = all)
#   compileProfile   normal | tesla | skip   (which `make ...Compile` target to run first; default normal)
#                    "tesla" is NOT a generic environment — it's the exact Gurobi 91 /
#                    old-ABI build used on one specific lab machine. Use "skip" to reuse
#                    whatever binary is already built.
#   licenseFile      Optional path exported as GRB_LICENSE_FILE before compiling/running
#                    (default: ./gurobi/gurobi.lic when it exists)
#
# Environment:
#   JOBS=<n>         Instances to run at the same time. Default: one per core for the
#                    single-threaded methods (ga, practitioner, onb), 1 for pt and modelo,
#                    which use every core themselves — keeping their reported times
#                    comparable with the published runs.
#   GA_SEED=<n>      --GA_SEED for the GA. Default 0, which draws a random seed per run and
#                    records it in the report; set 42 to reproduce the old deterministic runs.
#   FILTER=<regex>   Only run instances whose file name matches this extended regex, e.g.
#                    FILTER='instanceLarge(931|932|933)_' for part of the IPMTC II set.
#
# Examples:
#   ./scripts/experiments/runExperiment.sh ./output/TESTE same pt 9
#   JOBS=8 ./scripts/experiments/runExperiment.sh ./output/practitionerFinal both practitioner 9999
#   ./scripts/experiments/runExperiment.sh ./output/GATeste1 beezao pt 9999
#   ./scripts/experiments/runExperiment.sh ./output/BaseRun base ga 9999
#   FILTER='instanceLarge(931|932|933|946|947|948|952|953|954|958|959|960)_' \
#       ./scripts/experiments/runExperiment.sh ./output/IPMTC12 beezao pt 9999

set -euo pipefail

if [ "${1:-}" = "-h" ] || [ "${1:-}" = "--help" ]; then
    sed -n '2,30p' "$0" | sed 's/^# \{0,1\}//'
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
    same|diferent|base|both|beezao) ;;
    *)
        echo "Error: invalid runMode '$runMode' (expected: same | diferent | base | both | beezao)" >&2
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

# pt and modelo use every core for one instance; the others are single-threaded
case "$method" in
    pt|modelo) defaultJobs=1 ;;
    *)         defaultJobs=$(nproc) ;;
esac
jobs=${JOBS:-$defaultJobs}
gaSeed=${GA_SEED:-0}

[ ! -d "$outputFolder" ] && mkdir -p "$outputFolder"

instaceExtention="csv"
if [ "$runMode" = "beezao" ]; then
    instaceExtention="PMTC"
elif [ "$runMode" = "base" ]; then
    instaceExtention="txt"
fi

# If licenseFile is provided, set the environment variable; otherwise fall back
# to the license shipped with the repo (only --METHOD modelo needs one)
if [ -z "$licenseFile" ] && [ -f ./gurobi/gurobi.lic ]; then
    licenseFile=./gurobi/gurobi.lic
fi
if [ -n "$licenseFile" ]; then
    export GRB_LICENSE_FILE="$licenseFile"
fi

case "$compileProfile" in
    normal) make normalCompile ;;
    tesla)  make teslaCompile ;;
    skip)   echo "Skipping compile, reusing existing ./src/out/mainCpp" ;;
esac

toolSetsFile=./input/Processed/ToolSetInt.csv

# --METHOD plus the parameters that do not depend on the instance set
baseArgs="--METHOD $method"
case "$method" in
    modelo)       baseArgs+=" --INSTANCE_REPORT 0 --TIME_LIMIT 120" ;;   # minutes
    practitioner) baseArgs+=" --INSTANCE_REPORT 0 --SEQUENCE_BY 1" ;;
    ga)           baseArgs+=" --GA_SEED $gaSeed" ;;
    onb)          ;;
esac

if [ "$runMode" = "beezao" ]; then
    baseArgs+=" --COSTSWITCH 1 --COSTSWITCHINSTANCE 0 --COSTPRIORITY 30 --PROFITYFINISHED 0"
fi

# PT was tuned per instance set (irace); these are the settings behind the
# published tables. Only --PTL and four flags differ between the sets.
ptArgs() {
    case "$1" in
        same|base)
            echo "--TEMP_INIT 0.1 --TEMP_FIM 5 --N_REPLICAS 11 --MCL 500 --PASSO_GATILHO 10 \
                  --TEMP_DIST 3 --TYPE_UPDATE 1 --INIT_SOL_TYPE 0 --TEMP_UPDATE 3500 \
                  --PTL_TEMP_UPDATE_PROPORTION 3 --PTL $([ "$1" = base ] && echo 500 || echo 600)" ;;
        diferent)
            echo "--TEMP_INIT 0.1 --TEMP_FIM 10 --N_REPLICAS 11 --MCL 500 --TEMP_DIST 1 \
                  --TYPE_UPDATE 1 --INIT_SOL_TYPE 1 --PTL_TEMP_UPDATE_PROPORTION 5 --PTL 600" ;;
        beezao)
            echo "--TEMP_INIT 0.1 --TEMP_FIM 5 --N_REPLICAS 11 --MCL 500 --PTL 100 --PASSO_GATILHO 10 \
                  --TEMP_DIST 3 --TYPE_UPDATE 1 --INIT_SOL_TYPE 0 --TEMP_UPDATE 3500 \
                  --PTL_TEMP_UPDATE_PROPORTION 3" ;;
    esac
}

# run_instances <instancesFolder> <outputFolder> <toolsetFile> <diferentToolsetsMode> <setName> [outputPrefix]
run_instances() {
    local instancesFolder=$1 outFolder=$2 toolsets=$3 instanceMode=$4 setName=$5 prefix=${6:-""}

    mkdir -p "$outFolder"
    local extraArgs="$baseArgs"
    [ "$method" = "pt" ] && extraArgs+=" $(ptArgs "$setName")"

    echo "RODANDO INSTANCIAS DE $instancesFolder  (jobs=$jobs)"
    echo "  $extraArgs"

    local counter=1 running=0
    for entry in $(ls -v "$instancesFolder"/*."$instaceExtention" \
                   | { [ -n "${FILTER:-}" ] && grep -E "$FILTER" || cat; } \
                   | head -n "$head"); do
        local filename out timestamp
        filename=$(basename "$entry")
        out="$outFolder/$prefix$filename"
        timestamp=$(TZ="America/Sao_Paulo" date "+%Y-%m-%d %H:%M:%S.%3N")
        echo "$timestamp - $counter $entry -> $out"

        ./src/out/mainCpp "$entry" "$toolsets" "$out" \
            --DIFERENT_TOOLSETS_MODE "$instanceMode" $extraArgs >/dev/null 2>&1 \
            || echo "FAILED: $entry" >&2 &

        counter=$((counter+1))
        running=$((running+1))
        if [ "$running" -ge "$jobs" ]; then
            wait -n
            running=$((running-1))
        fi
    done
    wait
}

run_same()     { run_instances ./input/MyInstancesSameToolSets     "$outputFolder/MyInstancesSameToolSets"     "$toolSetsFile" 0 same; }
run_diferent() { run_instances ./input/MyInstancesDiferentToolSets "$outputFolder/MyInstancesDiferentToolSets" "$toolSetsFile" 1 diferent; }
run_beezao()   { run_instances ./input/BeezaoRaw/IPMTC-II          "$outputFolder"                             ""              0 beezao; }
# BaseInstances live in one folder per case; the case name prefixes the report so
# the reports of all three cases can share one output folder (as output-final does)
run_base() {
    local case_
    for case_ in "$(dirname "$0")"/../../input/BaseInstances/*/; do
        case_=$(basename "$case_")
        run_instances "./input/BaseInstances/$case_" "$outputFolder" "" 0 base "${case_}_"
    done
}

case "$runMode" in
    both)     run_same; run_diferent ;;
    same)     run_same ;;
    diferent) run_diferent ;;
    base)     run_base ;;
    beezao)   run_beezao ;;
esac
