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


class GenMulti:
    """docstring for GenMulti"""
    def __init__(self, argv):
        parser = argparse.ArgumentParser(
            prog='PROG',
            formatter_class=argparse.ArgumentDefaultsHelpFormatter)
        parser.add_argument('--dir', type=str, required=True,
                            help='output dir')
        parser.add_argument('--game', type=str, default='freecell',
                            help='The Solitaire variant')
        parser.add_argument('--ms', action='store_true',
                            help='MS/FC-Pro Deals')
        parser.add_argument('--prefix', type=str, required=True,
                            help='filename prefix')
        parser.add_argument('--suffix', type=str, required=True,
                            help='filename suffix')
        parser.add_argument('idxs', nargs='+', default=[],
                            help='indexes')
        args = parser.parse_args(argv[1:])
        self.dir_ = args.dir
        # Sanitize, see:
        # https://stackoverflow.com/questions/6803505
        self.pre = os.path.basename(args.prefix)
        self.suf = os.path.basename(args.suffix)
        self.game_variant = args.game
        self.which_deals = (RandomBase.DEALS_MS if args.ms
                            else RandomBase.DEALS_PYSOLFC)

        self.rend = CardRenderer(True)
        self.idxs = args.idxs
        self.game = Game(self.game_variant, 1, self.which_deals, 13)

    def _out_deal(self, deal):
        fn = os.path.join(self.dir_, self.pre + str(deal) + self.suf)
        with open(fn, 'wt') as f:
            f.write(self.game.calc_deal_string(deal, self.rend))

    def run(self):
        """docstring for run"""
        idxs = self.idxs
        while len(idxs):
            i = idxs.pop(0)
            if i == 'seq':
                start = int(idxs.pop(0))
                end = int(idxs.pop(0))
                for deal in range(start, end+1):
                    self._out_deal(deal)
            elif i == 'slurp':
                for line in open(idxs.pop(0), 'rt'):
                    self._out_deal(int(line))
            else:
                self._out_deal(int(i))
        return 0


if __name__ == "__main__":
    sys.exit(GenMulti(sys.argv).run())
