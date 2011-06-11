#!/bin/bash

script_dirname="$(dirname $BASH_SOURCE[-1])"
source "$script_dirname/fc_solve_common.bash"

arg="$1"
shift

if test "$arg" != "--halt" ; then
    echo "First argument to halt.bash must be '--halt'" 1>&2
    exit -1
fi

cd "$FCS_PATH" && perl scripts/parallel-range-solver-total --mode halt
