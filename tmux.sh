#!/bin/bash

# Define the name for your tmux session
session_name="RT-Processes"

# Check if the session already exists and re-attach to it if so
tmux has-session -t $session_name 2>/dev/null
if [ "$?" -eq 0 ]; then
  echo "Session $session_name already exists. Attaching to it."
  tmux attach-session -t $session_name
  exit 0
fi

# Create a new detached tmux session with a single window
tmux new-session -d -s $session_name -n "dev-env"

# Split the window into multiple panes
# Split the initial pane vertically into two panes (0 and 1)
tmux split-window -v -t "$session_name:dev-env"

# Split the first pane (pane 0) horizontally into two panes (0 and 2)
tmux split-window -h -t "$session_name:dev-env.0"

# Split the second pane (pane 1) horizontally into two panes (1 and 3)
tmux split-window -h -t "$session_name:dev-env.1"

# Select a specific layout to arrange the panes neatly
tmux select-layout -t "$session_name:dev-env" tiled

# Send a different command to each pane (optional)
# Send command to pane 0 (top-left)
tmux send-keys -t "$session_name:dev-env.0" './gogoGaget.sh 99 bin/rtInfinite 200' C-m

# Send command to pane 1 (top-right)
tmux send-keys -t "$session_name:dev-env.1" './gogoGaget.sh 98 bin/rtInfinite 333' C-m

# Send command to pane 2 (bottom-left)
tmux send-keys -t "$session_name:dev-env.2" './gogoGaget.sh 97 bin/rtInfinite 500' C-m

# Send command to pane 3 (bottom-right)
tmux send-keys -t "$session_name:dev-env.3" './gogoGaget.sh 96 bin/rtInfinite 1700' C-m

# Attach to the tmux session
tmux attach-session -t $session_name
