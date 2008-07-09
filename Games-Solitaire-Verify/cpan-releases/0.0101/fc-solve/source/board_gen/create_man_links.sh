#!/bin/bash
for I in make-gnome-freecell-board make_pysol_freecell_board.py \
    make-aisleriot-freecell-board pi-make-microsoft-freecell-board ; 
do
    echo ".so man6/fc-solve-board_gen.6" > $I.6
    echo "$I.6 \\"
done

