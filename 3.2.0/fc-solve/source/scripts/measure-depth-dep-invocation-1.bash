#!/bin/bash
start_board="1"
end_board="2000"

# p1="$start_board $end_board 1 --args-start --method random-dfs -seed 4 --args-end"
# to1="0123467"
# to2="0[123467]"

p1="$start_board $end_board 1 --args-start --method random-dfs -seed 500 --args-end"
to1="[01][23456789]"
to2="0123456789"

p2="--iters-limit 10000"
fn_base="0123456789-s$(date +"%s")s"
./measure-depth-dep-tests-order-perf $p1 --output-to "$fn_base".out --scan1-to "$to1" --scan2-to "$to2" $p2
./measure-depth-dep-tests-order-perf $p1 --output-to "$fn_base"-rev.out --scan1-to "$to2" --scan2-to "$to1" $p2
