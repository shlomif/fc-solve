#!/usr/bin/env python3
#
# make_board_fc_solve.py - Expat-licensed routines for the board-gen code.
#
# Copyright by Shlomi Fish, 2018
#
# Licensed under the MIT/Expat License.

import random2
import re
import sys


class Card(object):
    ACE = 1
    KING = 13

    def __init__(self, id, rank, suit, print_ts):
        self.id, self.rank, self.suit, self.print_ts = id, rank, suit, print_ts
        self.empty, self.flipped = False, False

    def rank_s(self):
        ret = "0A23456789TJQK"[self.rank]
        if ((not self.print_ts) and ret == 'T'):
            ret = '10'
        return ret

    def suit_s(self):
        return 'CSHD'[self.suit]

    def to_s(self):
        if self.empty:
            return '-'
        ret = self.rank_s() + self.suit_s()
        if self.flipped:
            ret = '<' + ret + '>'
        return ret

    def found_s(self):
        return self.suit_s() + '-' + self.rank_s()

    def is_ace(self):
        return self.rank == self.ACE

    def is_king(self):
        return self.rank == self.KING

    def flip(self, flipped=True):
        ret = Card(self.id, self.rank, self.suit, self.print_ts)
        ret.flipped = flipped
        return ret


def empty_card():
    ret = Card(0, 0, 0, True)
    ret.empty = True
    return ret


def column_to_string(col):
    return ' '.join([x.to_s() for x in col])


def createCards(num_decks, print_ts):
    ret = []
    for _ in range(num_decks):
        id = 0
        for s in range(4):
            for r in range(13):
                ret.append(Card(id, r+1, s, print_ts))
                id += 1
    return ret


class RandomBase:
    DEALS_PYSOL = 0
    DEALS_PYSOLFC = 1
    DEALS_MS = 2

    def shuffle(self, seq):
        for n in range(len(seq)-1, 0, -1):
            j = self.randint(0, n)
            seq[n], seq[j] = seq[j], seq[n]
        return seq

    def randint(self, a, b):
        return a + int(self.random() * (b+1-a))


class PysolRandom(RandomBase):
    def setSeed(self, seed):
        seed = self._convertSeed(seed)
        if not 0 <= seed <= self.MAX_SEED:
            raise ValueError("seed is out of range")
        self.seed = seed
        return seed

    def _convertSeed(self, seed):
        return int(seed)

# /***********************************************************************
# // Linear Congruential random generator
# //
# // Knuth, Donald.E., "The Art of Computer Programming,", Vol 2,
# // Seminumerical Algorithms, Third Edition, Addison-Wesley, 1998,
# // p. 106 (line 26) & p. 108
# ************************************************************************/


class LCRandom64(PysolRandom):
    MAX_SEED = 0xffffffffffffffff  # 64 bits

    def random(self):
        self.seed = (self.seed*6364136223846793005 + 1) & self.MAX_SEED
        return ((self.seed >> 21) & 0x7fffffff) / 2147483648.0


class LCRandom31(RandomBase):
    MAX_SEED = ((1 << (32+2))-1)         # 34 bits

    def setSeed(self, seed):
        self.seed = seed
        self.seedx = seed if (seed < 0x100000000) else (seed - 0x100000000)

    def random(self):
        if (self.seed < 0x100000000):
            ret = self._rand()
            return (ret if (self.seed < 0x80000000) else (ret | 0x8000))
        else:
            return self._randp() + 1

    def _randp(self):
        self.seedx = ((self.seedx) * 214013 + 2531011) & self.MAX_SEED
        return (self.seedx >> 16) & 0xffff

    def _rand(self):
        self.seedx = ((self.seedx) * 214013 + 2531011) & self.MAX_SEED
        return (self.seedx >> 16) & 0x7fff

    def randint(self, a, b):
        return a + self.random() % (b+1-a)


# * Mersenne Twister random number generator
class MTRandom(RandomBase, random2.Random):
    MAX_SEED = 100000000000000000000  # 20 digits

    def setSeed(self, seed):
        random2.Random.__init__(self, seed)
        self.initial_state = self.getstate()


def ms_rearrange(cards):
    if len(cards) != 52:
        return cards
    c = []
    for i in range(13):
        for j in (0, 39, 26, 13):
            c.append(cards[i + j])
    return c


class Columns:
    def __init__(self, num):
        self.num = num
        self.cols = [[] for _ in range(num)]

    def add(self, idx, card):
        self.cols[idx].append(card)

    def rev(self):
        self.cols.reverse()

    def output(self):
        for c in self.cols:
            print(column_to_string(c))


class Board:
    def __init__(self, num_columns, with_freecells=False,
                 with_talon=False, with_foundations=False):
        self.with_freecells = with_freecells
        self.with_talon = with_talon
        self.with_foundations = with_foundations
        self.columns = Columns(num_columns)
        if self.with_freecells:
            self.freecells = []
        if self.with_talon:
            self.talon = []
        if self.with_foundations:
            self.foundations = [empty_card() for s in range(4)]

    def reverse_cols(self):
        self.columns.rev()

    def add(self, idx, card):
        self.columns.add(idx, card)

    def add_freecell(self, card):
        if not self.with_freecells:
            raise AttributeError("Layout does not have freecells!")
        self.freecells.append(card)

    def add_talon(self, card):
        if not self.with_talon:
            raise AttributeError("Layout does not have a talon!")
        self.talon.append(card)

    def put_into_founds(self, card):
        if not self.with_foundations:
            raise AttributeError("Layout does not have foundations!")
        res = self.foundations[card.suit].rank+1 == card.rank
        if res:
            self.foundations[card.suit] = card
        return res

    def print_foundations(self):
        cells = []
        for f in [2, 0, 3, 1]:
            if not self.foundations[f].empty:
                cells.append(self.foundations[f].found_s())

        if len(cells):
            print("Foundations:" + ("".join([" "+s for s in cells])))

    def output(self):
        if self.with_talon:
            print("Talon: " + column_to_string(self.talon))
        if self.with_foundations:
            self.print_foundations()
        if self.with_freecells:
            print("Freecells: " + column_to_string(self.freecells))
        self.columns.output()


def shuffle(cards, game_num, which_deals):
    ms = ((game_num <= 32000) or (which_deals == RandomBase.DEALS_MS))
    r = LCRandom31() if ms else MTRandom() if which_deals == \
        RandomBase.DEALS_PYSOLFC else LCRandom64()
    r.setSeed(game_num)
    return r.shuffle(ms_rearrange(cards) if ms else cards)


class BaseGame:
    REVERSE_MAP = \
        {
                "freecell":
                ["forecell", "bakers_game",
                 "ko_bakers_game", "kings_only_bakers_game",
                 "relaxed_freecell", "eight_off"],
                "der_katz":
                ["der_katzenschwantz", "die_schlange"],
                "seahaven":
                ["seahaven_towers", "relaxed_seahaven",
                 "relaxed_seahaven_towers"],
                "bakers_dozen": None,
                "gypsy": None,
                "klondike":
                ["klondike_by_threes",
                 "casino_klondike", "small_harp", "thumb_and_pouch",
                 "vegas_klondike", "whitehead"],
                "simple_simon": None,
                "yukon": None,
                "beleaguered_castle":
                ["streets_and_alleys", "citadel"],
                "fan": None,
                "black_hole": None,
                "all_in_a_row": None,
        }

    def __init__(self, game_id, game_num, which_deals, print_ts):
        mymap = {}
        for k, v in self.REVERSE_MAP.items():
            for name in [k] + (v if v else []):
                mymap[name] = k
        self.games_map = mymap
        self.game_id = game_id
        self.game_num = game_num
        self.print_ts = print_ts
        self.which_deals = which_deals

    def is_two_decks(self):
        return self.game_id in ("der_katz", "der_katzenschwantz",
                                "die_schlange", "gypsy")

    def get_num_decks(self):
        return 2 if self.is_two_decks() else 1

    def print_layout(self):
        game_class = self.games_map[self.game_id]
        if not game_class:
            raise ValueError("Unknown game type " + self.game_id + "\n")

        self.deal()
        getattr(self, game_class)()
        self.board.output()


def find_index_main(args, find_ret):
    output_to_stdout = True
    is_ms = False
    while args[1][0] == '-':
        if (args[1] == "-o"):
            args.pop(0)
            if not len(args):
                raise ValueError("-o must accept an argument.")
            output_to_stdout = False
            args.pop(0)
        elif (args[1] == '--ms'):
            args.pop(0)
            is_ms = True
        elif (args[1] == '-'):
            break
        else:
            raise ValueError("Unknown flag " + args[1] + "!")

    if not is_ms:
        raise ValueError("only --ms is supported for now!")
    input_from_stdin = True
    input_fn = None
    if (len(args) >= 2):
        if (args[1] != "-"):
            input_fn = args[1]
            input_from_stdin = False
            args.pop(0)

    content = []
    if input_from_stdin:
        content = sys.stdin.readlines()
    else:
        with open(input_fn) as f:
            content = f.readlines()
    content = ''.join(content)

    rank_s = 'A23456789TJQK'
    rank_re = r'[' + rank_s + r']'
    suit_s = 'CSHD'
    suit_re = r'[' + suit_s + r']'

    card_re = rank_re + suit_re
    card_re_paren = r'(' + card_re + r')'

    def make_line(n):
        return r':?[ \t]*' + card_re_paren + \
            (r'[ \t]+' + card_re_paren) * (n-1) + r'[ \t]*\n'

    complete_re = r'^' + make_line(7) * 4 + make_line(6) * 4 + '\s*$'

    m = re.match(complete_re, content)
    if not m:
        raise ValueError("Could not match.")

    cards = [x.to_s() for x in ms_rearrange(createCards(1, True))]

    # Reverse shuffle:
    ints = []
    n = 4 * 13 - 1
    for i in range(n):
        col = i // 8
        row = i % 8
        s = m.group(1 + col + (4*7+(row-4)*6 if row >= 4 else row*7))
        idx = [j for j in range(n+1) if cards[j] == s]
        if len(idx) != 1:
            raise ValueError("Foo")
        j = idx[0]
        ints.append(j)
        cards[j] = cards[n]
        n -= 1

    ret = find_ret(ints)

    ret_code = 0
    if ret >= 0:
        if output_to_stdout:
            print("Found deal = %d" % ret)
        ret_code = 0
    else:
        print("Not found!")
        ret_code = -1
    return ret_code
