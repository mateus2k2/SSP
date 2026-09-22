#!/usr/bin/env bash
# Runs runExperiment.sh `repeats` times, into run-01 ... run-NN, so results from
# repeated runs of the same config don't overwrite each other.
#
# JOBS and GA_SEED are passed through to runExperiment.sh (see its --help).
#
# Usage:
#   ./scripts/experiments/runRepeated.sh <outputFolder> <runMode> <method> [head] [compileProfile] [licenseFile] [repeats]
#
# See runExperiment.sh for what each argument (besides `repeats`) means.
#
# Example:
#   ./scripts/experiments/runRepeated.sh ./output/SBPOPequenas same pt 9

set -euo pipefail

if [ "${1:-}" = "-h" ] || [ "${1:-}" = "--help" ]; then
    sed -n '2,10p' "$0" | sed 's/^# \{0,1\}//'
    exit 0
fi

outputFolder=${1:-"./output/Multiplos"}
runMode=${2:-"both"}
method=${3:-"pt"}
head=${4:-"9999"}
compileProfile=${5:-"normal"}
licenseFile=${6:-""}
repeats=${7:-10}

for i in $(seq 1 "$repeats")
do
    # run-01 ... run-10, the layout output-final/ uses and the results scripts expect
    runStart=$(date +%s)
    echo "##### $(date '+%Y-%m-%d %H:%M:%S')  run $i of $repeats  ($runMode/$method)"
    "$(dirname "$0")/runExperiment.sh" "$outputFolder/$(printf 'run-%02d' "$i")" \
        "$runMode" "$method" "$head" "$compileProfile" "$licenseFile"
    echo "##### run $i of $repeats finished in $(( ( $(date +%s) - runStart ) / 60 )) min" 
done
