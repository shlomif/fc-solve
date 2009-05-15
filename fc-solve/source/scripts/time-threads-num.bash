#!/bin/bash
MAX="${1:-6}"
shift

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
for NUM in $(seq 1 "$MAX") ; do
    echo "Testing $NUM"
    ./freecell-solver-multi-thread-solve 1 32000 500 \
        --num-workers "$NUM" -l gi \
        > "$(printf "DUMPS/dump%.3i" "$NUM")"
done
