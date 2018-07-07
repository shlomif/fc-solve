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


class Card:
    ACE = 1
    KING = 13

    def __init__(self, id, rank, suit):
        self.id, self.rank, self.suit = id, rank, suit
        self.empty, self.flipped = False, False

    def suit_s(self):
        return 'CSHD'[self.suit]

    def is_ace(self):
        return self.rank == self.ACE

    def is_king(self):
        return self.rank == self.KING

    def flip(self, flipped=True):
        ret = Card(self.id, self.rank, self.suit)
        ret.flipped = flipped
        return ret


class CardRenderer:
    """docstring for CardRenderer"""
    def __init__(self, print_ts):
        self.print_ts = print_ts

    def to_s(self, card):
        if card.empty:
            return '-'
        ret = self.rank_s(card) + card.suit_s()
        if card.flipped:
            ret = '<' + ret + '>'
        return ret

    def found_s(self, card):
        return card.suit_s() + '-' + self.rank_s(card)

    def rank_s(self, card):
        ret = "0A23456789TJQK"[card.rank]
        if ((not self.print_ts) and ret == 'T'):
            ret = '10'
        return ret

    def render_l(self, lst):
        return [self.to_s(x) for x in lst]

    def l_concat(self, lst):
        return ' '.join(self.render_l(lst))


def createCards(num_decks, max_rank=13):
    ret = []
    for _ in range(num_decks):
        id = 0
        for s in range(4):
            for r in range(max_rank):
                ret.append(Card(id, r+1, s))
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
    if len(content) > 0 and content[-1] != '\n':
        content += '\n'

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

    cards = CardRenderer(True).render_l(ms_rearrange(createCards(1)))

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
