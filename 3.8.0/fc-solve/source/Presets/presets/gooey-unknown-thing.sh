freecell-solver-range-parallel-solve 1 32000 1 \
--method soft-dfs -to 0123456789 -step 500 --st-name 1 -nst \
--method soft-dfs -to 0123467 -step 500 --st-name 2 -nst \
--method random-dfs -seed 1 -to 0[0123456789] -step 500 --st-name 4 -nst \
--method random-dfs -seed 3 -to 0[01][23467] -step 500 --st-name 5 -nst \
--method random-dfs -seed 4 -to 0[0123467] -step 500 --st-name 9 -nst \
--method a-star -asw 0.2,0.8,0,0 -step 500 --st-name 11 -nst \
--method a-star -asw 0.2,0.3,0.5,0,0 -step 500 --st-name 16 -nst \
--method random-dfs -to [01][23456789] -seed 5 -step 500 --st-name 17 -nst \
--method a-star -to 0123467 -asw 0.5,0,0.3,0,0 -step 500 --st-name 18 \
--prelude "350@18,350@16,11200@18,350@11,1050@16,350@9,350@5,2800@18,1050@16,350@18,1400@16,350@9,1400@11,700@9,1050@5,350@2,1400@5,350@9,350@2,700@9,700@2,2800@9,700@4,700@17,700@1,1050@11,1750@5,24150@1,35000@18,36750@16"


