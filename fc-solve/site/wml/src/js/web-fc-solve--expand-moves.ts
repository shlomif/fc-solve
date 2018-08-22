export function fc_solve_expand_move(num_stacks, num_freecells,
    initial_src_state_str, initial_move, initial_dest_state_str) {
    const matched = initial_move.str.match(
        /^Move ([0-9]+) cards from stack ([0-9]+) to stack ([0-9]+)$/
    );

    if (! matched) {
        return [initial_move];
    }

    function to_int(s) {
        return parseInt(s, 10);
    };
    const ultimate_num_cards = to_int(matched[1]);

    // TODO : Implement the case where the sequence move is unlimited.
    if (ultimate_num_cards == 1) {
        return [initial_move];
    }

    const ultimate_source = to_int(matched[2]);
    const ultimate_dest = to_int(matched[3]);

    // Need to process this move.
    let empty_fc_indexes = [];
    let empty_stack_indexes = [];

    let modified_state = {f: [], c: []};

    const freecell_match = initial_src_state_str.match(
        /\nFreecells:([^\n]*)\n/
    );
    if (!freecell_match) {
        throw "Cannot match freecell exception.";
    }

    const freecell_string = freecell_match[1];

    if (freecell_string.length != 4 * num_freecells) {
        throw "Miscount of freecells.";
    }

    for (let idx = 0; idx < num_freecells; ++idx) {
        const fc_s = freecell_string.substring(idx*4, (idx+1)*4);
        if (fc_s == "    ") {
            modified_state.f[idx] = null;
            empty_fc_indexes.push(idx);
        } else {
            modified_state.f[idx] = {t: 's', s: fc_s};
        }
    }

    const col_matches = initial_src_state_str.match(/(\n:[^\n]+)/g);

    if ((!col_matches) || (col_matches.length != num_stacks)) {
        throw "Miscount of stacks.";
    }

    for (let idx = 0; idx < num_stacks; ++idx) {
        let cards = col_matches[idx].match(/\w{2}/g);

        if (!cards) {
            cards = [];
        }

        modified_state.c[idx] = cards;
        if ((idx != ultimate_dest) && (idx != ultimate_source) &&
            (cards.length == 0)) {
                empty_stack_indexes.push(idx);
        }
    }

    const foundations_match = initial_src_state_str.match(
        /^(Foundations:[^\n]*\n)/
    );

    if (! foundations_match) {
        throw "Cannot find foundations.";
    }

    const foundations_str = foundations_match[1];

    let num_cards_moved_at_each_stage = [];

    let num_cards = 0;
    num_cards_moved_at_each_stage.push( num_cards );
    const step_width = 1 + empty_fc_indexes.length;
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

    let ret_array = [];


    let output_state_promise = function() {
        return;
    };

    let past_first_output_state_promise = function() {
        let state_string = foundations_str +
            "Freecells:" + (modified_state.f.map(function(fc) {
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

        return;
    };

    function render_move(my_move) {
        const src = my_move.src.toString();
        const dest = my_move.dest.toString();
        if (my_move.t == 's2f') {
            return ("Move a card from stack " + src + " to freecell " + dest);
        } else if (my_move.t == 's2s') {
            return ("Move 1 cards from stack " + src + " to stack " + dest);
        } else {
            return ("Move a card from freecell " + src + " to stack " + dest);
        }
    };
    function perform_move(my_move) {
        const src = my_move.src;
        const dest = my_move.dest;
        if (my_move.t == 's2f') {
            modified_state.f[dest] = {
                t: 'c',
                c: modified_state.c[src].pop(),
            };
        } else if (my_move.t == 's2s') {
            modified_state.c[dest].push(modified_state.c[src].pop());
        } else {
            if (modified_state.f[src].t != 'c') {
                throw "Wrong val in " + src + "Freecell.";
            }
            modified_state.c[dest].push(modified_state.f[src].c);
            modified_state.f[src] = null;
        }

        return;
    };
    function add_move(my_move) {
        output_state_promise();

        ret_array.push(
            {
                type: 'm',
                str: render_move(my_move),
            }
        );

        perform_move(my_move);

        output_state_promise = past_first_output_state_promise;

        return;
    };

    function move_using_freecells(source, dest, count) {
        const num_cards_thru_freecell = count - 1;
        for (let i=0; i < num_cards_thru_freecell; ++i) {
            add_move(
                {t: 's2f', src: source, dest: empty_fc_indexes[i]}
            );
        }
        add_move({t: 's2s', src: source, dest: dest});

        for (let i = num_cards_thru_freecell-1; i >= 0; --i) {
            add_move(
                {t: 'f2s', src: empty_fc_indexes[i], dest: dest}
            );
        }

        return;
    };

    function _find_max_step(n) {
        let x = 1;

        while ((x << 1) < n) {
            x <<= 1;
        }

        return x;
    };

    let recursive_move;
    recursive_move = function(source, dest, num_cards, empty_cols) {
        if (num_cards <= 0) {
            // Do nothing - the no-op.
            return;
        } else {
            let running_empty_cols = empty_cols.slice(0);
            let steps = [];

            while (Math.ceil(num_cards / step_width) > 1) {
                // Top power of two in num_steps
                const rec_num_steps = _find_max_step(
                    Math.ceil( num_cards / step_width )
                );
                const count_cards = rec_num_steps * step_width;
                const temp_dest = running_empty_cols.shift();
                recursive_move(
                    source,
                    temp_dest,
                    count_cards,
                    running_empty_cols
                );

                steps.push({
                    source: source,
                    dest: temp_dest,
                    count: count_cards,
                });
                num_cards -= count_cards;
            }
            move_using_freecells(source, dest, num_cards);

            steps.reverse().forEach(function(s) {
                recursive_move(
                    s.dest,
                    dest,
                    s.count,
                    running_empty_cols
                );
                running_empty_cols.push(s.dest);
                running_empty_cols.sort(function(a, b) {
                    return a - b;
                });
            });
            return;
        }
    };

    recursive_move(
        ultimate_source, ultimate_dest,
        ultimate_num_cards,
        empty_stack_indexes
    );

    return ret_array;
}
