#!/usr/bin/python
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
# This program is platform independant and will generate the same results
# on all architectures and operating systems.
#
# Based on the code by Markus Franz Xaver Johannes Oberhumer.
# Modified by Shlomi Fish, 2000
#
# Since much of the code here is ripped from the actual PySol code, this 
# program is distributed under the GNU General Public License.
#
#
#
## vim:ts=4:et:nowrap
##
##---------------------------------------------------------------------------##
##
## PySol -- a Python Solitaire game
##
## Copyright (C) 2000 Markus Franz Xaver Johannes Oberhumer
## Copyright (C) 1999 Markus Franz Xaver Johannes Oberhumer
## Copyright (C) 1998 Markus Franz Xaver Johannes Oberhumer
##
## This program is free software; you can redistribute it and/or modify
## it under the terms of the GNU General Public License as published by
## the Free Software Foundation; either version 2 of the License, or
## (at your option) any later version.
##
## This program is distributed in the hope that it will be useful,
## but WITHOUT ANY WARRANTY; without even the implied warranty of
## MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
## GNU General Public License for more details.
##
## You should have received a copy of the GNU General Public License
## along with this program; see the file COPYING.
## If not, write to the Free Software Foundation, Inc.,
## 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
##
## Markus F.X.J. Oberhumer
## <markus.oberhumer@jk.uni-linz.ac.at>
## http://wildsau.idv.uni-linz.ac.at/mfx/pysol.html
##
##---------------------------------------------------------------------------##


# imports
import sys, os, re, string, time, types

# PySol imports

# /***********************************************************************
# // Abstract PySol Random number generator.
# //
# // We use a seed of type long in the range [0, MAX_SEED].
# ************************************************************************/

class PysolRandom:
    MAX_SEED = 0L

    ORIGIN_UNKNOWN  = 0
    ORIGIN_RANDOM   = 1
    ORIGIN_PREVIEW  = 2         # random from preview
    ORIGIN_SELECTED = 3         # manually entered
    ORIGIN_NEXT_GAME = 4        # "Next game number"

    def __init__(self, seed=None):
        if seed is None:
            seed = self._getRandomSeed()
        self.initial_seed = self.setSeed(seed)
        self.origin = self.ORIGIN_UNKNOWN

    def __str__(self):
        return self.str(self.initial_seed)

    def reset(self):
        self.seed = self.initial_seed

    def getSeed(self):
        return self.seed

    def setSeed(self, seed):
        seed = self._convertSeed(seed)
        if type(seed) is not types.LongType:
            raise TypeError, "seeds must be longs"
        if not (0L <= seed <= self.MAX_SEED):
            raise ValueError, "seed out of range"
        self.seed = seed
        return seed

    def copy(self):
        random = PysolRandom(0L)
        random.__class__ = self.__class__
        random.__dict__.update(self.__dict__)
        return random

    #
    # implementation
    #

    def choice(self, seq):
        return seq[int(self.random() * len(seq))]

    # Get a random integer in the range [a, b] including both end points.
    def randint(self, a, b):
        return a + int(self.random() * (b+1-a))

    #
    # subclass responsibility
    #

    # Get the next random number in the range [0.0, 1.0).
    def random(self):
        raise SubclassResponsibility

    #
    # subclass overrideable
    #

    def _convertSeed(self, seed):
        return long(seed)

    def increaseSeed(self, seed):
        if seed < self.MAX_SEED:
            return seed + 1L
        return 0L

    def _getRandomSeed(self):
        t = long(time.time() * 256.0)
        t = (t ^ (t >> 24)) % (self.MAX_SEED + 1L)
        return t

    #
    # shuffle
    #   see: Knuth, Vol. 2, Chapter 3.4.2, Algorithm P
    #   see: FAQ of sci.crypt: "How do I shuffle cards ?"
    #

    def shuffle(self, seq):
        n = len(seq) - 1
        while n > 0:
            j = self.randint(0, n)
            seq[n], seq[j] = seq[j], seq[n]
            n = n - 1


# /***********************************************************************
# // Linear Congruential random generator
# //
# // Knuth, Donald.E., "The Art of Computer Programming,", Vol 2,
# // Seminumerical Algorithms, Third Edition, Addison-Wesley, 1998,
# // p. 106 (line 26) & p. 108
# ************************************************************************/

class LCRandom64(PysolRandom):
    MAX_SEED = 0xffffffffffffffffL  # 64 bits

    def str(self, seed):
        s = repr(long(seed))[:-1]
        s = "0"*(20-len(s)) + s
        return s

    def random(self):
        self.seed = (self.seed*6364136223846793005L + 1L) & self.MAX_SEED
        return ((self.seed >> 21) & 0x7fffffffL) / 2147483648.0
        


# /***********************************************************************
# // Linear Congruential random generator
# // In PySol this is only used for 0 <= seed <= 32000.
# ************************************************************************/

class LCRandom31(PysolRandom):
    MAX_SEED = 0x7fffffffL          # 31 bits

    def str(self, seed):
        return "%05d" % int(seed)

    def random(self):
        self.seed = (self.seed*214013L + 2531011L) & self.MAX_SEED
        return (self.seed >> 16) / 32768.0

    def randint(self, a, b):
        self.seed = (self.seed*214013L + 2531011L) & self.MAX_SEED
        return a + (int(self.seed >> 16) % (b+1-a))

def get_card_num(rank,print_ts):
    ret = ""
    if rank == 1:
        ret = ret + "A"
    elif rank < 10:
        ret = ret + str(rank)
    elif rank == 10:
        if print_ts:
            ret = ret + "T"
        else:
            ret = ret + "10"
    elif rank == 11:
        ret = ret + "J"
    elif rank == 12:
        ret = ret + "Q"
    elif rank == 13:
        ret = ret + "K"
    return ret;

def get_card_suit(suit):
    if suit == 0:
        return "C"
    elif suit == 1:
        return "S"
    elif suit == 2:
        return "H"
    elif suit == 3:
        return "D"

class Card:

    ACE = 1
    KING = 13

    def __init__(self, rank, suit, print_ts):
        self.rank = rank
        self.suit = suit
        self.flipped = False
        self.print_ts = print_ts
        self.empty = False

    def is_king(self):
        return self.rank == self.KING

    def is_ace(self):
        return self.rank == self.ACE

    def rank_s(self):
        return get_card_num(self.rank, self.print_ts)

    def suit_s(self):
        return get_card_suit(self.suit);

    def to_s(self):
        if self.empty:
            return "-"
        ret = ""
        ret = ret + self.rank_s()
        ret = ret + self.suit_s()
        if self.flipped:
            ret = "<" + ret + ">"

        return ret

    def found_s(self):
        return self.suit_s() + "-" + self.rank_s()

    def flip(self, flipped=True):
        new_card = Card(self.rank, self.suit, self.print_ts)
        new_card.flipped = flipped
        return new_card

    def is_empty(self):
        return self.empty

class Columns:

    def __init__(self, num):
        self.num = num
        cols = []
        for i in range(num):
            cols.append([])

        self.cols = cols

    def add(self, idx, card):
        self.cols[idx].append(card)

    def rev(self):
        self.cols.reverse()

    def output(self):
        for column in self.cols:
            print column_to_string(column)

class Board:
    def __init__(self, num_columns, with_freecells=False,
            with_talon=False, with_foundations=False):
        self.with_freecells = with_freecells
        self.with_talon = with_talon
        self.with_foundations = with_foundations
        self.columns = Columns(num_columns)
        if (self.with_freecells):
            self.freecells = []
        if (self.with_talon):
            self.talon = []
        if (self.with_foundations):
            self.foundations = map(lambda s:empty_card(),range(4))
    
    def reverse_cols(self):
        return self.columns.rev()

    def add(self, idx, card):
        return self.columns.add(idx, card)

    def print_freecells(self):
        print "FC: " + column_to_string(self.freecells)

    def print_talon(self):
        print "Talon: " + column_to_string(self.talon)

    def print_foundations(self):
        cells = []
        for f in [2,0,3,1]:
            if not self.foundations[f].is_empty():
                cells.append(self.foundations[f].found_s())

        if len(cells):
            print "Foundations:" + ("".join(map(lambda s: " "+s, cells)))

    def output(self):
        if (self.with_talon):
            self.print_talon()
        if (self.with_foundations):
            self.print_foundations()
        if (self.with_freecells):
            self.print_freecells()
        self.columns.output()

    def add_freecell(self, card):
        if not self.with_freecells:
            raise "Layout does not have freecells!"
        self.freecells.append(card)

    def add_talon(self, card):
        if not self.with_talon:
            raise "Layout does not have a talon!"
        
        self.talon.append(card)

    def put_into_founds(self, card):
        if not self.with_foundations:
            raise "Layout does not have foundations!"
        
        if ((self.foundations[card.suit].rank+1) == card.rank):
            self.foundations[card.suit] = card
            return True
        else:
            return False
        self.talon.append(card)


def empty_card():
    ret = Card(0,0,1)
    ret.empty = True
    return ret

def createCards(num_decks, print_ts):
    cards = []
    for deck in range(num_decks):
        for suit in range(4):
            for rank in range(13):
                cards.append(Card(rank+1, suit, print_ts))
    return cards

def column_to_list_of_strings(col):
    return map( lambda c: c.to_s(), col)

def column_to_string(col):
    return " ".join(column_to_list_of_strings(col))

def flip_card(card_str, flip):
    if flip:
        return "<" + card_str + ">"
    else:
        return card_str

    
def shuffle(orig_cards, game_num):
    if game_num <= 32000:
        r = LCRandom31()
        r.setSeed(game_num)
        fcards = []
        if (len(orig_cards) == 52):
            for i in range(13):
                for j in (0, 39, 26, 13):
                    fcards.append(orig_cards[i + j])
            orig_cards = fcards
        r.shuffle(orig_cards)
    else:
        r = LCRandom64()
        r.setSeed(game_num)
        r.shuffle(orig_cards)

    return orig_cards

class WhichGameIter:
    def __init__(self, game):
        self.game = game

    def __iter__(self):
        return self

    def next(self):
        return self.game.next()
 
class WhichGame:
    REVERSE_MAP = \
        { 
                "freecell": 
                [ "freecell", "forecell", "bakers_game", 
        "ko_bakers_game", "kings_only_bakers_game", "relaxed_freecell", 
        "eight_off" ],
                "der_katz":
                [ "der_katz", "der_katzenschwantz", "die_schlange"],
                "seahaven":
                [ "seahaven_towers", "seahaven", "relaxed_seahaven", "relaxed_seahaven_towers" ],
                "bakers_dozen" : True,
                "gypsy" : True,
                "klondike" : [ "klondike", "klondike_by_threes", "casino_klondike", "small_harp", "thumb_and_pouch", "vegas_klondike", "whitehead" ],
                "simple_simon" : True,
                "yukon" : True,
                "beleaguered_castle" : [ "beleaguered_castle", "streets_and_alleys", "citadel" ],
                "fan" : True
        }

    def __init__(self, game_id, print_ts):
        mymap = {}
        for k in self.REVERSE_MAP.keys():
            if (self.REVERSE_MAP[k] == True):
                mymap[k] = k
            else:
                for alias in self.REVERSE_MAP[k]:
                    mymap[alias] = k
        self.games_map = mymap
        self.game_id = game_id
        self.print_ts = print_ts

    def lookup(self):
        return self.games_map[self.game_id];

    def is_two_decks(self):
        return self.game_id in ("der_katz", "der_katzenschwantz", "die_schlange", "gypsy")

    def get_num_decks(self):
        if self.is_two_decks():
            return 2
        else:
            return 1

    def deal(self, game_num):
        orig_cards = createCards(self.get_num_decks(), self.print_ts)
    
        orig_cards = shuffle(orig_cards, game_num)
    
        cards = orig_cards
        cards.reverse()

        self.cards = cards
        self.card_idx = 0
        return True
    
    def __iter__(self):
        return WhichGameIter(self)

    def no_more_cards(self):
        return self.card_idx >= len(self.cards)

    def next(self):
        if self.no_more_cards():
            raise StopIteration
        c = self.cards[self.card_idx]
        self.card_idx = self.card_idx + 1
        return c

    def new_cards(self, cards):
        self.cards = cards
        self.card_idx = 0

def shlomif_main(args):
    print_ts = 0
    if (args[1] == "-t"):
        print_ts = 1
        args.pop(0)
    game_num = long(args[1])
    if (len(args) >= 3):
        which_game = args[2]
    else:
        which_game = "freecell"

    game = WhichGame(which_game, print_ts)

    game_class = game.lookup()

    game.deal(game_num)

    if game_class == "der_katz":
        if (which_game == "die_schlange"):
            print "Foundations: H-A S-A D-A C-A H-A S-A D-A C-A"

        board = Board(9)
        col_idx = 0

        for card in game:
            if card.is_king():
                col_idx = col_idx + 1
            if not ((which_game == "die_schlange") and (card.rank == 1)):
                board.add(col_idx, card)

    elif game_class == "freecell":
        is_fc = (which_game in ('forecell', 'eight_off'))

        board = Board(8, with_freecells=is_fc)

        if is_fc:
            for i in range(52):
                if (i < 48):
                    board.add(i%8, game.next())
                else:
                    board.add_freecell(game.next())
                    if which_game == "eight_off":
                        board.add_freecell(empty_card())
        else:
            for i in range(52):
                board.add(i%8, game.next())
        
    elif game_class == "seahaven":
        board = Board(10, with_freecells=True)

        board.add_freecell(empty_card())

        for i in range(52):
            if (i < 50):
                board.add(i%10, game.next())
            else:
                board.add_freecell(game.next())

    elif game_class == "bakers_dozen":
        i, n = 0, 13 
        kings = []
        cards = game.cards
        cards.reverse()
        for c in cards:
            if c.is_king():
                kings.append(i)
            i = i + 1
        for i in kings:
            j = i % n
            while j < i:
                if not cards[j].is_king():
                    cards[i], cards[j] = cards[j], cards[i]
                    break
                j = j + n

        board = Board(13)

        for i in range(52):
            board.add(i%13, cards[i])
        
    elif game_class == "gypsy":
        board = Board(8, with_talon=True)
        for i in range(8*3):
            board.add((i % 8), game.next().flip(flipped=(i<8*2)))

        for card in game:
            board.add_talon(card)

    elif game_class == "klondike":
        board = Board(7, with_talon=True)
        card_num = 0

        for r in range(1,7):
            for s in range(7-r):
                board.add(s, game.next().flip())

        for s in range(7):
            board.add(s, game.next())

        for card in game:
            board.add_talon(card)

        if not (which_game == "small_harp"):
            board.reverse_cols()

    elif game_class == "simple_simon":

        board = Board(10)

        num_cards = 9

        while num_cards >= 3:
            for s in range(num_cards):
                board.add(s, game.next())
            num_cards = num_cards - 1

        for s in range(10):
            board.add(s, game.next())

    elif game_class == "yukon":

        board = Board(7)

        for i in range(1, 7):
            for j in range(i, 7):
                board.add(j, game.next().flip())

        for i in range(4):
            for j in range(1,7):
                board.add(j, game.next())

        for i in range(7):
            board.add(i, game.next())

    elif game_class == "beleaguered_castle":
        aces_up = which_game in ("beleaguered_castle", "citadel")
    
        board = Board(8, with_foundations=True)

        if aces_up:
            new_cards = []

            for c in game:
                if c.is_ace():
                    board.put_into_founds(c)
                else:
                    new_cards.append(c)

            game.new_cards(new_cards)


        for i in range(6):
            for s in range(8):
                c = game.next()
                if (which_game == "citadel") and board.put_into_founds(c):
                    # Already dealt with this card
                    True
                else:
                    board.add(s, c)
            if game.no_more_cards():
                break

        if (which_game == "streets_and_alleys"):
            for s in range(4):
                board.add(s, game.next())

    elif game_class == "fan":
        board = Board(18)

        for i in range(52-1):
            board.add(i%17, game.next())
        
        board.add(17, game.next())

    else:
        raise "Unknown game type " + which_game + "\n"

    board.output()
            
if __name__ == "__main__":
    sys.exit(shlomif_main(sys.argv))


