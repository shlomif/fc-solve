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
--method a-star -asw 0.2,0.3,0.5,0,0 -step 500 --st-name 16 -nst \
--method random-dfs -to [01][23456789] -seed 5 -step 500 --st-name 17 -nst \
--method a-star -to 0123467 -asw 0.5,0,0.3,0,0 -step 500 --st-name 18 -nst \
--method random-dfs -seed 105 -step 500 --st-name 20 \
--prelude "280@2,394@9,101@2,394@5,186@2,396@10,345@12,257@9,163@2,516@15,357@3,346@2,342@18,691@5,374@20,328@11,350@20,305@12,352@9,265@4,349@10,306@18,342@4,350@9,863@11,350@9,114@4,177@20,697@18,783@16,350@18,587@10,1585@12,1750@18,376@20,801@17,680@20,1513@10,1328@11,4349@12,4550@20,4900@17,6300@1"
