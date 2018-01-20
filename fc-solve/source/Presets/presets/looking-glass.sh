#!/bin/sh
# This scan is derived from the cm scan.
freecell-solver-range-parallel-solve 1 32000 1 \
--method random-dfs -to 01j234 -dto2 "33,016[234j]=asw(1)" -step 500 -sp r:tf --st-name 24 -nst \
--method random-dfs -to "0[124]=asw(1)j[36]=asw(1)" -dto2 "55,016[234j]=asw(1)" -step 500 -sp r:tf --st-name 5 -nst \
--method random-dfs -seed 6 -to 0j26[34] -step 500 -sp r:tf --st-name 9 -nst \
--method random-dfs -to "012j[35]" -dto2 "9,[012]j[468]" -dto2 "15,[012346j8]=asw(1)" -seed 10468 -step 500 -sp r:tf --st-name sI -nst \
--method random-dfs -to "01[2345j]" -dto2 "9,012[456j]" -seed 44250 -step 500 -sp r:tf --st-name sF -nst \
--method random-dfs -to "[01][2345j]" -dto2 "5,[0123][456j]" -dto2 "13,[0123456j]=asw(1)" -seed 18296 -step 500 -sp r:tf --st-name sH -nst \
--method random-dfs -to "[01][2345j]" -dto2 "5,[0123][456j]" -dto2 "13,[0123456j]=asw(1)" -seed 33627 -step 500 -sp r:tf --st-name sG -nst \
--method random-dfs -to 0123456j -dto2 "13,01[8j465]=asw(1)" -dto2 "26,26[013458j]=asw(1)" -step 500 -sp r:tf --st-name 32 -nst \
--method random-dfs -to "[01]=asw(1)[23456j89]=asw(1)" -step 500 -sp r:tf --st-name 12 -nst \
--method random-dfs -to "0123" -dto2 "6,01[2435]=asw(3,20,10,0,1)" -dto2 "13,[0123456j]=asw(1)" -step 500 -sp r:tf --st-name new -nst \
--method random-dfs -to "[01][2345j]" -dto2 "7,[0123][456j]" -seed 98361 -step 500 -sp r:tf --st-name sD -nst \
--method random-dfs -to [01][23456j89] -seed 8 -step 500 -sp r:tf --st-name 10 -nst \
--method a-star -asw 0.2,0.8,0,0,0 -step 500 -sp r:tf --st-name 11 -nst \
--method a-star -to 012346j -asw 0.5,0,0.3,0,0 -step 500 -sp r:tf --st-name 18 -nst \
--method random-dfs -seed 105 -step 500 -sp r:tf --st-name 20 -nst \
--method random-dfs -to "[01][2345j]" -dto2 "8,[0123][456j]" -seed 38693 -step 500 -sp r:tf --st-name sB \
--prelude "245@24,230@5,280@9,412@sI,400@sF,490@sH,428@sG,760@32,380@12,281@24,380@10,76@5,200@new,225@sD,266@sB,380@24,760@20,380@11,760@24,380@11"
