#! /usr/bin/env python3
# -*- coding: utf-8 -*-
# vim:fenc=utf-8
#
# Copyright © 2018 Shlomi Fish <shlomif@cpan.org>
#
# Distributed under terms of the MIT license.

"""
Run like:

PYTHONPATH="`pwd`/t/lib" python3 ../scripts/py-trains-test.py

from:

~/progs/freecell/git/fc-solve/fc-solve/source

or equivalent.

"""

import argparse
import sys

from FC_Solve import FC_Solve

from pysol_cards.cards import CardRenderer
from pysol_cards.deal_game import Game
from pysol_cards.random import RandomBase

from trains import Task

Task.init("fc-solve", "looking-glass preset training")


class GenMulti:
    """docstring for GenMulti"""
    def __init__(self, argv):
        parser = argparse.ArgumentParser(
            prog='PROG',
            formatter_class=argparse.ArgumentDefaultsHelpFormatter)
        parser.add_argument('--game', type=str, default='freecell',
                            help='The Solitaire variant')
        parser.add_argument('--ms', action='store_true', default=True,
                            help='MS/FC-Pro Deals')
        parser.add_argument('idxs', nargs='*', default=['seq', '1', '1000000'],
                            help='indexes')
        args = parser.parse_args(argv[1:])
        # Sanitize, see:
        # https://stackoverflow.com/questions/6803505
        self.game_variant = args.game
        self.which_deals = (RandomBase.DEALS_MS if args.ms
                            else RandomBase.DEALS_PYSOLFC)

        self.rend = CardRenderer(True)
        self.idxs = args.idxs
        self.game = Game(self.game_variant, 1, self.which_deals, 13)

    def _out_deal(self, deal):
        board_s = self.game.calc_deal_string(deal, self.rend)
        sys.stdout.write(board_s)
        obj = self.obj
        if obj.solve_board(board_s) == 0:
            move = obj.get_next_move()
            while move:
                print([ord(move.s[x]) for x in range(4)])
                move = obj.get_next_move()
        obj.recycle()

    def run(self):
        """docstring for run"""
        idxs = self.idxs
        obj = FC_Solve()
        obj.input_cmd_line(['-l', 'lg'])
        obj.limit_iterations(100000)
        self.obj = obj
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
