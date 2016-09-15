#!/bin/sh
# This scan is derived from ct-mod13.sh in scripts/TEST_OPTIMIZATIONS .
# Its name is derived from https://www.youtube.com/watch?v=DNq1bkrWjbk in
# memory of Christina Grimmie.
freecell-solver-range-parallel-solve 1 32000 1 \
--method random-dfs -seed 2 -to 0[01][23456789] -step 500 -sp r:tf --st-name 3 -nst \
--method random-dfs -seed 3 -to "0[1]=asw(1)[23467]=asw(1)" -step 500 -sp r:tf --st-name 5 -nst \
--method random-dfs -seed 4 -to 0[0123467] -step 500 -sp r:tf --st-name 9 -nst \
--method random-dfs -to [01][23456789] -seed 8 -step 500 -sp r:tf --st-name 10 -nst \
--method a-star -asw 0.2,0.8,0,0,0 -step 500 -sp r:tf --st-name 11 -nst \
--method random-dfs -to "[01]=asw(1)[23456789]=asw(1)" -seed 268 -step 500 -sp r:tf --st-name 12 -nst \
--method a-star -to 0123467 -asw 0.5,0,0.3,0,0 -step 500 -sp r:tf --st-name 18 -nst \
--method random-dfs -seed 105 -step 500 -sp r:tf --st-name 20 -nst \
--method random-dfs -to 0123467 -dto "16,0[123467]=asw(1,0,0,0,0,0)" -step 500 -sp r:tf --st-name 24 -nst \
--method random-dfs -to 01234567 -dto "13,[0187465]=asw(1)" -dto "26,[0134587]=asw(1)" -step 500 -sp r:tf --st-name 32 -nst \
--method random-dfs -to "0123" -dto2 "6,01[2435]=asw(3,20,10,0,1)" -step 500 -sp r:tf --st-name new -nst \
--method random-dfs -to "[01][23468]" -seed 1547 -step 500 -sp r:tf --st-name s1 -nst \
--method random-dfs -to "[01][23457]" -dto2 "8,[0123][4567]" -seed 38693 -step 500 -sp r:tf --st-name sB -nst \
--method random-dfs -to "[01][23457]" -dto2 "7,[0123][4567]" -seed 98361 -step 500 -sp r:tf --st-name sD -nst \
--method random-dfs -to "[01][23457]" -dto2 "7,[0123][4567]" -seed 115235 -step 500 -sp r:tf --st-name sE -nst \
--method random-dfs -to "[01][23457]" -dto2 "7,[0123][4567]" -seed 44250 -step 500 -sp r:tf --st-name sF -nst \
--method random-dfs -to "[01][23457]" -dto2 "5,[0123][4567]"  -seed 33627 -step 500 -sp r:tf --st-name sG -nst \
--method random-dfs -to "[01][23457]" -dto2 "5,[0123][4567]" -seed 18296 -step 500 -sp r:tf --st-name sH -nst \
--method random-dfs -to "[01][23457]" -dto2 "10,[0123][4567]" -seed 10468 -step 500 -sp r:tf --st-name sI \
--prelude "380@24,380@5,380@9,414@sI,366@sF,490@sH,428@sG,760@32,380@12,380@24,380@10,380@5,200@s1,200@new,222@sE,225@sD,266@sB,380@3,380@9,380@24,380@18,760@20,380@11,760@24,380@11"
