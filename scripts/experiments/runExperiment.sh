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

# Runs one instance and reports when it FINISHES, with its own wall time, so the
# log stays readable with several instances in flight. Exported for xargs below.
# (Instance file names contain no spaces, which is what lets xargs split on them.)
run_one() {
    local idx=$1 entry=$2
    local filename out t0
    filename=$(basename "$entry")
    out="$RI_OUT/$RI_PREFIX$filename"
    t0=$(date +%s)
    if ./src/out/mainCpp "$entry" "$RI_TOOLSETS" "$out" \
           --DIFERENT_TOOLSETS_MODE "$RI_MODE" $RI_ARGS >/dev/null 2>&1; then
        printf '%s [%s/%s] %-42s %5ds\n' "$(date +%H:%M:%S)" "$idx" "$RI_TOTAL" "$filename" "$(( $(date +%s) - t0 ))"
    else
        printf '%s [%s/%s] %-42s FAILED\n' "$(date +%H:%M:%S)" "$idx" "$RI_TOTAL" "$filename" >&2
    fi
}
export -f run_one

# run_instances <instancesFolder> <outputFolder> <toolsetFile> <diferentToolsetsMode> <setName> [outputPrefix]
run_instances() {
    local instancesFolder=$1 outFolder=$2 toolsets=$3 instanceMode=$4 setName=$5 prefix=${6:-""}

    mkdir -p "$outFolder"
    local extraArgs="$baseArgs"
    [ "$method" = "pt" ] && extraArgs+=" $(ptArgs "$setName")"

    local entries total setStart
    # awk, not head: head exits early, which kills `ls` with SIGPIPE, and under
    # `set -o pipefail` that aborts the whole script (silently, for big folders)
    entries=$(ls -v "$instancesFolder"/*."$instaceExtention" \
              | { [ -n "${FILTER:-}" ] && grep -E "$FILTER" || cat; } \
              | awk -v n="$head" 'NR <= n')
    total=$(printf '%s\n' "$entries" | grep -c . || true)
    setStart=$(date +%s)

    echo "=== $instancesFolder -> $outFolder"
    echo "    $total instances, $jobs at a time, method $method"
    echo "    $extraArgs"

    # xargs -P throttles the parallel jobs; bash's `wait -n` would need bash 4.3,
    # which the lab machine does not have.
    export RI_OUT="$outFolder" RI_PREFIX="$prefix" RI_TOOLSETS="$toolsets" \
           RI_MODE="$instanceMode" RI_ARGS="$extraArgs" RI_TOTAL="$total"
    printf '%s\n' "$entries" | grep -v '^$' | nl -ba \
        | xargs -P "$jobs" -L1 bash -c 'run_one "$0" "$1"'

    local written elapsed
    written=$(ls "$outFolder" | wc -l)
    elapsed=$(( $(date +%s) - setStart ))
    printf '    set finished in %dh%02dm (%s now holds %d reports)\n\n' \
        $(( elapsed / 3600 )) $(( elapsed % 3600 / 60 )) "$outFolder" "$written"
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
