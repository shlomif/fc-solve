#!/usr/bin/env python3
#
# make_pysol_freecell_board.py - Program to generate the boards of
# PySol for input into Freecell Solver.
#
# Usage: make_pysol_freecell_board.py [board number] | fc-solve
#
# Or on non-UNIXes:
#
# python make_pysol_freecell_board.py [board number] | fc-solve
#
# This program is platform independent and will generate the same results
# on all architectures and operating systems.
#
# Based on the code by Markus Franz Xaver Johannes Oberhumer.
# Modified by Shlomi Fish, 2000
#
# Since much of the code here is ripped from the actual PySol code, this
# program is distributed under the GNU General Public License.

# Original PySol blurb:
#
# ---------------------------------------------------------------------------
#
# PySol -- a Python Solitaire game
#
# Copyright (C) 2000 Markus Franz Xaver Johannes Oberhumer
# Copyright (C) 1999 Markus Franz Xaver Johannes Oberhumer
# Copyright (C) 1998 Markus Franz Xaver Johannes Oberhumer
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; see the file COPYING.
# If not, write to the Free Software Foundation, Inc.,
# 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
#
# Markus F.X.J. Oberhumer
# <markus.oberhumer@jk.uni-linz.ac.at>
# http://wildsau.idv.uni-linz.ac.at/mfx/pysol.html
#
# ---------------------------------------------------------------------------

import sys
from make_board_fc_solve import RandomBase, Board, BaseGame


class Game(BaseGame):
    # These are the games variants:
    # Each one is a callback.
    def der_katz(game):
        if game.game_id == "die_schlange":
            print("Foundations: H-A S-A D-A C-A H-A S-A D-A C-A")

        game.board = Board(9)
        col_idx = 0

        for card in game:
            if card.is_king():
                col_idx += 1
            if not ((game.game_id == "die_schlange") and (card.rank == 1)):
                game.add(col_idx, card)

    def bakers_dozen(game):
        i, n = 0, 13
        kings = []
        cards = game.cards
        cards.reverse()
        for c in cards:
            if c.is_king():
                kings.append(i)
            i += 1
        for i in kings:
            j = i % n
            while j < i:
                if not cards[j].is_king():
                    cards[i], cards[j] = cards[j], cards[i]
                    break
                j += n

        game.new_cards(cards)
        game.board = Board(13)
        game.cyclical_deal(52, 13)

    def beleaguered_castle(game):
        aces_up = game.game_id in ("beleaguered_castle", "citadel")

        game.board = Board(8, with_foundations=True)

        if aces_up:
            new_cards = []

            for c in game:
                if c.is_ace():
                    game.board.put_into_founds(c)
                else:
                    new_cards.append(c)

            game.new_cards(new_cards)

        for i in range(6):
            for s in range(8):
                c = next(game)
                if not ((game.game_id == "citadel") and
                        game.board.put_into_founds(c)):
                    game.add(s, c)
            if game.no_more_cards():
                break

        if game.game_id == "streets_and_alleys":
            game.cyclical_deal(4, 4)


def shlomif_main(args):
    print_ts = False
    which_deals = RandomBase.DEALS_PYSOL
    while args[1][0] == '-':
        a = args[1]
        if a == "-t":
            print_ts = True
            args.pop(0)
        elif (a == "--pysolfc") or (a == "-F"):
            which_deals = RandomBase.DEALS_PYSOLFC
            args.pop(0)
        elif (a == "--ms") or (a == "-M"):
            which_deals = RandomBase.DEALS_MS
            args.pop(0)
        else:
            raise ValueError("Unknown flag " + a + "!")

    game_num = int(args[1])
    if len(args) >= 3:
        which_game = args[2]
    else:
        which_game = "freecell"

    game = Game(which_game, game_num, which_deals, print_ts)
    game.print_layout()


if __name__ == "__main__":
    sys.exit(shlomif_main(sys.argv))
