#!/usr/bin/env python3
#
# transpose-freecell-board.py - Program to transpose the input board from
# being playstacks-in-text-columns to being playstacks-in-lines.
#
# Copyright by Shlomi Fish, 2016
#
# Licensed under the MIT/X11 License.

# imports
import sys
import os
import re
import string
import time
import types
import random2


def shlomif_main(args):
    output_to_stdout = True
    output_fn = None
    while args[1][0] == '-':
        if (args[1] == "-o"):
            args.pop(0)
            if not len(args):
                raise ValueError("-o must accept an argument.")
            output_fn = args[1]
            output_to_stdout = False
            args.pop(0)
        elif (args[1] == '-'):
            break
        else:
            raise ValueError("Unknown flag " + args[1] + "!")

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

    layout = [[None for x in range(0,52)] for l in range(0,52)]

    rank_re = r'[A23456789TJQK]'
    suit_re = r'[HCDS]'

    card_re = rank_re + suit_re
    found_re = suit_re + '-' + '(?:0|' + rank_re + ')'

    freecell_re = (r'(?:%s|-)' % (card_re))

    line_num = 0

    def _find_indexes(myregex, too_many_msg, not_at_start_msg):
        idxs = [x for x in range(line_num, len(content)) if re.match(myregex, content[x])]

        if len(idxs) == 0:
            return None
        if len(idxs) != 1:
            raise ValueError(too_many_msg)

        i = idxs.pop(0)
        if (i != line_num):
            raise ValueError(not_at_start_msg)

        return i

    i = _find_indexes(r'^Foundations:', "There are too many \"Foundations:\" lines!", "The \"Foundations:\" line is not at the beginning!")
    foundations_line = None
    if i == line_num:
        foundations_line = content[line_num]
        if not re.match((r'^Foundations:(?:\s*%s)?(?:\s+%s)*\s*$' % (found_re, found_re)), foundations_line):
            raise ValueError("Wrong format for the foundations line.")
        foundations_line = re.sub(r'\s+$', '', foundations_line)
        line_num += 1

    i = _find_indexes(r'^Freecells:', "There are too many \"Freecells:\" lines!", "The \"Freecells:\" line is not at the beginning!")
    freecells_line = content[line_num]
    if not re.match((r'^Freecells:(\s*%s)?(?:\s+%s)*\s*$' % (freecell_re, freecell_re)), freecells_line):
        raise ValueError("Second line must be the freecells line.");
    freecells_line = re.sub(r'\s+$', '', freecells_line)
    line_num += 1

    start_line = line_num
    max_col = -1

    while line_num < len(content):
        l = content[line_num]
        x = 0
        pos = 0
        while x < len(l):
            s = l[x:min(x+3,len(l))]
            card = None
            if not re.match(r'^\s*$', s):
                m = re.match(r'^(' + card_re + r') ?$', s)
                if not m:
                    raise ValueError("Card " + s + " does not match pattern!");
                card = m.group(1)
            layout[pos][line_num - start_line] = card
            max_col = max(pos, max_col)
            x += 3
            pos += 1
        line_num += 1

    for idx in range(0,max_col+1):
        l = layout[idx]
        while not l[-1]:
            l.pop()
        for x in range(0, len(l)):
            if not l[x]:
                raise ValueError("Stack no. %d contains a gap at position no. %d. Aborting." % (idx+1, x+1))

    with (sys.stdout if output_to_stdout else open(output_fn)) as f:
        def _out_line(line):
            if line:
                f.write(line + "\n")
            return
        _out_line(foundations_line)
        _out_line(freecells_line)
        for l in layout[0:max_col+1]:
            _out_line(" ".join([":"] + l))

    return 0

if __name__ == "__main__":
    sys.exit(shlomif_main(sys.argv))
