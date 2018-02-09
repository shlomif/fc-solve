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
from make_board_fc_solve import empty_card, RandomBase, Board, BaseGame


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

    def freecell(game):
        is_fc = (game.game_id in ('forecell', 'eight_off'))

        game.board = Board(8, with_freecells=is_fc)

        if is_fc:
            game.cyclical_deal(48, 8)
            for card in game:
                game.add_freecell(card)
                if game.game_id == "eight_off":
                    game.add_freecell(empty_card())
        else:
            game.cyclical_deal(52, 8)

    def seahaven(game):
        game.board = Board(10, with_freecells=True)
        game.add_freecell(empty_card())
        game.cyclical_deal(50, 10)
        for card in game:
            game.add_freecell(card)

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

    def gypsy(game):
        num_cols = 8
        game.board = Board(num_cols, with_talon=True)
        game.cyclical_deal(num_cols*2, num_cols, flipped=True)
        game.cyclical_deal(num_cols, num_cols, flipped=False)
        game.add_all_to_talon()

    def klondike(game):
        num_cols = 7
        game.board = Board(num_cols, with_talon=True)

        for r in range(1, num_cols):
            for s in range(num_cols-r):
                game.add(s, next(game).flip())

        game.cyclical_deal(num_cols, num_cols)
        game.add_all_to_talon()
        if not (game.game_id == "small_harp"):
            game.board.reverse_cols()

    def simple_simon(game):
        game.board = Board(10)
        num_cards = 9
        while num_cards >= 3:
            for s in range(num_cards):
                game.add(s, next(game))
            num_cards -= 1
        for s in range(10):
            game.add(s, next(game))

    def fan(game):
        game.board = Board(18)
        game.cyclical_deal(52-1, 17)
        game.add(17, next(game))

    def _shuffleHookMoveSorter(self, cards, func, ncards):
        # note that we reverse the cards, so that smaller sort_orders
        # will be nearer to the top of the Talon
        sitems, i = [], len(cards)
        for c in cards[:]:
            select, sort_order = func(c)
            if select:
                cards.remove(c)
                sitems.append((sort_order, i, c))
                if len(sitems) >= ncards:
                    break
            i -= 1
        sitems.sort()
        sitems.reverse()
        return cards, [item[2] for item in sitems]

    def _shuffleHookMoveToBottom(self, cards, func, ncards=999999):
        # move cards to bottom of the Talon (i.e. last cards to be dealt)
        cards, scards = self._shuffleHookMoveSorter(cards, func, ncards)
        return scards + cards

    def _shuffleHookMoveToTop(self, cards, func, ncards=999999):
        # move cards to top of the Talon (i.e. last cards to be dealt)
        cards, scards = self._shuffleHookMoveSorter(cards, func, ncards)
        return cards + scards

    def black_hole(game):
        game.board = Board(17)

        # move Ace to bottom of the Talon (i.e. last cards to be dealt)
        game.cards = game._shuffleHookMoveToBottom(
            game.cards,
            lambda c: (c.id == 13, c.suit),
            1)
        next(game)
        game.cyclical_deal(52-1, 17)

        print("Foundations: AS")

    def all_in_a_row(game):
        game.board = Board(13)

        # move Ace to bottom of the Talon (i.e. last cards to be dealt)
        game.cards = game._shuffleHookMoveToTop(
            game.cards,
            lambda c: (c.id == 13, c.suit),
            1)
        game.cyclical_deal(52, 13)
        print("Foundations: -")

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

    def yukon(game):
        num_cols = 7
        game.board = Board(num_cols)

        for i in range(1, num_cols):
            for j in range(i, num_cols):
                game.add(j, next(game).flip())

        for i in range(4):
            for j in range(1, num_cols):
                game.add(j, next(game))

        game.cyclical_deal(num_cols, num_cols)


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
