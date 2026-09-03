#!/usr/bin/env bash
# Prints line 3 (the DAYS/planning-horizon value -- see the .dat file format
# in scripts/deprecated/consolidateInstances.py's trailing comment) of every
# .dat file in a folder, in natural filename order, with a blank line every
# 3 files.
#
# Usage: ./scripts/runPrint.sh [folder]   (default: ./input/MyInstancesSameToolSets)

set -euo pipefail

dir="${1:-./input/MyInstancesSameToolSets}"
count=0

for file in $(ls -v "$dir"/*.dat 2>/dev/null); do
    [[ -f "$file" ]] || continue

    sed -n '3p' "$file"

    count=$((count + 1))
    if ((count % 3 == 0)); then
        echo
    fi
done
