function _to_int(s: string): number {
    return parseInt(s, 10);
}

function _find_max_step(n: number): number {
    let x: number = 1;

    while (x << 1 < n) {
        x <<= 1;
    }

    return x;
}

function _render_move(my_move): string {
    const src = my_move.src.toString();
    const dest = my_move.dest.toString();
    if (my_move.t === "s2f") {
        return "Move a card from stack " + src + " to freecell " + dest;
    } else if (my_move.t === "s2s") {
        return "Move 1 cards from stack " + src + " to stack " + dest;
    } else {
        return "Move a card from freecell " + src + " to stack " + dest;
    }
}

class Expander {
    public modified_state = { f: [], c: [] };
    public empty_fc_indexes: number[] = [];
    public empty_stack_indexes: number[] = [];
    public ret_array: any[] = [];

    constructor() {
        return;
    }
    public populate_freecells(
        num_freecells: number,
        initial_src_state_str: string,
    ) {
        const expander = this;
        const freecell_match = initial_src_state_str.match(
            /\nFreecells:([^\n]*)\n/,
        );
        if (!freecell_match) {
            throw "Cannot match freecell exception.";
        }

        const freecell_string: string = freecell_match[1];

        if (freecell_string.length !== 4 * num_freecells) {
            throw "Miscount of freecells.";
        }

        for (let idx = 0; idx < num_freecells; ++idx) {
            const fc_s = freecell_string.substring(idx * 4, (idx + 1) * 4);
            if (fc_s === "    ") {
                expander.modified_state.f[idx] = null;
                expander.empty_fc_indexes.push(idx);
            } else {
                expander.modified_state.f[idx] = { t: "s", s: fc_s };
            }
        }
        return;
    }
    public perform_move(my_move) {
        const expander = this;
        const src = my_move.src;
        const dest = my_move.dest;
        if (my_move.t === "s2f") {
            expander.modified_state.f[dest] = {
                c: expander.modified_state.c[src].pop(),
                t: "c",
            };
        } else if (my_move.t === "s2s") {
            expander.modified_state.c[dest].push(
                expander.modified_state.c[src].pop(),
            );
        } else {
            if (expander.modified_state.f[src].t !== "c") {
                throw "Wrong val in " + src + "Freecell.";
            }
            expander.modified_state.c[dest].push(
                expander.modified_state.f[src].c,
            );
            expander.modified_state.f[src] = null;
        }

        return;
    }
}

export function fc_solve_expand_move(
    num_stacks: number,
    num_freecells: number,
    initial_src_state_str: string,
    initial_move,
    initial_dest_state_str: string,
) {
    const matched = initial_move.str.match(
        /^Move ([0-9]+) cards from stack ([0-9]+) to stack ([0-9]+)$/,
    );

    if (!matched) {
        return [initial_move];
    }

    const ultimate_num_cards = _to_int(matched[1]);

    // TODO : Implement the case where the sequence move is unlimited.
    if (ultimate_num_cards === 1) {
        return [initial_move];
    }

    const ultimate_source = _to_int(matched[2]);
    const ultimate_dest = _to_int(matched[3]);
    const expander = new Expander();

    // Need to process this move.
    expander.populate_freecells(num_freecells, initial_src_state_str);

    const col_matches = initial_src_state_str.match(/(\n:[^\n]+)/g);

    if (!col_matches || col_matches.length !== num_stacks) {
        throw "Miscount of stacks.";
    }

    for (let idx = 0; idx < num_stacks; ++idx) {
        let cards = col_matches[idx].match(/\w{2}/g);

        if (!cards) {
            cards = [];
        }

        expander.modified_state.c[idx] = cards;
        if (
            idx !== ultimate_dest &&
            idx !== ultimate_source &&
            cards.length === 0
        ) {
            expander.empty_stack_indexes.push(idx);
        }
    }

    const foundations_match = initial_src_state_str.match(
        /^(Foundations:[^\n]*\n)/,
    );

    if (!foundations_match) {
        throw "Cannot find foundations.";
    }

    const foundations_str = foundations_match[1];

    const num_cards_moved_at_each_stage = [];

    let num_cards = 0;
    num_cards_moved_at_each_stage.push(num_cards);
    const step_width = 1 + expander.empty_fc_indexes.length;
    while (
        (num_cards = Math.min(num_cards + step_width, ultimate_num_cards)) <
        ultimate_num_cards
    ) {
        num_cards_moved_at_each_stage.push(num_cards);
    }
    num_cards_moved_at_each_stage.push(num_cards);

    let output_state_promise = () => {
        return;
    };

    const past_first_output_state_promise = () => {
        const state_string =
            foundations_str +
            "Freecells:" +
            expander.modified_state.f
                .map((fc) => {
                    return !fc ? "    " : fc.t === "s" ? fc.s : "  " + fc.c;
                })
                .join("") +
            "\n" +
            expander.modified_state.c
                .map((col) => {
                    return ": " + col.join(" ") + "\n";
                })
                .join("");

        expander.ret_array.push({
            str: state_string,
            type: "s",
        });

        return;
    };

    function add_move(my_move) {
        output_state_promise();

        expander.ret_array.push({
            str: _render_move(my_move),
            type: "m",
        });

        expander.perform_move(my_move);

        output_state_promise = past_first_output_state_promise;

        return;
    }

    function move_using_freecells(source, dest, count) {
        const num_cards_thru_freecell = count - 1;
        for (let i = 0; i < num_cards_thru_freecell; ++i) {
            add_move({
                t: "s2f",
                src: source,
                dest: expander.empty_fc_indexes[i],
            });
        }
        add_move({ t: "s2s", src: source, dest });

        for (let i = num_cards_thru_freecell - 1; i >= 0; --i) {
            add_move({ t: "f2s", src: expander.empty_fc_indexes[i], dest });
        }

        return;
    }

    let recursive_move;
    recursive_move = (source, dest, num_cards_r, empty_cols) => {
        if (num_cards_r <= 0) {
            // Do nothing - the no-op.
            return;
        } else {
            const running_empty_cols = empty_cols.slice(0);
            const steps = [];

            while (Math.ceil(num_cards_r / step_width) > 1) {
                // Top power of two in num_steps
                const rec_num_steps = _find_max_step(
                    Math.ceil(num_cards_r / step_width),
                );
                const count_cards = rec_num_steps * step_width;
                const temp_dest = running_empty_cols.shift();
                recursive_move(
                    source,
                    temp_dest,
                    count_cards,
                    running_empty_cols,
                );

                steps.push({
                    count: count_cards,
                    dest: temp_dest,
                    source,
                });
                num_cards_r -= count_cards;
            }
            move_using_freecells(source, dest, num_cards_r);

            for (const s of steps.reverse()) {
                recursive_move(s.dest, dest, s.count, running_empty_cols);
                running_empty_cols.push(s.dest);
                running_empty_cols.sort((a, b) => {
                    return a - b;
                });
            }
            return;
        }
    };

    recursive_move(
        ultimate_source,
        ultimate_dest,
        ultimate_num_cards,
        expander.empty_stack_indexes,
    );

    return expander.ret_array;
}
