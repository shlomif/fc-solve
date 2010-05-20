#!/bin/bash
freecell-solver-fc-pro-range-solve 1 32000 1 \
--flare-name 1 --method soft-dfs -to 0123456789 -opt \
-nf --flare-name 9 --method random-dfs -seed 4 -to 0[0123467] -opt \
-nf --flare-name 16 --method a-star -asw 0.2,0.3,0.5,0,0 -opt \
-nf --flare-name 18 --method a-star -to 0123467 -asw 0.5,0,0.3,0,0 -opt \
--flares-plan "Run:3594@9,Run:7742@16,Run:3769@18"
