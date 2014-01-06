#!/bin/bash

# This script parallelises the task of getting the results of a time-consuming
# flare-based scan using GNU parallel.

# To set up:
# mkdir -p ~/Arcs/FC_SOLVE_SUMMARIZE_RESULTS/ on sh.
# mkdir -p ~/Arcs/FC_SOLVE_SUMMARIZE_RESULTS/ on lap.
# sshfs sh:Arcs/FC_SOLVE_SUMMARIZE_RESULTS/ ~/Arcs/FC_SOLVE_SUMMARIZE_RESULTS/ on lap.
# ../Tatzer -l ci7b --prefix="$HOME/apps/fcs" ; make ; make install on sh.
# ../Tatzer -l c2b --prefix="$HOME/apps/fcs" ; make ; make install on lap.

prefix="$1"
shift

seq 0 3199 | parallel --ungroup -j2 \
    bash "$(pwd)/gnu-par-wrapper.bash" "{}" "$prefix" "$@"
