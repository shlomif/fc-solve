#!/bin/bash
for BOARD_NUM in 5376 16039 17880 22894 31465 31975 ; do
    pi-make-microsoft-freecell-board "$BOARD_NUM" | \
        ./fc-solve "$@" -mi 100000 | \
        tail -3 | \
        perl -0777 -e \
        '$_=<STDIN>; /Total number of states checked is (\d+)/; $n = $1;
         $bn=shift;
         print $bn.": ".(/This game is solve/?"Solved at $n":"Unsolved")."\n";
         ' "$BOARD_NUM"

done

