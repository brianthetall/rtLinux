#!/usr/bin/env sh

set -o errexit
set +o nounset

if [ -z "$RT" ]; then
    printf "RT not set"
    export RT="../"
fi

set -o nounset
export SCRIPTS=${RT}/scripts
