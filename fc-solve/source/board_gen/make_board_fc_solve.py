#!/usr/bin/env python3
#
# make_board_fc_solve.py - Expat-licensed routines for the board-gen code.
#
# Copyright by Shlomi Fish, 2018
#
# Licensed under the MIT/Expat License.


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
    def shuffle(self, seq):
        for n in range(len(seq)-1, 0, -1):
            j = self.randint(0, n)
            seq[n], seq[j] = seq[j], seq[n]

    def randint(self, a, b):
        return a + int(self.random() * (b+1-a))


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


def ms_rearrange(cards):
    if len(cards) != 52:
        return cards
    c = []
    for i in range(13):
        for j in (0, 39, 26, 13):
            c.append(cards[i + j])
    return c
