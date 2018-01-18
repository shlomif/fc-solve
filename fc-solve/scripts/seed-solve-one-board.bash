#!/bin/bash
d="$1"
shift
seed="$SEED"
echo "Trying seed = $seed deal = $d"
if ./fc-solve --method random-dfs -sp r:tf -to '[0123456789ABCDE]' -seed "$seed" -mi 250000 --freecells-num 3 "$d" | grep -E '^This game is solv' ; then
    echo "Found seed = $seed for deal = $d"
fi
