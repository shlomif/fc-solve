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
import sys

from fc_solve_find_index_s2ints import find_index__board_string_to_ints

from pysol_cards.cards import CardRenderer
from pysol_cards.deal_game import Game
from pysol_cards.single_deal_args_parse import SingleDealArgsParser


def make_pysol_board__main(args):
    obj = SingleDealArgsParser(args)
    Game(obj.which_game, obj.game_num,
         obj.which_deals, obj.max_rank
         ).print_layout(CardRenderer(obj.print_ts))


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
        with open(input_fn, "rt") as f:
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
