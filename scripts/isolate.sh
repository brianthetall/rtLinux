#!/usr/bin/env sh

set -o errexit
set -o nounset

. ${HOME}/repos/rtLinux/scripts/env.sh
. ${SCRIPTS}/rootBumper.sh

#https://documentation.ubuntu.com/real-time/latest/how-to/isolate-workload-cpusets/
doas systemctl set-property --runtime custom-workload.slice AllowedCPUs=0
doas systemctl set-property --runtime init.scope AllowedCPUs=1-3
doas systemctl set-property --runtime system.slice AllowedCPUs=1-3
doas systemctl set-property --runtime user.slice AllowedCPUs=1-3
