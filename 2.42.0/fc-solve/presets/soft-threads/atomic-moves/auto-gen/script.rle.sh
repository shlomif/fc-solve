freecell-solver-range-parallel-solve 1 32000 1 \
--method soft-dfs -to 0123456789 -step 500 --st-name 1 -nst \
--method soft-dfs -to 0123467 -step 500 --st-name 2 -nst \
--method random-dfs -seed 2 -to 0[01][23456789] -step 500 --st-name 3 -nst \
--method random-dfs -seed 1 -to 0[0123456789] -step 500 --st-name 4 -nst \
--method random-dfs -seed 3 -to 0[01][23467] -step 500 --st-name 5 -nst \
--method random-dfs -seed 4 -to 0[0123467] -step 500 --st-name 9 -nst \
--method random-dfs -to [01][23456789] -seed 8 -step 500 --st-name 10 \
--prelude "200@2,200@9,200@2,200@5,200@9,200@5,200@2,200@5,200@9,400@4,400@10,200@2,400@5,200@9,200@10,400@3,200@5,400@2,200@3,200@5,200@10,200@3,400@2,800@5,200@10,200@4,200@9,600@2,600@5,800@9,200@10,200@5,400@1,800@2,200@5,400@9,400@1,1400@2,400@9,1000@2,2600@3,1000@10,600@4,1200@3,1400@5,1200@4,1000@5,800@10,1000@1,1200@2,2400@3,2600@5,1600@10,5400@2,5800@1,14000@9,16200@2,21600@9"


