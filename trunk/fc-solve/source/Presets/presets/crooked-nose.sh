#!/bin/sh

# "Crooked Nose"
# This is the first atomic moves preset that was recorded.
# It can solve the MS 32000 in 275,662,296 iterations.


freecell-solver-range-parallel-solve 1 32000 1 \
--method soft-dfs -to 01ABCDE -step 500 --st-name 0 -nst \
--method random-dfs -to [01][ABCDE] -seed 1 -step 500 --st-name 1 -nst \
--method random-dfs -to [01][ABCDE] -seed 2 -step 500 --st-name 2 -nst \
--method random-dfs -to [01][ABCDE] -seed 3 -step 500 --st-name 3 -nst \
--method random-dfs -to 01[ABCDE] -seed 268 -step 500 --st-name 4 -nst \
--method a-star -to 01ABCDE -step 500 --st-name 5 -nst \
--method a-star -to 01ABCDE -asw 0.2,0.3,0.5,0,0 -step 500 --st-name 6 -nst \
--method random-dfs -to [01][ABCDE] -seed 192 -step 500 --st-name 9 -nst \
--method random-dfs -to [01ABCDE] -seed 1977 -step 500 --st-name 10 -nst \
--method random-dfs -to [01ABCDE] -seed 24 -step 500 --st-name 11 \
--prelude "3000@0,3000@11,3000@2,3000@9,3000@10,3000@3,3000@4,3000@0,3000@1,3000@10,3000@11,3000@2,3000@0,3000@1,3000@6,3000@1,3000@3,3000@9,3000@2,3000@4,3000@0,3000@4,3000@3,3000@6,3000@9,3000@3,3000@4,6000@9,12000@10,6000@1,3000@6,6000@3,3000@6,3000@9,6000@3,3000@6,9000@11,6000@1,12000@0,12000@3,3000@4,6000@9,6000@1,33000@2,12000@9,9000@11,6000@0,6000@4,6000@2,24000@4,9000@6,45000@0,9000@1,21000@3,9000@4,21000@5,33000@6,18000@3,21000@9,42000@10,42000@0,42000@3,66000@2,123000@3,129000@6,81000@4,132000@5,165000@11"


