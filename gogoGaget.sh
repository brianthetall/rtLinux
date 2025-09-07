#!/bin/bash

nproc
scripts/isolate.sh
nproc

scripts/runIsolated.sh scripts/runRT.sh bin/rtInfinite
