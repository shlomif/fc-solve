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

max_iters="${MAX_ITERS:-1500000}"

seq "$start" "$end" |  \
(while read deal ; do
    echo "${deal}:"
    ./board_gen/make_pysol_freecell_board.py $ms -t "$deal" "$variant" |
        ./fc-solve --game "$variant" $theme -p -t -mi "$max_iters" |
        tail -3
done) | tee -a total_dump.txt

