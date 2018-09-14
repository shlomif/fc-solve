#! /usr/bin/env python3
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
parser.add_argument('--game', type=str, default='freecell',
                    help='The Solitaire variant')
parser.add_argument('--ms', action='store_true', help='MS/FC-Pro Deals')
parser.add_argument('--prefix', type=str, required=True,
                    help='filename prefix')
parser.add_argument('--suffix', type=str, required=True,
                    help='filename suffix')
parser.add_argument('idxs', nargs='+', default=[],
                    help='indexes')
args = parser.parse_args(sys.argv[1:])
dir_ = args.dir
pre = args.prefix
suf = args.suffix
game_variant = args.game
which_deals = RandomBase.DEALS_MS if args.ms else RandomBase.DEALS_PYSOLFC

rend = CardRenderer(True)


def _out_deal(deal):
    fn = os.path.join(dir_, pre + str(deal) + suf)
    with open(fn, 'wt') as f:
        f.write(Game(game_variant, deal, which_deals, 13).
                calc_layout_string(rend))


idxs = args.idxs
while len(idxs):
    i = idxs.pop(0)
    if i == 'seq':
        start = int(idxs.pop(0))
        end = int(idxs.pop(0))
        for deal in range(start, end+1):
            _out_deal(deal)
    else:
        _out_deal(int(i))
