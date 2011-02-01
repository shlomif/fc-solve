#!/bin/sh

freecell-solver-range-parallel-solve 1 32000 1 \
    --method random-dfs -to [01][23456789] -seed 1 -step 500 -nst \
    --method random-dfs -to [01][23456789] -seed 2 -step 500 -nst \
    --method random-dfs -to [01][23456789] -seed 3 -step 500 -nst \
    --method random-dfs -to [01][23456789] -seed 4 -step 500 -nst \
    --method random-dfs -to [01][23456789] -seed 5 -step 500 -nst \
    --method random-dfs -to [01][23456789] -seed 6 -step 500 -nst \
    --method random-dfs -to [01][23456789] -seed 7 -step 500 -nst \
    --method random-dfs -to [01][23456789] -seed 8 -step 500 -nst \
    --method random-dfs -to [01][23456789] -seed 9 -step 500 -nst \
    --method random-dfs -to [01][23456789] -seed 10 -step 500 
    
