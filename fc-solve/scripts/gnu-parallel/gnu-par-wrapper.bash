#!/bin/bash

i="$1"
shift
prefix="$1"
shift

fn="$HOME/Arcs/FC_SOLVE_SUMMARIZE_RESULTS/${prefix}__$(printf "%06d" "$i").txt"
tfn="$fn.temp"

if ! test -e "$fn" ; then
    nice -19 "$HOME"/apps/fcs/bin/freecell-solver-fc-pro-range-solve \
        "$(($i*10+1))" "$((($i+1)*10))" 1 \
        "$@" \
        > "$tfn"
    mv -f "$tfn" "$fn"
fi
