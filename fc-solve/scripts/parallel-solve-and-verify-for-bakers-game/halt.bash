#!/bin/bash

script_dirname="$(dirname $BASH_SOURCE[-1])"
source "$script_dirname/fc_solve_common.bash"

cd "$FCS_PATH" && perl scripts/parallel-range-solver-total --mode halt
