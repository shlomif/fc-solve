# This scan is based on as-modified-3.sh only it contains the new scans:
#   --st-name s5 - for solving 3294 - generated using find-min-seed-for-3294.pl
#
# They were placed here in strategic places.
freecell-solver-range-parallel-solve 1 32000 1 \
--method random-dfs -seed 2 -to 0[01][23456789] -step 500 -sp r:tf --st-name 3 -nst \
--method random-dfs -seed 1 -to 0[0123456789] -step 500 -sp r:tf --st-name 4 -nst \
--method random-dfs -seed 3 -to "0[1]=asw(1)[23467]=asw(1)" -step 500 -sp r:tf --st-name 5 -nst \
--method random-dfs -seed 4 -to 0[0123467] -step 500 -sp r:tf --st-name 9 -nst \
--method random-dfs -to [01][23456789] -seed 8 -step 500 -sp r:tf --st-name 10 -nst \
--method a-star -asw 0.2,0.8,0,0,0 -step 500 -sp r:tf --st-name 11 -nst \
--method random-dfs -to "[01]=asw(1)[23456789]=asw(1)" -seed 268 -step 500 -sp r:tf --st-name 12 -nst \
--method random-dfs -to [0123456789] -seed 142 -step 500 -sp r:tf --st-name 15 -nst \
--method a-star -to 0123467 -asw 0.5,0,0.3,0,0 -step 500 -sp r:tf --st-name 18 -nst \
--method random-dfs -seed 105 -step 500 -sp r:tf --st-name 20 -nst \
--method a-star -asw 0.5,0,0.5,0,0 -step 500 -sp r:tf --st-name 21 -nst \
--method soft-dfs -to 013[2456789] -step 500 -sp r:tf --st-name 22 -nst \
--method soft-dfs -to 0123456789 -dto 19,0126394875 -step 500 -sp r:tf --st-name 23 -nst \
--method random-dfs -to 0123467 -dto "16,0[123467]=asw(1,0,0,0,0,0)" -step 500 -sp r:tf --st-name 24 -nst \
--method random-dfs -to 01234567 -dto "13,[0187465]=asw(1)" -dto "26,[0134587]=asw(1)" -step 500 -sp r:tf --st-name 32 -nst \
--method random-dfs -to "0123" -dto2 "6,01[2435]=asw(3,20,10,0,1)" -step 500 -sp r:tf --st-name new -nst \
--method random-dfs -to "[01][23468]" -seed 1547 -step 500 -sp r:tf --st-name s1 -nst \
--method random-dfs -to "[01][23468]" -seed 3709 -step 500 -sp r:tf --st-name s2 -nst \
--method random-dfs -to "[01][2345][6789]" -seed 332838 -step 500 -sp r:tf --st-name s3 -nst \
--method random-dfs -to "01[2345789]" -seed 923 -step 500 -sp r:tf --st-name s4  -nst \
--method random-dfs -to "01[234567]" -dto2 "6,0132[456789]" -seed 6519 -step 500 -sp r:tf --st-name s5 \
--prelude "380@24,380@5,380@9,760@32,380@12,380@24,380@10,380@5,200@s1,120@s2,200@new,380@3,380@9,380@24,380@18,85@s5,760@20,100@s3,171@s4,380@11,380@12,380@5,760@24,380@21,380@4,380@9,380@11,380@9,380@10,380@4,380@5,380@15,380@5,760@15,380@18,380@21,380@15,380@18,380@20,380@22,760@23,380@24,380@32,1520@12,760@18,1520@32,1520@24,2280@10,2280@24,4560@12"
