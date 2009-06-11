#!/bin/bash

RUN_SERIAL=false

PARAM1="$1"
shift

if test "$PARAM1" = "-s" ; then
    RUN_SERIAL=true
    PARAM1="$1"
    shift
fi

PARAM2="$1"
shift

MIN=1
MAX=6

if ! test -z "$PARAM2" ; then
    MIN="$PARAM1"
    MAX="$PARAM2"
elif ! test -z "$PARAM1" ; then
    MAX="$PARAM1"
fi

strip * > /dev/null 2>&1

if ! test -e DUMPS ; then
    mkdir DUMPS
fi

p_dir="__p"
if ! test -e "$p_dir" ; then
    cp -R Presets "$p_dir"
    
    P="$(cd "$p_dir"/presets && pwd)" \
        perl -lpi -e 's!\A(dir=).*\z!$1$ENV{P}/!ms' ./"$p_dir"/presetrc
fi

export FREECELL_SOLVER_PRESETRC="$(ls $(pwd)/"$p_dir"/presetrc)"

if $RUN_SERIAL ; then
    ./freecell-solver-range-parallel-solve 1 32000 500 -l gi > dump
fi

for NUM in $(seq "$MIN" "$MAX") ; do
    echo "Testing $NUM"
    ./freecell-solver-multi-thread-solve 1 32000 4000 \
        --iters-update-on 10000000 \
        --num-workers "$NUM" \
        -l gi \
        > "$(printf "DUMPS/dump%.3i" "$NUM")"
done
