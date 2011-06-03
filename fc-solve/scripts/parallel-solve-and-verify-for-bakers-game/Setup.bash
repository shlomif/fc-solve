#!/bin/bash

set -x

script_dirname="$(dirname $BASH_SOURCE[-1])"
source "$script_dirname/fc_solve_common.bash"

if ! test -e "$FCS_PATH" ; then
    svn export "$FCS_SOURCE_URL" "$FCS_PATH" && \
    cd "$FCS_PATH" && \
    ./Tatzer -l x64bb --nobuild-docs --prefix="$HOME/fcs-bin" && \
    make && \
    perl scripts/parallel-range-solver-total --mode setup &&
    rm -f range-queue.sock

fi

