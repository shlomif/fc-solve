#!/usr/bin/env python3
#
# find-freecell-deal-index.py - a program to find out the Freecell deal index
# based on the initial cards layout.
#
# Copyright by Shlomi Fish, 2018
#
# Licensed under the MIT/Expat License.

# imports
import sys
import re
from make_pysol_freecell_board import createCards, LCRandom31

if sys.version_info > (3,):
    long = int
    xrange = range


def shlomif_main(args):
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

    cards = createCards(1, True)
    c = []
    for i in range(13):
        for j in (0, 39, 26, 13):
            c.append(cards[i + j])
    cards = c

    # Reverse shuffle:
    ints = []
    n = 4 * 13 - 1
    for i in range(n):
        col = i // 8
        row = i % 8
        s = m.group(1 + col + (4*7+(row-4)*6 if row >= 4 else row*7))
        idx = [j for j in range(n+1) if cards[j].to_s() == s]
        if len(idx) != 1:
            raise ValueError("Foo")
        j = idx[0]
        ints.append(j)
        cards[n], cards[j] = cards[j], cards[n]
        n -= 1

    def is_right(d):
        r = LCRandom31()
        r.setSeed(d)
        n = len(ints)
        i = 0
        while n > 0:
            if r.randint(0, n) != ints[i]:
                return False
            i += 1
            n -= 1
        return True

    for d in xrange(1, (1 << 33)):
        if is_right(d):
            if output_to_stdout:
                print("Found deal = %d" % d)
            return 0
    print("Not found!")
    return -1


if __name__ == "__main__":
    sys.exit(shlomif_main(sys.argv))
