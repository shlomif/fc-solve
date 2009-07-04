#!/bin/sh

freecell-solver-range-parallel-solve 1 32000 1 \
        --method random-dfs -seed 1 -to 0[0123456789] -step 500 -nst \
        --method random-dfs -seed 2 -to 0[01][23456789] -step 500 -nst \
        --method random-dfs -seed 3 -to 0[01][23467] -step 500 -nst \
        --method a-star -step 500 -nst \
        --method a-star -to 0123467 -step 500 -nst \
        --method random-dfs -seed 4 -to 0[0123467] -step 500 -nst \


