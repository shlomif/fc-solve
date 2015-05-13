"use strict";

function fc_solve_expand_move (num_stacks, num_freecells, initial_src_state_str, initial_move, initial_dest_state_str) {

    var matched = initial_move.match(/^Move ([0-9]+) cards from stack ([0-9]+) to stack ([0-9]+)$/);

    if (! matched) {
        return [initial_move];
    }

    var to_int = function(s) { return parseInt(s, 10); };
    var ultimate_num_cards = to_int(matched[1]);

    // TODO : Implement the case where the sequence move is unlimited.
    if (ultimate_num_cards == 1) {
        return [initial_move];
    }

    var ultimate_source = to_int(matched[2]);
    var ultimate_dest = to_int(matched[3]);

    // Need to process this move.
    var empty_fc_indexes = [];
    var empty_stack_indexes = [];

    var freecell_match = initial_src_state_str.match(/\nFreecells:([^\n]*)\n/);
    if (!freecell_match) {
        throw "Cannot match freecell exception.";
    }

    var freecell_string = freecell_match[1];

    if (freecell_string.length != 4 * num_freecells) {
        throw "Miscount of freecells.";
    }

    for (var idx = 0 ; idx < num_freecells ; idx++) {
        if (freecell_string.substring(idx*4, (idx+1)*4) == "    ") {
            empty_fc_indexes.push(idx);
        }
    }

    var col_matches = initial_src_state_str.match(/(\n:[^\n]+)/g);

    if ((!col_matches) || (col_matches.length != num_stacks)) {
        throw "Miscount of stacks.";
    }

    for (var idx = 0 ; idx < num_stacks ; idx++) {
        if ((idx != ultimate_dest) && (idx != ultimate_source) &&
            col_matches[idx].match(/^\n:\s*$/) {
                empty_stack_indexes.push(idx);
        }
    }

    var num_cards_moved_at_each_stage = [];

    var num_cards = 0;
    num_cards_moved_at_each_stage.push( num_cards );
    var step_width = 1 + empty_fc_indexes.length;
    while (
        (num_cards = Math.min(
                num_cards + step_width,
                ultimate_num_cards
        ))
        < ultimate_num_cards
    ) {
        num_cards_moved_at_each_stage.push( num_cards );
    }
    num_cards_moved_at_each_stage.push( num_cards );

}
