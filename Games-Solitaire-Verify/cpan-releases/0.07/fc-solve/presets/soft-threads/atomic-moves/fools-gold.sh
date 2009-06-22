#!/bin/sh

# Fool's Gold - a computer generated scan of atomic moves.
# Takes 256,442,916 Iterations.

freecell-solver-range-parallel-solve 1 32000 1 \
--method soft-dfs -to 01ABCDE -step 500 --st-name 0 -nst \
--method random-dfs -to [01][ABCDE] -seed 1 -step 500 --st-name 1 -nst \
--method random-dfs -to [01][ABCDE] -seed 2 -step 500 --st-name 2 -nst \
--method random-dfs -to [01][ABCDE] -seed 3 -step 500 --st-name 3 -nst \
--method random-dfs -to 01[ABCDE] -seed 268 -step 500 --st-name 4 -nst \
--method a-star -to 01ABCDE -step 500 --st-name 5 -nst \
--method a-star -to 01ABCDE -asw 0.2,0.3,0.5,0,0 -step 500 --st-name 6 -nst \
--method a-star -to 01ABCDE -asw 0.5,0,0.5,0,0 -step 500 --st-name 7 -nst \
--method random-dfs -to [01][ABD][CE] -seed 1900 -step 500 --st-name 8 -nst \
--method random-dfs -to [01][ABCDE] -seed 192 -step 500 --st-name 9 -nst \
--method random-dfs -to [01ABCDE] -seed 1977 -step 500 --st-name 10 -nst \
--method random-dfs -to [01ABCDE] -seed 24 -step 500 --st-name 11 -nst \
--method soft-dfs -to 01ABDCE -step 500 --st-name 12 -nst \
--method soft-dfs -to ABC01DE -step 500 --st-name 13 -nst \
--method soft-dfs -to 01EABCD -step 500 --st-name 14 -nst \
--method soft-dfs -to 01BDAEC -step 500 --st-name 15 \
--prelude "1000@0,1000@3,1000@0,1000@9,1000@4,1000@9,1000@3,1000@4,2000@2,1000@0,2000@1,1000@14,2000@11,1000@14,1000@3,1000@11,1000@2,1000@0,2000@4,2000@10,1000@0,1000@2,2000@10,1000@0,2000@11,2000@1,1000@10,1000@2,1000@10,2000@0,1000@9,1000@1,1000@2,1000@14,3000@8,1000@2,1000@14,1000@1,1000@10,3000@6,2000@4,1000@2,2000@0,1000@2,1000@11,2000@6,1000@0,5000@1,1000@0,2000@1,1000@2,3000@3,1000@10,1000@14,2000@6,1000@0,1000@2,2000@11,6000@8,8000@9,3000@1,2000@10,2000@14,3000@15,4000@0,1000@8,1000@10,1000@14,7000@0,14000@2,6000@3,7000@4,1000@8,4000@9,2000@15,2000@6,4000@3,2000@4,3000@15,2000@0,6000@1,2000@4,4000@6,4000@9,4000@14,7000@8,3000@0,3000@1,5000@2,3000@3,4000@9,8000@10,9000@3,5000@8,7000@11,11000@12,12000@0,8000@3,11000@9,9000@15,7000@2,12000@8,16000@5,8000@13,18000@0,9000@15,12000@10,16000@0,14000@3,16000@9,26000@4,23000@3,42000@6,22000@8,27000@10,38000@7,41000@0,42000@3,84000@13,61000@15,159000@5,90000@9"


