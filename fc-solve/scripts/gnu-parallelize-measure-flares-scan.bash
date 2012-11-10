#!/bin/bash

# This script parallelises the task of getting the results of a time-consuming
# flare-based scan using GNU parallel.

# To set up:
# mkdir -p ~/Arcs/FC_SOLVE_SUMMARIZE_RESULTS/ on sh.
# mkdir -p ~/Arcs/FC_SOLVE_SUMMARIZE_RESULTS/ on lap.
# sshfs sh:Arcs/FC_SOLVE_SUMMARIZE_RESULTS/ ~/Arcs/FC_SOLVE_SUMMARIZE_RESULTS/ on lap.
# ../Tatzer -l ci7b --prefix="$HOME/apps/fcs" ; make ; make install on sh.
# ../Tatzer -l c2b --prefix="$HOME/apps/fcs" ; make ; make install on lap.

seq 0 319 | parallel --sshlogin 4/sh --sshlogin 2/lap \
    "$HOME"/apps/fcs/bin/freecell-solver-fc-pro-range-solve \
        "\$(({}*100+1))" "\$((({}+1)*100))" 1 \
        -l mfi --flares-choice fcpro \> \
        "$HOME"/Arcs/FC_SOLVE_SUMMARIZE_RESULTS/mfi-flares-choice-fcpro.fc-pro-dump__\$\(printf \"%06d\" \"{}\"\).txt
