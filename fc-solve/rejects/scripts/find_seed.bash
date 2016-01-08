#!/bin/bash

seed=1

min_seed=""
min_num_times=10000000

while true; do

    num_times="$(pi-make-microsoft-freecell-board 12 | ./fc-solve --freecells-num 2 -to '[012][347]' --method random-dfs -seed "$seed" | grep -P '^Total number of states checked is' | grep -Po '\d+')"

    if test "$num_times" -lt "$min_num_times" ; then
        min_seed="$seed"
        min_num_times="$num_times"
    fi

    echo "min = $min_seed , $min_num_times"
    let seed++
done
