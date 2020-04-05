#!/usr/bin/env python3
for fn in ["make_pysol_freecell_board.py", "pi-make-microsoft-freecell-board"]:
    open(fn + ".6", "wt").write(".so man6/fc-solve-board_gen.6\n")
