#!/bin/bash
for i in make_pysol_freecell_board.py pi-make-microsoft-freecell-board
do
    echo ".so man6/fc-solve-board_gen.6" > "$i.6"
done
