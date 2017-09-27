#!/bin/bash
for i in $(seq 1 32000) ; do
    echo "$i"
    pi-make-microsoft-freecell-board -t "$i" | \
        ./fc-solve -s -i -p -t -sam -sel --read-from-file "4,$HOME/progs/freecell/git/fc-solve/fc-solve/scripts/TEST_OPTIMIZATIONS/obf-mod4.sh" > \
    sols/"$i.sol"
done
