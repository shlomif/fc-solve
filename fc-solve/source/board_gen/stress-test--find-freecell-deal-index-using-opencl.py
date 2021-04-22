#!/usr/bin/env python3
#
# find-freecell-deal-index.py - a program to find out the Freecell deal index
# based on the initial cards layout.
#
# Copyright by Shlomi Fish, 2018
#
# Licensed under the MIT/Expat License.

import sys

# from opencl_find_index_lib_py import ffi, lib
from opencl_find_index_lib_py import lib

from make_pysol_freecell_board import find_index_main


def _to_bytes(s):
    if sys.version_info > (3,):
        return bytes(s, 'UTF-8')
    else:
        return s


def find_ret(ints):
    first_int = ints.pop(0)
    first_int |= (ints.pop(0) << 6)
    first_int |= (ints.pop(0) << 12)
    first_int |= (ints.pop(0) << 18)
    assert len(ints) == 47
    # print(first_int, ints)
    inp = [0] * 2 + list(reversed(ints))
    count = 0
    obj = lib.fc_solve_user__opencl_create()
    while True:
        ret = lib.fc_solve_user__opencl_find_deal(
            obj, first_int, inp
        )
        count += 1
        if not count & 0b11111111:
            print(count, flush=True)
    lib.fc_solve_user__opencl_release(obj)
    return ret


if __name__ == "__main__":
    sys.exit(find_index_main(sys.argv, find_ret))
