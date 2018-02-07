#!/usr/bin/env python3
#
# find-freecell-deal-index.py - a program to find out the Freecell deal index
# based on the initial cards layout.
#
# Copyright by Shlomi Fish, 2018
#
# Licensed under the MIT/Expat License.

import sys
from make_board_fc_solve import find_index_main
from find_index_lib_py import ffi, lib


def find_ret(ints):
    obj = lib.fc_solve_user__find_deal__alloc()
    lib.fc_solve_user__find_deal__fill(
        obj, "".join(["%-10d" % x for x in ints]))
    ret = int(ffi.string(lib.fc_solve_user__find_deal__run(
        obj, "1", "%d" % ((1 << 33) - 1))))
    lib.fc_solve_user__find_deal__free(obj)
    return ret


if __name__ == "__main__":
    sys.exit(find_index_main(sys.argv, find_ret))
