#!/usr/bin/env python

import sys
sys.path.append("./t/lib")

from TAP.Simple import *
from ctypes import *

def main():
    plan(1)

    board_gen_lib = CDLL("../libfcs_gen_ms_freecell_boards.so")

    fc_solve_get_board = board_gen_lib.fc_solve_get_board

    buffer = ("x" * 500)
    fc_solve_get_board(24, c_char_p(buffer))

    # TEST
    ok (buffer[0:buffer.find("\0")] == """4C 2C 9C 8C QS 4S 2H
5H QH 3C AC 3H 4H QD
QC 9S 6H 9H 3S KS 3D
5D 2S JC 5C JH 6D AS
2D KD TH TC TD 8D
7H JS KH TS KC 7C
AH 5S 6S AD 8H JD
7S 6C 7D 4D 8S 9D
""", 
    "Board 24 was generated fine")

if __name__ == "__main__":
    main()

