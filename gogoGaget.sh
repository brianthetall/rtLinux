#!/bin/bash

# gogoGaget.sh  – updated version with argument validation
# ------------------------------------------------------------------
# 1.  Verify that exactly three arguments are supplied
# ------------------------------------------------------------------
if [[ $# -ne 3 ]]; then
    echo "Usage: $0 <priority (1-99)> <program> <cycleTime [us]>"
    exit 1
fi

# ------------------------------------------------------------------
# 2.  Assign arguments to named variables for clarity
# ------------------------------------------------------------------
priority=$1
program=$2
cycleTime=$3

# ------------------------------------------------------------------
# 3.  Validate <priority>
# ------------------------------------------------------------------
if ! [[ $priority =~ ^[1-9][0-9]?$ ]]; then
    echo "Error: priority must be an integer between 1 and 99."
    exit 1
fi
if (( priority < 1 || priority > 99 )); then
    echo "Error: priority must be between 1 and 99."
    exit 2
fi

# ------------------------------------------------------------------
# 4.  Validate <program>
# ------------------------------------------------------------------
if [[ -z "$program" ]]; then
    echo "Error: program name must be a non‑empty string."
    exit 3
fi

# ------------------------------------------------------------------
# 5.  Validate <cycleTime>
# ------------------------------------------------------------------
if ! [[ $cycleTime =~ ^[1-9][0-9]*$ ]]; then
    echo "Error: cycleTime must be a positive integer."
    exit 4
fi

# ------------------------------------------------------------------
# 6.  
# ------------------------------------------------------------------
nproc
scripts/isolate.sh
nproc
scripts/runIsolated.sh scripts/runRT.sh "$priority" "$program" "$cycleTime"
