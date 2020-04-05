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
 * Under MIT/Expat Licence.
 *
 * */

function deal_ms_fc_board(seed) {
    const randomizer = new MSRand({gamenumber: seed});
    const num_cols = 8;

    function _perl_range(start, end) {
        const ret = [];

        for (let i = start; i <= end; i++) {
            ret.push(i);
        }

        return ret;
    };

    const columns = _perl_range(0, num_cols-1).map(function() {
        return [];
    });
    let deck = _perl_range(0, 4*13-1);

    randomizer.shuffle(deck);

    deck = deck.reverse();

    for (let i = 0; i < 52; i++) {
        columns[i % num_cols].push(deck[i]);
    }

    function render_card(card) {
        const suit = (card % 4);
        const rank = Math.floor(card / 4);

        return "A23456789TJQK".charAt(rank) + "CDHS".charAt(suit);
    };

    function render_column(col) {
        return ": " + col.map(render_card).join(" ") + "\n";
    };

    return columns.map(render_column).join("");
}
