import * as bigInt from 'big-integer';
import * as validate from "./fcs-validate";
import * as expand from "./web-fc-solve--expand-moves";

const fc_solve_expand_move = expand.fc_solve_expand_move;

let fc_solve__hll_ms_rand__get_singleton = null;
let fc_solve__hll_ms_rand__init = null;
let fc_solve__hll_ms_rand__mod_rand = null;
let fc_solve_user__find_deal__alloc = null;
let fc_solve_user__find_deal__fill = null;
let fc_solve_user__find_deal__free = null;
let fc_solve_user__find_deal__run = null;
let freecell_solver_user_alloc = null;
let freecell_solver_user_solve_board = null;
let freecell_solver_user_resume_solution = null;
let freecell_solver_user_cmd_line_read_cmd_line_preset = null;
let malloc = null;
let c_free = null;
let freecell_solver_user_get_next_move = null;
let freecell_solver_user_get_num_freecells = null;
let freecell_solver_user_get_num_stacks = null;
let freecell_solver_user_current_state_stringify = null;
let freecell_solver_user_stringify_move_ptr = null;
let freecell_solver_user_free = null;
let freecell_solver_user_limit_iterations_long = null;
let freecell_solver_user_get_invalid_state_error_into_string = null;
let freecell_solver_user_cmd_line_parse_args_with_file_nesting_count = null;
let fc_solve_Pointer_stringify = null;
let fc_solve_FS_writeFile = null;
let fc_solve_getValue = null;
let fc_solve_setValue = null;
let fc_solve_intArrayFromString = null;
let fc_solve_allocate_i8 = null;

export function FC_Solve_init_wrappers_with_module(Module) {
    fc_solve__hll_ms_rand__get_singleton =
        Module.cwrap('fc_solve__hll_ms_rand__get_singleton', 'number', []);
    fc_solve__hll_ms_rand__init =
        Module.cwrap('fc_solve__hll_ms_rand__init', 'number',
            ['number', 'string']);
    fc_solve__hll_ms_rand__mod_rand =
        Module.cwrap('fc_solve__hll_ms_rand__mod_rand', 'number',
            ['number', 'number']);
    fc_solve_user__find_deal__alloc =
        Module.cwrap('fc_solve_user__find_deal__alloc', 'number', []);
    fc_solve_user__find_deal__fill =
        Module.cwrap('fc_solve_user__find_deal__fill', 'number',
            ['number', 'string']);
    fc_solve_user__find_deal__free =
        Module.cwrap('fc_solve_user__find_deal__free', 'number', ['number']);
    fc_solve_user__find_deal__run =
        Module.cwrap('fc_solve_user__find_deal__run', 'string',
            ['number', 'string', 'string']);
    freecell_solver_user_alloc =
        Module.cwrap('freecell_solver_user_alloc', 'number', []);
    freecell_solver_user_solve_board =
        Module.cwrap('freecell_solver_user_solve_board', 'number',
            ['number', 'string']);
    freecell_solver_user_resume_solution =
        Module.cwrap('freecell_solver_user_resume_solution', 'number',
            ['number']);
    freecell_solver_user_cmd_line_read_cmd_line_preset =
        Module.cwrap('freecell_solver_user_cmd_line_read_cmd_line_preset',
            'number', ['number', 'string', 'number', 'number', 'number',
                'string']);
    malloc = Module.cwrap('malloc', 'number', ['number']);
    c_free = Module.cwrap('free', 'number', ['number']);
    freecell_solver_user_get_next_move =
        Module.cwrap('freecell_solver_user_get_next_move', 'number',
            ['number', 'number']);
    freecell_solver_user_get_num_freecells =
        Module.cwrap('freecell_solver_user_get_num_freecells', 'number',
            ['number']);
    freecell_solver_user_get_num_stacks =
        Module.cwrap('freecell_solver_user_get_num_stacks', 'number',
            ['number']);
    freecell_solver_user_current_state_stringify =
        Module.cwrap('freecell_solver_user_current_state_stringify',
            'number', ['number', 'number', 'number', 'number',
                'number']);
    freecell_solver_user_stringify_move_ptr =
        Module.cwrap('freecell_solver_user_stringify_move_ptr', 'number',
            ['number', 'number', 'number', 'number']);
    freecell_solver_user_free = Module.cwrap('freecell_solver_user_free',
        'number', ['number']);
    freecell_solver_user_limit_iterations_long =
        Module.cwrap('freecell_solver_user_limit_iterations_long', 'number',
            ['number', 'number']);
    freecell_solver_user_get_invalid_state_error_into_string =
        Module.cwrap(
            'freecell_solver_user_get_invalid_state_error_into_string',
            'number', ['number', 'number', 'number']);
    freecell_solver_user_cmd_line_parse_args_with_file_nesting_count =
        Module.cwrap(
            'freecell_solver_user_cmd_line_parse_args_with_file_nesting_count',
            'number', ['number', 'number', 'number', 'number', 'number', 'number',
                'number', 'number', 'number', 'number', 'number']);

    fc_solve_Pointer_stringify = (ptr) => {
        return Module.Pointer_stringify(ptr);
    };
    fc_solve_FS_writeFile = (p1, p2, p3) => {
        return Module.FS.writeFile(p1, p2, p3);
    };
    fc_solve_getValue = (p1, p2) => {
        return Module.getValue(p1, p2);
    };
    fc_solve_setValue = (p1, p2, p3) => {
        return Module.setValue(p1, p2, p3);
    };
    fc_solve_intArrayFromString = (s) => {
        return Module.intArrayFromString(s);
    };
    fc_solve_allocate_i8 = (p1) => {
        return Module.allocate(p1, 'i8', Module.ALLOC_STACK);
    };

    return;
}

function alloc_wrap(size, desc, error) {
    const ret = malloc(size);
    if (ret === 0) {
        alert("Could not allocate " + desc + " (out of memory?)");
        throw error;
    }
    return ret;
}

const remove_trailing_space_re = /[ \t]+$/gm;

export const FCS_STATE_WAS_SOLVED = 0;
const FCS_STATE_IS_NOT_SOLVEABLE = 1;
const FCS_STATE_ALREADY_EXISTS = 2;
const FCS_STATE_EXCEEDS_MAX_NUM_TIMES = 3;
const FCS_STATE_BEGIN_SUSPEND_PROCESS = 4;
export const FCS_STATE_SUSPEND_PROCESS = 5;
const FCS_STATE_EXCEEDS_MAX_DEPTH = 6;
const FCS_STATE_ORIGINAL_STATE_IS_NOT_SOLVEABLE = 7;
const FCS_STATE_INVALID_STATE = 8;
const FCS_STATE_NOT_BEGAN_YET = 9;
const FCS_STATE_DOES_NOT_EXIST = 10;
const FCS_STATE_OPTIMIZED = 11;
const FCS_STATE_FLARES_PLAN_ERROR = 12;

const iters_step = 1000;
const upper_iters_limit = 128 * 1024;

const fc_solve_2uni_suit_map = {H: '♥', C: '♣', D: '♦', S: '♠'};
const fc_solve_2uni_suit_map_num = {H: 1, C: 3, D: 2, S: 0};

function fc_solve_2uni_card(match, p1, p2, offset, mystring) {
    return p1 + fc_solve_2uni_suit_map[p2];
}

function fc_solve_2uni_char_card(match, p1, p2, offset, mystring) {
    const rank = validate.ranks__str_to_int[p1];

    const ret = String.fromCodePoint(
        fc_solve_2uni_suit_map_num[p2] * 16 + 0x1F0A0 + rank,
    );
    return ret;
}

function fc_solve_2uni_found(match, p1, p2, offset, mystring) {
    return fc_solve_2uni_suit_map[p1] + p2;
}

export class FC_Solve {
    private dir_base: string;
    private string_params: string[];
    private set_status_callback: any;
    private is_unicode_cards: boolean;
    private is_unicode_cards_chars: boolean;
    private cmd_line_preset: string;
    private current_iters_limit: number;
    private obj: any;
    private _proto_states_and_moves_seq: any[];
    private _pre_expand_states_and_moves_seq: any;
    private _post_expand_states_and_moves_seq: any;
    private _state_string_buffer: number;
    private _move_string_buffer: number;

    constructor(args) {
        const that = this;

        that.dir_base = args.dir_base;
        that.string_params = args.string_params;
        that.set_status_callback = args.set_status_callback;
        that.is_unicode_cards = (args.is_unicode_cards || false);
        that.is_unicode_cards_chars = (args.is_unicode_cards_chars || false);
        that.cmd_line_preset = args.cmd_line_preset;
        that.current_iters_limit = 0;
        that.obj = ( () => {
            const ret_obj = freecell_solver_user_alloc();

            // TODO : add an option to customise the limit of the
            // iterations count.

            if (ret_obj === 0) {
                alert("Could not allocate solver instance " +
                    "(out of memory?)");
                throw "Foo";
            }

            if (that._initialize_obj( ret_obj ) !== 0) {
                alert("Failed to initialize solver (Bug!)");
                freecell_solver_user_free(ret_obj);
                throw "Bar";
            }

            return ret_obj;
        })();
        that._proto_states_and_moves_seq = null;
        that._pre_expand_states_and_moves_seq = null;
        that._post_expand_states_and_moves_seq = null;
        that._state_string_buffer = alloc_wrap(
            500, "state string buffer", "Zam");
        that._move_string_buffer = alloc_wrap(
            200, "move string buffer", "Plum");

        return;
    }
    public set_status(myclass, mylabel) {
        const that = this;

        return that.set_status_callback(myclass, mylabel);
    }
    public handle_err_code(solve_err_code) {
        const that = this;
        if (solve_err_code === FCS_STATE_INVALID_STATE) {
            const error_string_ptr = alloc_wrap(
                300, "state error string", "Gum");

            freecell_solver_user_get_invalid_state_error_into_string(
                that.obj, error_string_ptr, 1,
            );

            const error_string = fc_solve_Pointer_stringify(
                error_string_ptr);
            c_free(error_string_ptr );

            alert(error_string + "\n");

            throw "Foo";
        } else if (solve_err_code === FCS_STATE_SUSPEND_PROCESS) {
            if (that.current_iters_limit >= upper_iters_limit) {
                that.set_status("exceeded",
                    "Iterations count exceeded at " +
                    that.current_iters_limit,
                );
                return;
            } else {
                // 50 milliseconds.
                that.set_status("running",
                    "Running (" + that.current_iters_limit +
                    " iterations)");
                return;
            }
        } else if (solve_err_code === FCS_STATE_WAS_SOLVED) {
            that.set_status("solved", "Solved");

            return;
        } else if (solve_err_code === FCS_STATE_IS_NOT_SOLVEABLE) {
            that.set_status("impossible", "Could not solve.");

            return;
        } else {
            alert("Unknown Error code " + solve_err_code + "!");
            throw "Foo";
        }
    }
    public resume_solution() {
        const that = this;

        that._increase_iters_limit();
        const solve_err_code = freecell_solver_user_resume_solution(
            that.obj,
        );
        that.handle_err_code(solve_err_code);
        return solve_err_code;
    }
    public do_solve(proto_board_string) {
        const that = this;

        that.set_status("running", "Running");

        try {
            that._increase_iters_limit();
            // Removed; for debugging purposes.
            // alert("preset_ret = " + preset_ret);

            const board_string = that._process_board_string(
                proto_board_string);
            const solve_err_code = freecell_solver_user_solve_board(
                that.obj, board_string,
            );
            that.handle_err_code(solve_err_code);
            return solve_err_code;
        } catch (e) {
            that.set_status("error", "Error");
            return;
        }
    }
    public unicode_preprocess(out_buffer) {
        const that = this;

        if (! that.is_unicode_cards) {
            return out_buffer;
        }

        if (that.is_unicode_cards_chars) {
            return that._replace_found(that._replace_char_card(out_buffer));
        } else {
            return that._replace_found(that._replace_card(out_buffer));
        }
    }
    public display_solution(args) {
        const that = this;

        let ret;

        try {
            that._calc_states_and_moves_seq();
            that.set_status("solved", "Solved");
            ret = that._display_specific_sol(
                that._pre_expand_states_and_moves_seq,
            );
        } catch (e) {
            return;
        }

        return ret;
    }
    public display_expanded_moves_solution(args) {
        const that = this;

        that._calc_expanded_seq();
        that.set_status("solved", "Solved");
        return that._display_specific_sol(
            that._post_expand_states_and_moves_seq,
        );
    }
    public generic_display_sol(args) {
        const that = this;

        return args.expand ? that.display_expanded_moves_solution(args)
            : that.display_solution(args);
    }
    public get_num_freecells() {
        const that = this;

        return freecell_solver_user_get_num_freecells(that.obj);
    }
    public get_num_stacks() {
        const that = this;

        return freecell_solver_user_get_num_stacks(that.obj);
    }
    private _calc_states_and_moves_seq() {
        const that = this;

        if (that._pre_expand_states_and_moves_seq) {
            return;
        }

        // A sequence to hold the moves and states for post-processing,
        // such as expanding multi-card moves.
        const states_and_moves_sequence = [];

        function _out_state(s) {
            states_and_moves_sequence.push({type: 's', str: s});
        }

        function get_state_str() {
            freecell_solver_user_current_state_stringify(
                that.obj, that._state_string_buffer, 1, 0, 1,
            );

            return fc_solve_Pointer_stringify(that._state_string_buffer);
        }

        _out_state(get_state_str());

        let move_ret_code;
        // 128 bytes are enough to hold a move.
        const move_buffer = alloc_wrap(
            128, "a buffer for the move", "maven");
        while ((move_ret_code = freecell_solver_user_get_next_move(
            that.obj, move_buffer)) === 0) {
            const state_as_string = get_state_str();
            freecell_solver_user_stringify_move_ptr(
                that.obj, that._move_string_buffer, move_buffer, 0);
            const move_as_string = fc_solve_Pointer_stringify(
                that._move_string_buffer);

            states_and_moves_sequence.push({
                exp: null,
                is_exp: false,
                m: {
                    str: move_as_string,
                    type: 'm',
                },
                type: 'm',
            });
            _out_state(state_as_string);
        }

        that._proto_states_and_moves_seq = states_and_moves_sequence;
        that._pre_expand_states_and_moves_seq =
            states_and_moves_sequence.map(
                (item) => {
                    return (item.type === 'm' ? item.m : item);
                },
            );
        that._post_expand_states_and_moves_seq = null;

        // Cleanup C resources
        c_free(move_buffer);
        freecell_solver_user_free(that.obj);
        that.obj = 0;
        c_free(that._state_string_buffer);
        that._state_string_buffer = 0;
        c_free(that._move_string_buffer);
        that._move_string_buffer = 0;

        return;
    }
    private _calc_expanded_move(idx) {
        const that = this;

        const states_and_moves_sequence = that._proto_states_and_moves_seq;

        if (! states_and_moves_sequence[idx].exp) {
            states_and_moves_sequence[idx].exp =
                fc_solve_expand_move(
                    8,
                    4,
                    states_and_moves_sequence[idx - 1].str,
                    states_and_moves_sequence[idx].m,
                    states_and_moves_sequence[idx + 1].str,
                );
        }
        return states_and_moves_sequence[idx].exp;
    }
    private _calc_expanded_seq() {
        const that = this;

        if (that._post_expand_states_and_moves_seq) {
            return;
        }

        that._calc_states_and_moves_seq();

        const states_and_moves_sequence = that._proto_states_and_moves_seq;
        const new_array = [states_and_moves_sequence[0]];
        for (let i = 1; i < states_and_moves_sequence.length - 1; i += 2) {
            Array.prototype.push.apply(
                new_array, that._calc_expanded_move(i),
            );
            new_array.push(states_and_moves_sequence[i + 1]);
        }

        that._post_expand_states_and_moves_seq = new_array;

        return;
    }
    private _display_specific_sol(seq) {
        const that = this;

        let out_buffer = '';

        function my_append(str) {
            out_buffer += str;
        }

        my_append("-=-=-=-=-=-=-=-=-=-=-=-\n\n");

        seq.forEach((x) => {
            const t_ = x.type;
            const str = x.str;
            my_append(str +
                (t_ === 's' ? "\n\n====================\n\n" : "\n\n"),
            );
        });

        return that.unicode_preprocess(
            out_buffer.replace(remove_trailing_space_re, ''),
        );
    }
    private _increase_iters_limit() {
        const that = this;

        that.current_iters_limit += iters_step;
        freecell_solver_user_limit_iterations_long(
            that.obj, that.current_iters_limit);

        return;
    }
    // Ascertain that the string contains a trailing newline.
    // Without the trailing newline, the parser is sometimes confused.
    private _process_board_string(proto_board_string) {
        const that = this;

        if (proto_board_string.match(/\n$/)) {
            return proto_board_string + "";
        } else {
            return proto_board_string + "\n";
        }
    }
    private _stringify_possibly_null_ptr(s_ptr) {
        const that = this;
        return s_ptr ? fc_solve_Pointer_stringify(s_ptr) : '';
    }
    private _initialize_obj(obj) {
        const that = this;
        const cmd_line_preset = that.cmd_line_preset;
        try {
            if (cmd_line_preset !== "default") {
                const error_string_ptr_buf = alloc_wrap(
                    128, "error string buffer", "Foo");
                const preset_ret =
                    freecell_solver_user_cmd_line_read_cmd_line_preset(
                        obj, cmd_line_preset, 0, error_string_ptr_buf,
                        0, null);

                const error_string_ptr = fc_solve_getValue(
                    error_string_ptr_buf, '*');

                const error_string = that._stringify_possibly_null_ptr(
                    error_string_ptr);

                c_free(error_string_ptr);
                c_free(error_string_ptr_buf);

                if (preset_ret !== 0) {
                    alert("Failed to load command line preset '" +
                        cmd_line_preset + "'. Problem is: «" +
                        error_string + "». Should not happen.");
                    throw "Foo";
                }
            }

            if (that.string_params) {
                const error_string_ptr_buf = alloc_wrap(
                    128, "error string buffer", "Engo");
                // Create a file with the contents of string_params.
                // var base_path = '/' + that.dir_base;
                const base_path = '/';
                const file_basename = 'string-params.fc-solve.txt';
                const string_params_file_path = base_path + file_basename;
                fc_solve_FS_writeFile(string_params_file_path,
                    that.string_params, {},
                );

                const args_buf = alloc_wrap(4 * 2, "args buf", "Seed");
                // TODO : Is there a memory leak here?
                const read_from_file_str_ptr = fc_solve_allocate_i8(
                    fc_solve_intArrayFromString("--read-from-file"),
                );
                const arg_str_ptr = fc_solve_allocate_i8(
                    fc_solve_intArrayFromString(
                        "0," + string_params_file_path));

                fc_solve_setValue(args_buf, read_from_file_str_ptr, '*');
                fc_solve_setValue(args_buf + 4, arg_str_ptr, '*');

                const last_arg_ptr = alloc_wrap(
                    4, "last_arg_ptr", "cherry",
                );

                // Input the file to the solver.
                const args_ret_code =
                    freecell_solver_user_cmd_line_parse_args_with_file_nesting_count(
                        obj,
                        2,
                        args_buf,
                        0,
                        0,
                        0,
                        0,
                        error_string_ptr_buf,
                        last_arg_ptr,
                        -1,
                        0,
                    );

                c_free(last_arg_ptr);
                c_free(args_buf);

                const error_string_ptr = fc_solve_getValue(
                    error_string_ptr_buf, '*');

                const error_string = that._stringify_possibly_null_ptr(
                    error_string_ptr);
                c_free(error_string_ptr);
                c_free(error_string_ptr_buf);

                if (args_ret_code !== 0) {
                    alert("Failed to process user-specified command " +
                        "line arguments. Problem is: «" +
                        error_string + "».");
                    throw "Foo";
                }
            }
            return 0;
        } catch (e) {
            that.set_status("error", "Error");
            return -1;
        }
    }
    private _replace_char_card(s) {
        return s.replace(/\b([A2-9TJQK])([HCDS])\b/g,
            fc_solve_2uni_char_card,
        );
    }
    private _replace_card(s) {
        return s.replace(/\b([A2-9TJQK])([HCDS])\b/g,
            fc_solve_2uni_card,
        );
    }
    private _replace_found(s) {
        return s.replace(/\b([HCDS])(-[0A2-9TJQK])\b/g,
            fc_solve_2uni_found,
        );
    }
}
/*
 * Microsoft C Run-time-Library-compatible Random Number Generator
 * Copyright by Shlomi Fish, 2011.
 * Released under the MIT/Expat License
 * ( http://en.wikipedia.org/wiki/MIT_License ).
 * */
class MSRand {
    private gamenumber: string;
    private rander: number;
    constructor(args) {
        const that = this;
        that.gamenumber = args.gamenumber;
        that.rander = fc_solve__hll_ms_rand__get_singleton();
        fc_solve__hll_ms_rand__init(that.rander, "" + that.gamenumber);
        return;
    }
    public max_rand(mymax) {
        return fc_solve__hll_ms_rand__mod_rand(this.rander, mymax);
    }
    public shuffle(deck) {
        if (deck.length) {
            let i = deck.length;
            while (--i) {
                const j = this.max_rand(i + 1);
                const tmp = deck[i];
                deck[i] = deck[j];
                deck[j] = tmp;
            }
        }
        return deck;
    }
}
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

export function deal_ms_fc_board(seed) {
    const randomizer = new MSRand({gamenumber: seed});
    const num_cols = 8;

    function _perl_range(start, end) {
        const ret = [];

        for (let i = start; i <= end; i++) {
            ret.push(i);
        }

        return ret;
    }

    const columns = _perl_range(0, num_cols - 1).map(() => {
        return [];
    });
    let deck = _perl_range(0, 4 * 13 - 1);

    randomizer.shuffle(deck);

    deck = deck.reverse();

    for (let i = 0; i < 52; i++) {
        columns[i % num_cols].push(deck[i]);
    }

    function render_card(card) {
        const suit = (card % 4);
        const rank = Math.floor(card / 4);

        return "A23456789TJQK".charAt(rank) + "CDHS".charAt(suit);
    }

    function render_column(col) {
        return ": " + col.map(render_card).join(" ") + "\n";
    }

    return columns.map(render_column).join("");
}

export class Freecell_Deal_Finder {
    private obj: number;
    private abs_end: bigInt.BigInteger;
    private CHUNKM: bigInt.BigInteger;
    private start: bigInt.BigInteger;
    private update_cb: any;
    constructor(args) {
        const that = this;
        that.obj = fc_solve_user__find_deal__alloc();
    }

    public fill(str) {
        const that = this;
        fc_solve_user__find_deal__fill(that.obj, str);
        return;
    }

    public release() {
        fc_solve_user__find_deal__free(this.obj);
        return;
    }

    public run(abs_start, abs_end_param, update_cb) {
        const that = this;
        const CHUNK = bigInt(1000000);
        that.CHUNKM = CHUNK.add(bigInt.minusOne);
        const start = bigInt(abs_start);
        const abs_end = bigInt(abs_end_param);
        that.abs_end = abs_end;
        that.start = start;
        that.update_cb = update_cb;

        return;
    }

    public cont() {
        const that = this;
        const abs_end = that.abs_end;
        if (that.start.lesser(abs_end)) {
            that.update_cb({start: that.start});
            let end = that.start.add(that.CHUNKM);
            if (end.gt(abs_end)) {
                end = abs_end;
            }
            const result = fc_solve_user__find_deal__run(
                that.obj,
                that.start.toString(),
                end.toString(),
            );
            if (result !== "-1") {
                return {found: true, result};
            }
            that.start = end.add(bigInt.one);
            return {found: false, cont: true};
        } else {
            return {found: false, cont: false};
        }
    }
}
