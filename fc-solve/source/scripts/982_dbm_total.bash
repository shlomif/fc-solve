#!/bin/bash

# Parameters for the deal:
deal_idx=982

# Some preferences
delta_limit="1,000,000"
count_items_in_queue="5,000"

board="$deal_idx.board"

if ! test -e "$board" ; then
    pi-make-microsoft-freecell-board -t "$deal_idx" > "$board"
fi

let iter=0
intermediate_output="out-$deal_idx-$iter.txt"
intermediate_output_processed="out-$deal_idx-$iter-proc.txt"
get_proc=''
if test $iter -gt 0 ; then
    get_proc="--intermediate-input $previous_intermediate_input"
fi
if ! ./dbm_fc_solver -o "$intermediate_output" \
    $get_proc \
    --iters-delta-limit "${delta_limit//,/}" \
    --max-count-of-items-in-queue "${count_items_in_queue//,/}" \
    --num-threads 1 \
    982.board ; then
    echo "Error in iter $iter!" 1>2
    exit 1
fi
perl scripts/dbm-
