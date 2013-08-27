#!/bin/bash

# Parameters for the deal:
# deal_idx=982
deal_idx=384243

# Some preferences
delta_limit="40,000,000"
count_items_in_queue="5,000"
cache_size="40,000,000"

board="$deal_idx.board"

offload_dir="FCS-DBM-QUEUE-DIR"
if ! test -e "$offload_dir" ; then
    mkdir "$offload_dir"
fi

if ! test -e "$board" ; then
    pi-make-microsoft-freecell-board -t "$deal_idx" > "$board"
fi

let iter=0

cache_size="${cache_size//,/}"
pre_cache_dummy_size="1000"
caches_delta="$(($cache_size - $pre_cache_dummy_size))"

while true; do

    intermediate_output="out-$deal_idx-$iter.txt"
    intermediate_output_processed="out-$deal_idx-$iter-proc.txt"
    get_proc=''

    if test \( \! -e "$intermediate_output_processed" \) -o \( $iter -eq 0 \) ; then

        if test $iter -gt 0 ; then
            get_proc="--intermediate-input $previous_intermediate_input"
        fi
        if ! ./dbm_fc_solver -o "$intermediate_output" \
            $get_proc \
            --offload-dir-path "$offload_dir/" \
            --iters-delta-limit "${delta_limit//,/}" \
            --max-count-of-items-in-queue "${count_items_in_queue//,/}" \
            --pre-cache-max-count "$pre_cache_dummy_size" \
            --caches-delta "$caches_delta" \
            --num-threads 1 \
            "$board" ; then
            echo "Error in iter $iter!" 1>2
            exit 1
        fi
        perl "$(dirname "$0")"/dbm-process.pl "$intermediate_output" \
            > "$intermediate_output_processed"
        if ! test -s "$intermediate_output_processed" ; then
            echo "No solution was found at iter ${iter}"
            exit 0
        fi
    elif ! test -s "$intermediate_output_processed" ; then
        echo "No solution was found at iter ${iter}"
        exit 0
    fi

    let iter++
    previous_intermediate_input="$intermediate_output_processed"
done
