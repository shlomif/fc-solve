#!/usr/bin/env python3
#
# find-freecell-deal-index.py - a program to find out the Freecell deal index
# based on the initial cards layout.
#
# Copyright by Shlomi Fish, 2018
#
# Licensed under the MIT/Expat License.

import sys
from make_board_fc_solve import find_index_main, LCRandom31

if sys.version_info > (3,):
    xrange = range


def find_ret(ints):
    def is_right(d):
        r = LCRandom31()
        r.setSeed(d)
        n = len(ints)
        for i in ints:
            if r.randint(0, n) != i:
                return False
            n -= 1
        return True

    for d in xrange(1, (1 << 33)):
        if is_right(d):
            return d
    return -1


if __name__ == "__main__":
    sys.exit(find_index_main(sys.argv, find_ret))
