#!/usr/bin/env bash
# Runs runAuto.sh `repeats` times, once per numbered subfolder, so results from
# repeated runs of the same config don't overwrite each other.
#
# Usage:
#   ./scripts/runMultiple.sh <outputFolder> <runMode> <method> [head] [compileProfile] [licenseFile] [repeats]
#
# See runAuto.sh for what each argument (besides `repeats`) means.
#
# Example:
#   ./scripts/runMultiple.sh ./output/SBPOPequenas same pt 9

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
    echo "Running $i"
    ./scripts/runAuto.sh "$outputFolder/$i" "$runMode" "$method" "$head" "$compileProfile" "$licenseFile"
done
