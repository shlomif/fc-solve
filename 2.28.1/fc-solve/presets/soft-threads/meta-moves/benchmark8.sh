#!/bin/sh
freecell-solver-range-parallel-solve 1 32000 1 \
    --method random-dfs -seed 1 -to 0[123456789] -nst \
    --method random-dfs -seed 2 -to 0[1][23456789] -nst \
    --method random-dfs -seed 3 -to 0[1][23467] -nst \
    --method a-star -nst \
    --method a-star -to 0123467 -nst \
    --method random-dfs -seed 4 -to 0[123467] -nst \
    --method soft-dfs -nst


    #--method random-dfs -seed 24 -to 01[234679] -nst \
    #--method soft-dfs -to 0123467 -nst \
    #--method random-dfs -seed 5 -to 012[789][3456]

