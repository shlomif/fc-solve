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
    perl -ne "printf (qq{$RESULTS_DIR/%09d.sol\\n}, \$_)" |
    xargs grep -l '^I could not solve this' |
    perl -lpe 's/\D//g;s/\A0+//' |
(while read DEAL ; do
    echo "${DEAL}:"
    ./board_gen/make_pysol_freecell_board.py "$DEAL" "$GAME" |
        ./fc-solve --game "$GAME" -l "toons-for-twenty-somethings" -mi "${MAX_ITERS//,/}" -p -t -sam > \
        "$(printf "%s/%09d.atomic.sol" "$RESULTS_DIR" "$DEAL")"
done) | tee -a total_dump.atomic.txt

