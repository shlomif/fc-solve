#!/bin/bash

set -x

script_dirname="$(dirname $BASH_SOURCE[-1])"
source "$script_dirname/fc_solve_common.bash"

if ! test -e "$FCS_PATH" ; then
    svn export http://svn.berlios.de/svnroot/repos/fc-solve/fc-solve/trunk/fc-solve/source "$FCS_PATH" 
    cd "$FCS_PATH"
    ./Tatzer -l x64bb --nobuild-docs --prefix="$HOME/fcs-bin"
    make
    perl scripts/parallel-range-solver-total --mode setup
    rm -f range-queue.sock

fi

