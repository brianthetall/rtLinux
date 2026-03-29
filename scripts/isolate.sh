#!/usr/bin/env bash

set -o errexit
set -o nounset

source ${RT}/scripts/env.sh
source ${SCRIPTS}/rootBumper.sh

#https://documentation.ubuntu.com/real-time/latest/how-to/isolate-workload-cpusets/
#doas systemctl set-property --runtime custom-workload.slice AllowedCPUs=0
#doas systemctl set-property --runtime init.scope AllowedCPUs=1-3
#doas systemctl set-property --runtime system.slice AllowedCPUs=1-3
#doas systemctl set-property --runtime user.slice AllowedCPUs=1-3
echo "fix me"
