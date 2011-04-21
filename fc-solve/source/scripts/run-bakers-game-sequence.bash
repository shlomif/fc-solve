#!/bin/bash

GAME="bakers_game"

RESULTS_DIR="$GAME-results/"

start="$1"
shift
end="$1"
shift

MAX_ITERS='10,000,000'

mkdir -p "$RESULTS_DIR"

seq "$start" "$end" |  \
(while read DEAL ; do
    echo "${DEAL}:"
    ./board_gen/make_pysol_freecell_board.py "$DEAL" "$GAME" |
        ./fc-solve --game "$GAME" -l eo -mi "$MAX_ITERS" > "$RESULTS_DIR/$DEAL.sol"
done) | tee -a total_dump.txt

