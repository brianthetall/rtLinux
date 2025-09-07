#!/bin/bash

nproc
scripts/isolate.sh
nproc

scripts/runIsolated.sh scripts/runRT.sh $1 $2 # 99 bin/rtInfinite_100us
