#!/usr/bin/env python3
#
# make_pysol_freecell_board.py - generate PySol's solitaire deals.
#
# Copyright by Shlomi Fish, 2018
#
# Licensed under the MIT/Expat License.
#
# This code is inspired by PySol by Markus F.X.J. Oberhumer and others.
# See: http://pysolfc.sourceforge.net/ .
import random2
import sys
from fc_solve_find_index_s2ints import createCards, Card, CardRenderer, \
        find_index__board_string_to_ints, ms_rearrange


def empty_card():
    ret = Card(0, 0, 0)
    ret.empty = True
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


class Columns:
    def __init__(self, num):
        self.num = num
        self.cols = [[] for _ in range(num)]

    def add(self, idx, card):
        self.cols[idx].append(card)

    def rev(self):
        self.cols.reverse()


class Board:
    def __init__(self, num_columns, with_freecells=False,
                 with_talon=False, with_foundations=False):
        self.with_freecells = with_freecells
        self.with_talon = with_talon
        self.with_foundations = with_foundations
        self.raw_foundations_line = None
        self.columns = Columns(num_columns)
        if self.with_freecells:
            self.freecells = []
        if self.with_talon:
            self.talon = []
        if self.with_foundations:
            self.foundations = [empty_card() for s in range(4)]
        self._lines = []

    def add_line(self, string):
        self._lines.append(string)

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

    def print_foundations(self, renderer):
        cells = []
        for f in [2, 0, 3, 1]:
            if not self.foundations[f].empty:
                cells.append(renderer.found_s(self.foundations[f]))

        if len(cells):
            self.add_line("Foundations:" + ("".join([" "+s for s in cells])))

    def gen_lines(self, renderer):
        self._lines = []
        if self.with_talon:
            self.add_line("Talon: " + renderer.l_concat(self.talon))
        if self.with_foundations:
            self.print_foundations(renderer)
        if self.raw_foundations_line:
            self.add_line(self.raw_foundations_line)
        if self.with_freecells:
            self.add_line("Freecells: " + renderer.l_concat(self.freecells))

        self._lines += [renderer.l_concat(c) for c in self.columns.cols]

    def calc_string(self, renderer):
        self.gen_lines(renderer)
        return "".join(l + "\n" for l in self._lines)


def shuffle(cards, game_num, which_deals):
    ms = ((game_num <= 32000) or (which_deals == RandomBase.DEALS_MS))
    r = LCRandom31() if ms else MTRandom() if which_deals == \
        RandomBase.DEALS_PYSOLFC else LCRandom64()
    r.setSeed(game_num)
    return r.shuffle(ms_rearrange(cards) if ms else cards)


class Game:
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

    def __init__(self, game_id, game_num, which_deals, max_rank=13):
        mymap = {}
        for k, v in self.REVERSE_MAP.items():
            for name in [k] + (v if v else []):
                mymap[name] = k
        self.games_map = mymap
        self.game_id = game_id
        self.game_num = game_num
        self.which_deals = which_deals
        self.max_rank = max_rank
        self.game_class = self.games_map[self.game_id]
        if not self.game_class:
            raise ValueError("Unknown game type " + self.game_id + "\n")

    def is_two_decks(self):
        return self.game_id in ("der_katz", "der_katzenschwantz",
                                "die_schlange", "gypsy")

    def get_num_decks(self):
        return 2 if self.is_two_decks() else 1

    def calc_deal_string(self, game_num, renderer):
        self.game_num = game_num
        self.deal()
        getattr(self, self.game_class)()
        return self.board.calc_string(renderer)

    def calc_layout_string(self, renderer):
        self.deal()
        getattr(self, self.game_class)()
        return self.board.calc_string(renderer)

    def print_layout(self, renderer):
        print(self.calc_layout_string(renderer), sep='', end='')

    def new_cards(self, cards):
        self.cards = cards
        self.card_idx = 0

    def deal(self):
        cards = shuffle(createCards(self.get_num_decks(),
                                    self.max_rank),
                        self.game_num, self.which_deals)
        cards.reverse()
        self.new_cards(cards)

    def __iter__(self):
        return self

    def no_more_cards(self):
        return self.card_idx >= len(self.cards)

    def __next__(self):
        if self.no_more_cards():
            raise StopIteration
        c = self.cards[self.card_idx]
        self.card_idx += 1
        return c

    def add(self, idx, card):
        self.board.add(idx, card)

    def add_freecell(self, card):
        self.board.add_freecell(card)

    def cyclical_deal(self, num_cards, num_cols, flipped=False):
        for i in range(num_cards):
            self.add(i % num_cols, next(self).flip(flipped=flipped))

    def add_all_to_talon(self):
        for c in self:
            self.board.add_talon(c)

    def add_empty_fc(self):
        self.add_freecell(empty_card())

    def _shuffleHookMoveSorter(self, cards, cb, ncards):
        extracted, i, new = [], len(cards), []
        for c in cards:
            select, ord_ = cb(c)
            if select:
                extracted.append((ord_, i, c))
                if len(extracted) >= ncards:
                    new += cards[(len(cards)-i+1):]
                    break
            else:
                new.append(c)
            i -= 1
        return new, [x[2] for x in reversed(sorted(extracted))]

    def _shuffleHookMoveToBottom(self, inp, cb, ncards=999999):
        cards, scards = self._shuffleHookMoveSorter(inp, cb, ncards)
        return scards + cards

    def _shuffleHookMoveToTop(self, inp, cb, ncards=999999):
        cards, scards = self._shuffleHookMoveSorter(inp, cb, ncards)
        return cards + scards

    def all_in_a_row(game):
        game.board = Board(13)
        game.cards = game._shuffleHookMoveToTop(
            game.cards,
            lambda c: (c.id == 13, c.suit),
            1)
        game.cyclical_deal(52, 13)
        game.board.raw_foundations_line = 'Foundations: -'

    def bakers_dozen(game):
        n = 13
        cards = list(reversed(game.cards))
        for i in [i for i, c in enumerate(cards) if c.is_king()]:
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
        game.board = Board(8, with_foundations=True)
        if game.game_id in ('beleaguered_castle', 'citadel'):
            new = []
            for c in game:
                if c.is_ace():
                    game.board.put_into_founds(c)
                else:
                    new.append(c)
            game.new_cards(new)
        for _ in range(6):
            for s in range(8):
                c = next(game)
                if not ((game.game_id == 'citadel') and
                        game.board.put_into_founds(c)):
                    game.add(s, c)
            if game.no_more_cards():
                break
        if game.game_id == 'streets_and_alleys':
            game.cyclical_deal(4, 4)

    def black_hole(game):
        game.board = Board(17)
        game.cards = game._shuffleHookMoveToBottom(
            game.cards,
            lambda c: (c.id == 13, c.suit),
            1)
        next(game)
        game.cyclical_deal(52-1, 17)
        game.board.raw_foundations_line = 'Foundations: AS'

    def der_katz(game):
        is_ds = game.game_id == 'die_schlange'
        if is_ds:
            print('Foundations: H-A S-A D-A C-A H-A S-A D-A C-A')
        game.board = Board(9)
        i = 0
        for c in game:
            if c.is_king():
                i += 1
            if not (is_ds and c.is_ace()):
                game.add(i, c)

    def fan(game):
        game.board = Board(18)
        game.cyclical_deal(52-1, 17)
        game.add(17, next(game))

    def freecell(game):
        is_fc = (game.game_id in ("forecell", "eight_off"))
        game.board = Board(8, with_freecells=is_fc)
        game.cyclical_deal(4 *
                           (game.max_rank-1 if is_fc else game.max_rank), 8)

        if is_fc:
            for c in game:
                game.add_freecell(c)
                if game.game_id == "eight_off":
                    game.add_empty_fc()

    def gypsy(game):
        num_cols = 8
        game.board = Board(num_cols, with_talon=True)
        game.cyclical_deal(num_cols*2, num_cols, flipped=True)
        game.cyclical_deal(num_cols, num_cols)
        game.add_all_to_talon()

    def klondike(game):
        num_cols = 7
        game.board = Board(num_cols, with_talon=True)
        for r in range(num_cols-1, 0, -1):
            game.cyclical_deal(r, r, flipped=True)
        game.cyclical_deal(num_cols, num_cols)
        game.add_all_to_talon()
        if not (game.game_id == 'small_harp'):
            game.board.reverse_cols()

    def seahaven(game):
        game.board = Board(10, with_freecells=True)
        game.cyclical_deal(50, 10)
        game.add_empty_fc()
        for c in game:
            game.add_freecell(c)

    def simple_simon(game):
        game.board = Board(10)
        for num_cards in range(9, 2, -1):
            game.cyclical_deal(num_cards, num_cards)
        game.cyclical_deal(10, 10)

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


def make_pysol_board__main(args):
    print_ts = False
    which_deals = RandomBase.DEALS_PYSOL
    max_rank = 13
    while args[1][0] == '-':
        a = args[1]
        if a == "-t":
            print_ts = True
            args.pop(1)
        elif (a == "--max-rank"):
            args.pop(1)
            max_rank = int(args.pop(1))
        elif (a == "--pysolfc") or (a == "-F"):
            which_deals = RandomBase.DEALS_PYSOLFC
            args.pop(1)
        elif (a == "--ms") or (a == "-M"):
            which_deals = RandomBase.DEALS_MS
            args.pop(1)
        else:
            raise ValueError("Unknown flag " + a + "!")

    game_num = int(args[1])
    which_game = args[2] if len(args) >= 3 else "freecell"
    Game(which_game, game_num, which_deals, max_rank).print_layout(
        CardRenderer(print_ts))


def find_index_main(args, find_ret):
    output_to_stdout = True
    output_fn = None
    is_ms = False
    while len(args) >= 2 and args[1][0] == '-':
        if args[1] == "-o":
            args.pop(1)
            if not len(args):
                raise ValueError("-o must accept an argument.")
            output_to_stdout = False
            output_fn = args.pop(1)
        elif args[1] == '--ms':
            args.pop(1)
            is_ms = True
        elif args[1] == '-':
            break
        else:
            raise ValueError("Unknown flag " + args[1] + "!")

    if not is_ms:
        raise ValueError("only --ms is supported for now!")
    input_from_stdin = True
    input_fn = None
    if len(args) >= 2:
        if args[1] != "-":
            input_fn = args.pop(1)
            input_from_stdin = False

    content = []
    if input_from_stdin:
        content = sys.stdin.readlines()
    else:
        with open(input_fn) as f:
            content = f.readlines()

    content = ''.join(content)
    ints = find_index__board_string_to_ints(content)
    ret = find_ret(ints)

    ret_code = 0
    if ret >= 0:
        s = "Found deal = %d" % ret
        if output_to_stdout:
            print(s)
        else:
            with open(output_fn, 'w') as f:
                f.write("%s\n" % s)
        ret_code = 0
    else:
        print("Not found!")
        ret_code = -1
    return ret_code


if __name__ == "__main__":
    sys.exit(make_pysol_board__main(sys.argv))
