freecell-solver-range-parallel-solve 1 32000 1 \
--method soft-dfs -to 01ABCDE -step 500 --st-name 0 -nst \
--method random-dfs -to [01][ABCDE] -seed 1 -step 500 --st-name 1 -nst \
--method random-dfs -to [01][ABCDE] -seed 2 -step 500 --st-name 2 -nst \
--method random-dfs -to [01][ABCDE] -seed 3 -step 500 --st-name 3 -nst \
--method random-dfs -to 01[ABCDE] -seed 268 -step 500 --st-name 4 -nst \
--method a-star -to 01ABCDE -step 500 --st-name 5 -nst \
--method a-star -to 01ABCDE -asw 0.5,0,0.5,0,0 -step 500 --st-name 7 -nst \
--method random-dfs -to [01][ABD][CE] -seed 1900 -step 500 --st-name 8 -nst \
--method random-dfs -to [01][ABCDE] -seed 192 -step 500 --st-name 9 -nst \
--method random-dfs -to [01ABCDE] -seed 1977 -step 500 --st-name 10 -nst \
--method random-dfs -to [01ABCDE] -seed 24 -step 500 --st-name 11 -nst \
--method soft-dfs -to 01EABCD -step 500 --st-name 14 -nst \
--method soft-dfs -to 01BDAEC -step 500 --st-name 15 -nst \
--method random-dfs -to [01][ABCDE] -seed 4 -step 500 --st-name 16 -nst \
--method random-dfs -to [01][ABD][CE] -seed 8 -step 500 --st-name 17 -nst \
--method soft-dfs -to 01DECAB -step 500 --st-name 18 -nst \
--method random-dfs -to [01][ABCDE] -seed 71 -step 500 --st-name 19 \
--prelude "2800@0,2800@3,1750@0,3150@16,350@3,700@0,1400@3,2450@4,350@0,1400@4,700@16,2100@0,350@3,350@4,350@3,1400@16,700@3,700@4,700@0,700@3,1400@0,1050@4,700@0,350@3,350@0,4550@18,5600@7,700@4,2450@18,4900@19,1400@18,1050@3,2450@19,1750@0,4550@1,350@18,1050@0,350@1,700@15,350@7,1050@0,8050@1,2450@4,350@18,1750@19,1750@0,12250@2,700@4,2450@18,3500@11,2800@9,1750@7,3150@8,2100@10,700@15,4200@10,700@2,1050@4,700@5,700@7,4550@9,2800@10,3500@14,1050@4,1050@17,3150@1,3500@3,2100@14,1050@15,1750@18,2100@0,4900@15,4200@3,1750@7,2450@9,1750@17,1750@19,2100@16,2800@4,7700@8,5600@16,2800@19,4550@18,4900@1,5950@4,2450@14,2450@16,2800@2,2800@3,2800@9,5600@19,3150@3,3150@14,3850@2,3850@19,4550@7,4900@4,6650@18,6650@16,13650@19,6300@4,14700@15,11900@5,11900@17,9800@2,10150@19,10500@4,27650@3,23450@8,35700@9,50750@11,82600@14"


