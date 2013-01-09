"use strict";
/*
 * Microsoft Windows Freecell / Freecell Pro boards generation.
 *
 * See:
 *
 * - http://rosettacode.org/wiki/Deal_cards_for_FreeCell
 *
 * - http://www.solitairelaboratory.com/mshuffle.txt
 *
 * Under MIT/X11 Licence.
 *
 * */

function deal_ms_fc_board(seed) {
    var randomizer = new MSRand({ seed: seed });
    var num_cols = 8;

    var _perl_range = function(start, end) {
        var ret = [];

        for (var i = start; i <= end; i++) {
            ret.push(i);
        }

        return ret;
    };

    var columns = _perl_range(0, num_cols-1).map(function () { return []; });
    var deck = _perl_range(0, 4*13-1);

    randomizer.shuffle(deck);

    deck = deck.reverse()

    for (var i = 0; i < 52; i++) {
        columns[i % num_cols].push(deck[i]);
    }

    var render_card = function (card) {
        var suit = (card % 4);
        var rank = Math.floor(card / 4);

        return "A23456789TJQK".charAt(rank) + "CDHS".charAt(suit);
    }

    var render_column = function(col) {
        return ": " + col.map(render_card).join(" ") + "\n";
    }

    return columns.map(render_column).join("");
}
