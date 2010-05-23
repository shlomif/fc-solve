#!/bin/bash
freecell-solver-fc-pro-range-solve 1 32000 1 \
--flare-name 1 --method soft-dfs -to 0123456789 -opt \
-nf --flare-name 2 --method soft-dfs -to 0123467 -opt \
-nf --flare-name 3 --method random-dfs -seed 2 -to 0[01][23456789] -opt \
-nf --flare-name 4 --method random-dfs -seed 1 -to 0[0123456789] -opt \
-nf --flare-name 5 --method random-dfs -seed 3 -to 0[01][23467] -opt \
-nf --flare-name 9 --method random-dfs -seed 4 -to 0[0123467] -opt \
-nf --flare-name 10 --method random-dfs -to [01][23456789] -seed 8 -opt \
-nf --flare-name 11 --method a-star -asw 0.2,0.8,0,0,0 -opt \
-nf --flare-name 12 --method random-dfs -to [01][23456789] -seed 268 -opt \
-nf --flare-name 15 --method random-dfs -to [0123456789] -seed 142 -opt \
-nf --flare-name 16 --method a-star -asw 0.2,0.3,0.5,0,0 -opt \
-nf --flare-name 17 --method random-dfs -to [01][23456789] -seed 5 -opt \
-nf --flare-name 18 --method a-star -to 0123467 -asw 0.5,0,0.3,0,0 -opt \
-nf --flare-name 19 --method soft-dfs -to 0126394875 -opt \
-nf --flare-name 20 --method random-dfs -seed 105 -opt \
--flares-plan "Run:6246@1,Run:2663@2,Run:6799@3,Run:7161@4,Run:3466@5,Run:3594@9,Run:6896@10,Run:7269@11,Run:7194@12,Run:6462@15,Run:7742@16,Run:7029@17,Run:3769@18,Run:5244@19,Run:7149@20"
