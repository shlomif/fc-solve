#!/bin/bash
for I in make_pysol_freecell_board.py pi-make-microsoft-freecell-board ;
do
    echo ".so man6/fc-solve-board_gen.6" > $I.6
    echo "$I.6 \\"
done
