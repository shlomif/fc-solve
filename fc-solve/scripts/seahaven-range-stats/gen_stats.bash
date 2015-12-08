#!/bin/bash

seq 1 5000 |
    (while read SEED; do
        echo "SEED=$SEED"
        make_pysol_freecell_board.py "$SEED" seahaven |
            fc-solve -l gi --game seahaven -mi 1500000 |
                tail -3
    done) | 
    tee total.dump.txt
