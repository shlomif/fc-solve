#!/usr/bin/env python3
#
# gen_solvitaire_json_board.py - generate PySol FC's solitaire deals as
# Solvitaire-accepted JSON.
#
# See:
#
# https://github.com/thecharlesblake/Solvitaire
#
# Copyright by Shlomi Fish, 2018
#
# Licensed under the MIT/Expat License.
#
# This code is inspired by PySol by Markus F.X.J. Oberhumer and others.
# See: http://pysolfc.sourceforge.net/ .

import json
import sys

from pysol_cards.cards import CardRenderer
from pysol_cards.deal_game import Game
from pysol_cards.single_deal_args_parse import SingleDealArgsParser


class JsonRenderer(CardRenderer):
    def to_s(self, card):
        if card.empty:
            return '-'
        ret = self.rank_s(card) + card.suit_s()
        return ret


def make_pysol_board__main(args):
    obj = SingleDealArgsParser(args)
    g = Game(obj.which_game, obj.game_num, obj.which_deals, obj.max_rank)
    g.deal()
    getattr(g, g.game_class)()
    renderer = JsonRenderer(obj.print_ts)
    je = json.JSONEncoder(sort_keys=True)
    json_obj = {}
    json_obj["tableau piles"] = [
        renderer.render_l(x) for x in g.board.columns.cols]
    try:
        talon = renderer.render_l(g.board.talon)
        json_obj["stock"] = talon
    except AttributeError:
        pass
    print(je.encode(json_obj))


if __name__ == "__main__":
    sys.exit(make_pysol_board__main(sys.argv))
