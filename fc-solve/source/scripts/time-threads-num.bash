#!/bin/bash

RUN_SERIAL=false
PROG="${PROG:-./freecell-solver-multi-thread-solve}"

while getopts "sp" flag ; do
    # Run the serial scan first.
    if   [ "$flag" = "s" ] ; then
        RUN_SERIAL=true
    # Run the multi-process version instead of the multi-threaded version.
    elif [ "$flag" = "p" ] ; then
        PROG="./freecell-solver-fork-solve"
    fi
done

while [ $OPTIND -ne 1 ] ; do
    shift
    let OPTIND--
done

MIN="${1:-1}"
shift

MAX="${1:-6}"
shift

OUT_DIR="${OUT_DIR:-.}"
ARGS="${ARGS:--l gi}"

strip * > /dev/null 2>&1

mkdir -p "$OUT_DIR/DUMPS"

p_dir="__p"
if ! test -e "$p_dir" ; then
    cp -R Presets "$p_dir"
    
    P="$(cd "$p_dir"/presets && pwd)" \
        perl -lpi -e 's!\A(dir=).*\z!$1$ENV{P}/!ms' ./"$p_dir"/presetrc
fi

export FREECELL_SOLVER_PRESETRC="$(ls $(pwd)/"$p_dir"/presetrc)"

if $RUN_SERIAL ; then
    echo "Testing Serial Run"
    ./freecell-solver-range-parallel-solve 1 32000 500 $ARGS > "$OUT_DIR"/dump
fi

for NUM in $(seq "$MIN" "$MAX") ; do
    echo "Testing $NUM"
    $PROG 1 32000 4000 \
        --iters-update-on 10000000 \
        --num-workers "$NUM" \
        $ARGS \
        > "$(printf "$OUT_DIR/DUMPS/dump%.3i" "$NUM")"
done

