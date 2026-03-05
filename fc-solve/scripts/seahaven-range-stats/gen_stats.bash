#!/bin/bash

(
for (( seed=1 ; seed<=5000 ; ++seed ))
do
    echo "SEED=$seed"
    make_pysol_freecell_board.py "$seed" seahaven |
        fc-solve -l gi --game seahaven -mi 1500000 -p -t -sam -sel - |
            tail -3
done
) | tee total.dump.txt
