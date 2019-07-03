#!/bin/bash

start="$1"
shift
end="$1"
shift

variant="${VARIANT:-simple_simon}"
ms=''
if test -n "${MS}"; then
    ms='--ms'
fi

theme=''
if test "$variant" = "simple_simon" ; then
    theme='-ni -to abcdefghi'
fi

if test -n "${THEME}" ; then
    theme="${THEME}"
fi

deals_dir="run-variant-sequence--deals"
if ! test -d "$deals_dir"
then
    mkdir -p "$deals_dir"
    gen-multiple-pysol-layouts --dir "$deals_dir" --ms --prefix '' --suffix .board --game "$variant" seq 1 100000
fi

max_iters="${MAX_ITERS:-1500000}"

export FREECELL_SOLVER_QUIET=1

time ./fc-solve-multi --game "$variant" $theme -sel -sam -p -t -mi "$max_iters" $(eval echo "$deals_dir"/{$start..$end}.board) \
 | tee -a total_dump.txt
