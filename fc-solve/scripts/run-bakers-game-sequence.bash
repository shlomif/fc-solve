#!/bin/bash

GAME="bakers_game"

RESULTS_DIR="$GAME-results/"

START="${1//,/}"
shift
END="${1//,/}"
shift

MAX_ITERS='10,000,000'
MAX_ITERS="${MAX_ITERS//,/}"

mkdir -p "$RESULTS_DIR"

(
for (( deal=START ; deal<=END ; ++deal ))
do
    echo "${deal}:"
    ./board_gen/make_pysol_freecell_board.py "$deal" "$GAME" |
        ./fc-solve --game "$GAME" -l eo -mi "${MAX_ITERS}" -p -t -sam > \
        "$(printf "%s/%09d.sol" "$RESULTS_DIR" "$deal")"
done
) | tee -a total_dump.txt
