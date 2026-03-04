#!/bin/bash
for (( i=1 ; i<=32000 ; ++i ))
do
    echo "$i"
    pi-make-microsoft-freecell-board -t "$i" | \
        ./fc-solve -s -i -p -t -sam -sel --read-from-file "4,$HOME/progs/freecell/git/fc-solve/fc-solve/scripts/TEST_OPTIMIZATIONS/obf-mod4.sh" > \
    sols/"$i.sol"
done
