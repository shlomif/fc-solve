#!/usr/bin/env python3
#
# find-freecell-deal-index.py - a program to find out the Freecell deal index
# based on the initial cards layout.
#
# Copyright by Shlomi Fish, 2018
#
# Licensed under the MIT/Expat License.

import sys

from make_pysol_freecell_board import find_index_main

from pysol_cards.random import LCRandom31

from six.moves import range


def find_ret(ints):
    r = LCRandom31()

    def is_right(d):
        r.setSeed(d)
        n = len(ints)
        for i in ints:
            if r.randint(0, n) != i:
                return False
            n -= 1
        return True

    def _h():
        for d in range(1, r.MAX_SEED+1):
            if is_right(d):
                return d
        return -1
    return _h()
    count = 0
    while True:
        ret = _h()
        count += 1
        if not count & 0b1:
            print(count, flush=True)

    return ret


if __name__ == "__main__":
    sys.exit(find_index_main(sys.argv, find_ret))
