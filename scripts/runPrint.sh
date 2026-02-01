#!/bin/bash

# Directory containing the files (change as needed)
dir="/home/mateus/WSL/IC/SSP/input/MyInstancesSameToolSets"

# Counter for tracking every 3 files
count=0

# Loop through .dat files sorted in natural order
for file in $(ls -v "$dir"/*.dat 2>/dev/null); do
    # Check if the file exists (in case there are no .dat files)
    if [[ -f "$file" ]]; then
        # Print the third line if it exists
        sed -n '3p' "$file"

        # Increment counter
        ((count++))

        # Print a blank line every 3 files
        if ((count % 3 == 0)); then
            echo
        fi
    fi
done
