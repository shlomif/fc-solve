#!/usr/bin/env python3
#
# transpose-freecell-board.py - Program to transpose the input board from
# being playstacks-in-text-columns to being playstacks-in-lines.
#
# Copyright by Shlomi Fish, 2016
#
# Licensed under the MIT/Expat License.

import sys
import re


class LinesHandler:
    def __init__(self, content):
        """docstring for __init__"""
        self.content = content
        self.line_num = 0

    def _find_indices(self, myregex, too_many_msg, not_at_start_msg):
        idxs = [x for x in range(self.line_num, len(self.content))
                if re.match(myregex, self.content[x])]

        if len(idxs) == 0:
            return None
        if len(idxs) != 1:
            raise ValueError(too_many_msg)

        i = idxs.pop(0)
        if (i != self.line_num):
            raise ValueError(not_at_start_msg)

        return i

    def _extract_optional_line_from_start(self, validregex, wrong_fmt_msg,
                                          myregex, too_many_msg,
                                          not_at_start_msg):
        i = self._find_indices(myregex, too_many_msg, not_at_start_msg)
        if i == self.line_num:
            line = self.content[self.line_num]
            if not re.match(validregex, line):
                raise ValueError(wrong_fmt_msg)
            self.line_num += 1
            return re.sub(r'\s+$', '', line)
        else:
            return None


def cmd_line_main(args):
    output_to_stdout = True
    output_fn = None
    while args[1][0] == '-':
        if args[1] == "-o":
            args.pop(0)
            if not len(args):
                raise ValueError("-o must accept an argument.")
            output_fn = args[1]
            output_to_stdout = False
            args.pop(0)
        elif args[1] == '-':
            break
        else:
            raise ValueError("Unknown flag " + args[1] + "!")

    input_from_stdin = True
    input_fn = None
    if len(args) >= 2:
        if args[1] != "-":
            input_fn = args[1]
            input_from_stdin = False
            args.pop(0)

    content = []
    if input_from_stdin:
        content = sys.stdin.readlines()
    else:
        with open(input_fn) as f:
            content = f.readlines()

    layout = [[None for x in range(52)] for line in range(52)]

    rank_re = r'[A23456789TJQK]'
    suit_re = r'[HCDS]'

    card_re = rank_re + suit_re
    found_re = suit_re + '-' + '(?:0|' + rank_re + ')'

    freecell_re = (r'(?:%s|-)' % (card_re))

    h = LinesHandler(content)
    foundations_line = h._extract_optional_line_from_start(
            (r'^Foundations:(?:\s*%s)?(?:\s+%s)*\s*$' % (found_re, found_re)),
            "Wrong format for the foundations line.",
            r'^Foundations:',
            "There are too many \"Foundations:\" lines!",
            "The \"Foundations:\" line is not at the beginning!"
    )

    freecells_line = h._extract_optional_line_from_start(
        (r'^Freecells:(\s*%s)?(?:\s+%s)*\s*$' % (freecell_re, freecell_re)),
        "Wrong format for the freecells line.",
        r'^Freecells:',
        "There are too many \"Freecells:\" lines!",
        "The \"Freecells:\" line is not at the beginning!"
    )

    start_line = h.line_num
    max_col = -1

    while h.line_num < len(content):
        line = content[h.line_num]
        x = 0
        pos = 0
        while x < len(line):
            s = line[x:min(x+3, len(line))]
            card = None
            if not re.match(r'^\s*$', s):
                m = re.match(r'^(' + card_re + r') ?$', s)
                if not m:
                    raise ValueError("Card " + s + " does not match pattern!")
                card = m.group(1)
            if pos >= len(layout):
                raise ValueError("Line %d is too long!" % (h.line_num + 1))
            l_idx = h.line_num - start_line
            if l_idx >= len(layout[pos]):
                raise ValueError("Too many lines!")
            layout[pos][l_idx] = card
            max_col = max(pos, max_col)
            x += 3
            pos += 1
        h.line_num += 1

    for idx in range(max_col + 1):
        line = layout[idx]
        while not line[-1]:
            line.pop()
        for x in range(len(line)):
            if not line[x]:
                raise ValueError(
                    "Stack no. %d contains a gap at position no. %d. Aborting."
                    % (idx+1, x+1)
                )

    with (sys.stdout if output_to_stdout else open(output_fn)) as f:
        def _out_line(line):
            if line:
                f.write(line + "\n")
            return
        _out_line(foundations_line)
        _out_line(freecells_line)
        for line in layout[0:max_col+1]:
            _out_line(" ".join([":"] + line))

    return 0


if __name__ == "__main__":
    sys.exit(cmd_line_main(sys.argv))
