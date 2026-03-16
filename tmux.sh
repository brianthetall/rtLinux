#!/usr/bin/env bash

# tmux.sh – launch an arbitrary number of RT processes
#  (program-path <cycle-time1> [<cycle-time2> ...])

set -euo pipefail

# -------------------------------------------------------------------
# 1. Basic argument handling
# -------------------------------------------------------------------
if [[ $# -lt 2 ]]; then
  echo "Usage: $0 <program-path> <cycle-time1> [<cycle-time2> ...]"
  exit 1
fi

PROGRAM_PATH="$1"        # <program-path>
shift

# -------------------------------------------------------------------
# 2. Validate cycle‑times
# -------------------------------------------------------------------
for ct in "$@"; do
  # must be a decimal integer
  if ! [[ "$ct" =~ ^[0-9]+$ ]]; then
    echo "Error: cycle‑time '$ct' is not a positive integer."
    exit 1
  fi

  # must be at least 150 µs
  if (( ct < 150 )); then
    echo "Error: cycle‑time $ct µs is below the minimum of 150 µs."
    exit 1
  fi
done

# -------------------------------------------------------------------
# 3. Session name (same as the original script)
# -------------------------------------------------------------------
SESSION="RT-Processes"

# If the session already exists, re‑attach; otherwise create it.
if ! tmux has-session -t "$SESSION" 2>/dev/null; then
  tmux new-session -d -s "$SESSION" -n main
fi

# -------------------------------------------------------------------
# 4. Start processes – one pane per cycle‑time
# -------------------------------------------------------------------
#  Priority is hard‑coded to 99 as in the original `gogoGaget.sh`
#  call [2].
priority=99

# Launch the first instance in the window that just got created.
first_cycleTime="$1"
tmux send-keys -t "$SESSION" "./gogoGaget.sh $priority $PROGRAM_PATH $first_cycleTime" C-m
((priority--))   # next pane gets one priority lower

# For every remaining cycle‑time, split the window vertically
# and launch another instance.
shift
while (( $# > 0 )); do
  cycleTime="$1"
  tmux split-window -t "$SESSION" -v   # new pane below the previous one
  tmux send-keys -t "$SESSION" "./gogoGaget.sh $priority $PROGRAM_PATH $cycleTime" C-m
  ((priority--))   # next pane gets one priority lower
  shift
done

# -------------------------------------------------------------------
# 4. Bring the user into the session
# -------------------------------------------------------------------
tmux attach-session -t "$SESSION"
