#!/bin/sh

# This is a meta-moves preset optimised for 2-freecells deals.

freecell-solver-range-parallel-solve 1 32000 1 \
--method soft-dfs -to 0123467 -step 500 --st-name 2 -nst \
--method random-dfs -seed 2 -to 0[01][23456789] -step 500 --st-name 3 -nst \
--method random-dfs -seed 1 -to 0[0123456789] -step 500 --st-name 4 -nst \
--method random-dfs -seed 3 -to 0[01][23467] -step 500 --st-name 5 -nst \
--method a-star -step 500 --st-name 6 -nst \
--method random-dfs -seed 4 -to 0[0123467] -step 500 --st-name 8 -nst \
--method random-dfs -to [01][23456789] -seed 8 -step 500 --st-name 9 -nst \
--method a-star -asw 0.2,0.8,0,0,0 -step 500 --st-name 10 -nst \
--method random-dfs -to [01][23456789] -seed 268 -step 500 --st-name 11 -nst \
--method a-star -asw 0.2,0.3,0.5,0,0 -step 500 --st-name 13 -nst \
--method random-dfs -to [01][23456789] -seed 5 -step 500 --st-name 14 -nst \
--method soft-dfs -to 0126394875 -step 500 --st-name 16 -nst \
--method random-dfs -seed 105 -step 500 --st-name 17 -nst \
--method soft-dfs -to 0123456789 -dto 19,0126394875 -step 500 --st-name 20 -nst \
--method random-dfs -to 0123467 -dto 16,0[123467] -step 500 --st-name 21 -nst \
-to [01][23456789] -dto 27,0123456789 -step 500 --st-name 23 \
--prelude "885@21,746@10,108@21,319@10,315@21,1059@10,375@21,195@10,709@21,915@10,577@21,120@10,116@21,1229@10,692@21,2267@3,598@10,699@21,149@3,281@21,460@3,820@8,591@21,340@3,224@8,879@21,281@3,1394@10,119@21,1596@10,332@8,1750@10,1750@4,1400@10,350@8,1400@17,1400@21,3850@17,700@14,1400@4,350@10,350@4,1050@10,1400@4,1400@5,9450@10,700@14,350@10,700@23,4550@3,350@4,1400@14,2450@21,1400@3,1750@4,350@8,350@16,1400@21,3500@4,1050@6,350@8,350@9,1750@11,350@2,1400@3,5250@5,350@6,700@9,1750@10,2450@13,4200@14,350@17,350@20,1400@10,5250@21"


