#!/usr/bin/env python

import sys
sys.path.append("./t/lib")

from TAP.Simple import *
from ctypes import *

board_gen_lib = CDLL("../libfcs_gen_ms_freecell_boards.so")

fc_solve_get_board = board_gen_lib.fc_solve_get_board

def test_board(idx, want_string):

    buffer = ("x" * 500)

    fc_solve_get_board(idx, c_char_p(buffer))

    ok (buffer[0:buffer.find("\0")] == want_string,
        ("board %d was generated fine" % (idx)))

def main():
    plan(3)

    # TEST
    test_board(24,
    """4C 2C 9C 8C QS 4S 2H
5H QH 3C AC 3H 4H QD
QC 9S 6H 9H 3S KS 3D
5D 2S JC 5C JH 6D AS
2D KD TH TC TD 8D
7H JS KH TS KC 7C
AH 5S 6S AD 8H JD
7S 6C 7D 4D 8S 9D
""");

    # TEST
    test_board(11982,
"""AH 3D KD JC 6C JD KC
AS 3H 6H 5D 2C 7D 8D
4H QS 5S 5C TH 8H 2S
AC QC 4D 8C QH 9C 3S
2D 8S 9H 9D 6D 2H
6S 7H JH TD TC QD
TS AD 9S KH 4S 4C
JS KS 3C 7C 7S 5H
""");

    # TEST
    test_board(1234567,
"""2H 7S 4S 4D 8C KD QD
9C KH 5D 6C TC 5H 7D
JC TH JD 2S KS TS 9H
4H 8H 5C 9D 5S JS 3D
AD 3H 6D JH 3S KC
QC 9S 8D AS 7C 8S
AC 3C QH TD 6S 6H
2C 2D 4C 7H AH QS
""");

if __name__ == "__main__":
    main()

