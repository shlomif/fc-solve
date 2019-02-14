#!/usr/bin/env python3
#
# Copyright by Shlomi Fish, 2018
#
# Licensed under the MIT/Expat License.
#
# This code is inspired by PySol by Markus F.X.J. Oberhumer and others.
# See: http://pysolfc.sourceforge.net/ .
import re


from pysol_cards.cards import CardRenderer, createCards, ms_rearrange


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
        string_repeat_for_transcrypt(make_line(6), 4) + '\\s*$'

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
