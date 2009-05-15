#!/bin/bash
MAX="${1:-6}"
shift

strip *

if ! test -e DUMPS ; then
    mkdir DUMPS
fi

for NUM in $(seq 1 "$MAX") ; do
    echo "Testing $NUM"
    ./freecell-solver-multi-thread-solve 1 32000 500 \
        --num-workers "$NUM" -l gi \
        > "$(printf "DUMPS/dump%.3i" "$NUM")"
done
