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
import re


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


def ms_rearrange(cards):
    if len(cards) != 52:
        return cards
    c = []
    for i in range(13):
        for j in (0, 39, 26, 13):
            c.append(cards[i + j])
    return c


def find_index__board_string_to_ints(content):
    rank_s = 'A23456789TJQK'
    rank_re = r'[' + rank_s + r']'
    suit_s = 'CSHD'
    suit_re = r'[' + suit_s + r']'

    card_re = rank_re + suit_re
    card_re_paren = r'(' + card_re + r')'

    def string_repeat_for_transcrypt(s, n):
        if n == 0:
            return ''
        return s + string_repeat_for_transcrypt(s, n-1)

    def make_line(n):
        return r':?[ \t]*' + card_re_paren + \
            string_repeat_for_transcrypt((r'[ \t]+' + card_re_paren), n-1) + \
            r'[ \t]*\n'

    complete_re = r'^' + string_repeat_for_transcrypt(make_line(7), 4) + \
        string_repeat_for_transcrypt(make_line(6), 4) + '\s*$'

    m = re.match(complete_re, content)
    if not m:
        raise ValueError("Could not match.")

    cards = [x.to_s() for x in ms_rearrange(createCards(1, True))]

    # Reverse shuffle:
    ints = []
    limit = n = 4 * 13 - 1
    for i in range(limit):
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

    return ints


def factory():
    return {'find_index__board_string_to_ints':
            find_index__board_string_to_ints}
