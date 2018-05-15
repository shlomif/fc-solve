"use strict";

define(['dist/fc_solve_find_index_s2ints'], function(s2i) {
    function find_deal_ui() {
        const input_str = `# MS Freecell Deal #245
#
: 2H 7H 4D 8D 8H 7C TD
: 2C 4S AD 8S JD AC 4C
: KC 9C 2D 5D KH 6C QH
: TH JH 9H 5S TS 3D JS
: 6H 2S JC 7D QC 9S
: 7S 9D 3S KD 5C 3H
: AH AS QD 6S 5H 3C
: TC 8C KS QS 6D 4H`;

        const deal_str = input_str.replace(/#[^\r\n]*\r?\n?/g, '').
            replace(/\r+\n/, "\n").replace(/([^\n])$/, "$1\n");
        let ints;
        try {
            ints = s2i.find_index__board_string_to_ints(deal_str);
            alert(ints.map((i) => i.toString()).join(','));
        } catch (e) {
            alert(e);
        }

        return;
    }

    find_deal_ui();

    return find_deal_ui;
});
