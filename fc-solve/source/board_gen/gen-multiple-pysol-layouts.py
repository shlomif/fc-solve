#! /usr/bin/env python
# -*- coding: utf-8 -*-
# vim:fenc=utf-8
#
# Copyright © 2018 Shlomi Fish <shlomif@cpan.org>
#
# Distributed under terms of the MIT license.

"""

"""

import argparse
import os.path
import sys
from fc_solve_find_index_s2ints import CardRenderer
from make_pysol_freecell_board import Game, RandomBase

parser = argparse.ArgumentParser(
    prog='PROG',
    formatter_class=argparse.ArgumentDefaultsHelpFormatter)
parser.add_argument('--dir', type=str, required=True, help='output dir')
parser.add_argument('--prefix', type=str, required=True,
                    help='filename prefix')
parser.add_argument('--suffix', type=str, required=True,
                    help='filename suffix')
parser.add_argument('idxs', nargs='+', default=[],
                    help='indexes')
args = parser.parse_args(sys.argv[1:])

rend = CardRenderer(True)

while len(args.idxs):
    i = args.idxs.pop(0)
    deal = int(i)
    fn = os.path.join(args.dir, args.prefix + str(deal) + args.suffix)
    with open(fn, 'wt') as f:
        f.write(Game("freecell", deal, RandomBase.DEALS_MS, 13).
                calc_layout_string(rend))
