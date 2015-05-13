"use strict";

function fc_solve_expand_move (num_stacks, num_freecells, initial_src_state_str, initial_move, initial_dest_state_str) {

    var matched = initial_move.str.match(/^Move ([0-9]+) cards from stack ([0-9]+) to stack ([0-9]+)$/);

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

    var modified_state = {f:[],c:[]};

    var freecell_match = initial_src_state_str.match(/\nFreecells:([^\n]*)\n/);
    if (!freecell_match) {
        throw "Cannot match freecell exception.";
    }

    var freecell_string = freecell_match[1];

    if (freecell_string.length != 4 * num_freecells) {
        throw "Miscount of freecells.";
    }

    for (var idx = 0 ; idx < num_freecells ; idx++) {
        var fc_s = freecell_string.substring(idx*4, (idx+1)*4);
        if (fc_s == "    ") {
            modified_state.f[idx] = null;
            empty_fc_indexes.push(idx);
        } else {
            modified_state.f[idx] = {t: 's', s: fc_s};
        }
    }

    var col_matches = initial_src_state_str.match(/(\n:[^\n]+)/g);

    if ((!col_matches) || (col_matches.length != num_stacks)) {
        throw "Miscount of stacks.";
    }

    for (var idx = 0 ; idx < num_stacks ; idx++) {
        var cards = col_matches[idx].match(/\w{2}/g);

        modified_state.c[idx] = cards;
        if ((idx != ultimate_dest) && (idx != ultimate_source) &&
            (card.length == 0)) {
                empty_stack_indexes.push(idx);
        }
    }

    var foundations_match = initial_src_state_str.match(/^(Foundations:[^\n]*\n)/);

    if (! foundations_match) {
        throw "Cannot find foundations.";
    }

    var foundations_str = foundations_match[1];

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

    var ret_array = [];


    var output_state_promise = function() { return; };

    var past_first_output_state_promise = function() {
        var state_string = foundations_str +
            "Freecells: " + (modified_state.f.map(function (fc) {
            return ((!fc) ? '    ' : (fc.t == 's') ? fc.s : ("  " + fc.c));
        }).join("")) + "\n" + (modified_state.c.map(function(col) {
            return ": " + col.join(" ") + "\n";
        }).join(""));
        ret_array.push(
            {
                type: 's',
                str: state_string,
            }
        );
    };

    return ret_array;
}
