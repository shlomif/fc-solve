#!/bin/sh
freecell-solver-range-parallel-solve 1 32000 1 \
--method soft-dfs -to 0123456789 -step 500 --st-name 1 -nst \
--method soft-dfs -to 0123467 -step 500 --st-name 2 -nst \
--method random-dfs -seed 2 -to 0[01][23456789] -step 500 --st-name 3 -nst \
--method random-dfs -seed 1 -to 0[0123456789] -step 500 --st-name 4 -nst \
--method random-dfs -seed 3 -to 0[01][23467] -step 500 --st-name 5 -nst \
--method random-dfs -seed 4 -to 0[0123467] -step 500 --st-name 9 -nst \
--method random-dfs -to [01][23456789] -seed 8 -step 500 --st-name 10 -nst \
--method a-star -asw 0.2,0.8,0,0,0 -step 500 --st-name 11 -nst \
--method random-dfs -to [01][23456789] -seed 268 -step 500 --st-name 12 -nst \
--method random-dfs -to [0123456789] -seed 142 -step 500 --st-name 15 -nst \
--method random-dfs -to [01][23456789] -seed 5 -step 500 --st-name 17 -nst \
--method a-star -to 0123467 -asw 0.5,0,0.3,0,0 -step 500 --st-name 18 -nst \
--method random-dfs -seed 105 -step 500 --st-name 20 -nst \
--method a-star -asw 0.5,0,0.5,0,0 -step 500 --st-name 21 -nst \
--method soft-dfs -to 013[2456789] -step 500 --st-name 22 \
--prelude "263@2,323@9,389@5,343@2,345@12,398@10,334@9,349@3,346@5,336@18,350@5,313@20,350@2,649@4,328@12,329@11,181@20,788@18,493@15,347@10,346@9,349@11,700@9,576@21,196@12,801@17,917@5,433@20,219@22,416@18,562@12,530@1,442@4,350@12,279@21,350@12,350@20,937@2,2100@10,1750@2,4453@12,4764@17,5144@20,5460@1"
