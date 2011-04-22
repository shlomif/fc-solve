#!/bin/bash

GAME="bakers_game"

RESULTS_DIR="$GAME-results/"

START="${1//,/}"
shift
END="${1//,/}"
shift

MAX_ITERS='10,000,000'

mkdir -p "$RESULTS_DIR"

seq "$START" "$END" |
(while read DEAL ; do
    echo "${DEAL}:"
    ./board_gen/make_pysol_freecell_board.py "$DEAL" "$GAME" |
        ./fc-solve --game "$GAME" -l eo -mi "${MAX_ITERS//,/}" -p -t -sam > \
        "$(printf "%s/%09d.sol" "$RESULTS_DIR" "$DEAL")"
done) | tee -a total_dump.txt

