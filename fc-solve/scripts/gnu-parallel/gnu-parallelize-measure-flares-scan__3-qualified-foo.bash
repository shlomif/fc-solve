#!/bin/bash

# This script parallelises the task of getting the results of a time-consuming
# flare-based scan using GNU parallel.

# To set up:
# mkdir -p ~/Arcs/FC_SOLVE_SUMMARIZE_RESULTS/ on sh.
# mkdir -p ~/Arcs/FC_SOLVE_SUMMARIZE_RESULTS/ on lap.
# sshfs sh:Arcs/FC_SOLVE_SUMMARIZE_RESULTS/ ~/Arcs/FC_SOLVE_SUMMARIZE_RESULTS/ on lap.
# ../Tatzer -l ci7b --prefix="$HOME/apps/fcs" ; make ; make install on sh.
# ../Tatzer -l c2b --prefix="$HOME/apps/fcs" ; make ; make install on lap.

bash gnu-parallelize-measure-flares-scan__4__GENERIC.bash "qualified-foo-flares-choice-fc_solve.fc-pro-dump" \
    --read-from-file "4,/home/shlomif/progs/freecell/git/fc-solve/fc-solve/source/Presets/presets/qualified-foo.sh" --flares-choice fc_solve -fif 5
