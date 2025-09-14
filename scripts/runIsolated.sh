#!/bin/bash
#https://documentation.ubuntu.com/real-time/latest/how-to/isolate-workload-cpusets/

#systemd-run --user --scope -p Slice=custom-workload.slice $1 $2 $3 $4
systemd-run --system --scope -p Slice=custom-workload.slice $1 $2 $3 $4
