#!/bin/bash
if ! test -e 982.board ; then
    pi-make-microsoft-freecell-board 982 > 982.board
fi
./fc-solve-pruner --freecells-num 2 --method bfs -to 01ABCDE \
    -s -i -p -t 982.board 2>&1 | tee 982.dump
