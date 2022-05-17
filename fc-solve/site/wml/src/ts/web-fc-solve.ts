import * as validate from "./fcs-validate";
import * as BaseApi from "./web-fcs-api-base";
import { fc_solve_expand_move } from "./web-fc-solve--expand-moves";
import { rank_re, suit_re } from "./french-cards";

export interface ModuleWrapper extends BaseApi.ModuleWrapper {
    alloc_wrap: (size: number, desc: string, error: string) => number;
    c_free: (buffer: number) => number;
    fc_solve_allocate_i8: (pointer: number) => number;
    fc_solve_Pointer_stringify: (buffer: number) => string;
    user_alloc: (...args: any) => any;
    user_cmd_line_parse_args_with_file_nesting_count: (...args: any) => any;
    user_cmd_line_read_cmd_line_preset: (...args: any) => any;
    user_current_state_stringify: (...args: any) => any;
    user_free: (...args: any) => any;
    user_get_invalid_state_error_into_string: (...args: any) => any;
    user_get_next_move: (...args: any) => any;
    user_get_num_freecells: (...args: any) => any;
    user_get_num_stacks: (...args: any) => any;
    user_get_unrecognized_cmd_line_flag: (...args: any) => any;
    user_get_unrecognized_cmd_line_flag_status: (...args: any) => any;
    user_limit_iterations_long: (...args: any) => any;
    user_resume_solution: (...args: any) => any;
    user_solve_board: (...args: any) => any;
    user_stringify_move_ptr: (...args: any) => any;
}

export function FC_Solve_init_wrappers_with_module(Module): ModuleWrapper {
    const module_wrapper = BaseApi.base_calc_module_wrapper(
        Module,
    ) as ModuleWrapper;
    module_wrapper.fc_solve_allocate_i8 = (p1) => {
        return Module.allocate(p1, "i8", Module.ALLOC_STACK);
    };
    module_wrapper.user_alloc = Module.cwrap(
        "freecell_solver_user_alloc",
        "number",
        [],
    );
    module_wrapper.user_solve_board = Module.cwrap(
        "freecell_solver_user_solve_board",
        "number",
        ["number", "string"],
    );
    module_wrapper.user_resume_solution = Module.cwrap(
        "freecell_solver_user_resume_solution",
        "number",
        ["number"],
    );
    module_wrapper.user_cmd_line_read_cmd_line_preset = Module.cwrap(
        "freecell_solver_user_cmd_line_read_cmd_line_preset",
        "number",
        ["number", "string", "number", "number", "number", "string"],
    );
    module_wrapper.user_get_next_move = Module.cwrap(
        "freecell_solver_user_get_next_move",
        "number",
        ["number", "number"],
    );
    module_wrapper.user_get_num_freecells = Module.cwrap(
        "freecell_solver_user_get_num_freecells",
        "number",
        ["number"],
    );
    module_wrapper.user_get_num_stacks = Module.cwrap(
        "freecell_solver_user_get_num_stacks",
        "number",
        ["number"],
    );
    module_wrapper.user_get_unrecognized_cmd_line_flag = Module.cwrap(
        "freecell_solver_user_get_unrecognized_cmd_line_flag",
        "number",
        ["number", "number"],
    );
    module_wrapper.user_get_unrecognized_cmd_line_flag_status = Module.cwrap(
        "freecell_solver_user_get_unrecognized_cmd_line_flag_status",
        "number",
        ["number", "number"],
    );
    module_wrapper.user_current_state_stringify = Module.cwrap(
        "freecell_solver_user_current_state_stringify",
        "number",
        ["number", "number", "number", "number", "number"],
    );
    module_wrapper.user_stringify_move_ptr = Module.cwrap(
        "freecell_solver_user_stringify_move_ptr",
        "number",
        ["number", "number", "number", "number"],
    );
    module_wrapper.user_free = Module.cwrap(
        "freecell_solver_user_free",
        "number",
        ["number"],
    );
    module_wrapper.user_limit_iterations_long = Module.cwrap(
        "freecell_solver_user_limit_iterations_long",
        "number",
        ["number", "number"],
    );
    module_wrapper.user_get_invalid_state_error_into_string = Module.cwrap(
        "freecell_solver_user_get_invalid_state_error_into_string",
        "number",
        ["number", "number", "number"],
    );
    module_wrapper.user_cmd_line_parse_args_with_file_nesting_count =
        Module.cwrap(
            "freecell_solver_user_cmd_line_parse_args_with_file_nesting_count",
            "number",
            [
                "number",
                "number",
                "number",
                "number",
                "number",
                "number",
                "number",
                "number",
                "number",
                "number",
                "number",
            ],
        );
    module_wrapper.alloc_wrap = ((my_malloc) => {
        return (size: number, desc: string, error: string) => {
            const buffer = my_malloc(size);
            if (buffer === 0) {
                alert("Could not allocate " + desc + " (out of memory?)");
                throw error;
            }
            return buffer;
        };
    })(Module.cwrap("malloc", "number", ["number"]));
    module_wrapper.c_free = Module.cwrap("free", "number", ["number"]);
    module_wrapper.fc_solve_Pointer_stringify = (ptr) => {
        return Module.UTF8ToString(ptr, 10000);
    };

    return module_wrapper;
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

const fc_solve_2uni_suit_map = { H: "♥", C: "♣", D: "♦", S: "♠" };
const fc_solve_2uni_suit_map_num = { H: 1, C: 3, D: 2, S: 0 };

function fc_solve_2uni_card(match, p1, p2, offset, mystring) {
    return p1 + fc_solve_2uni_suit_map[p2];
}

function fc_solve_2uni_char_card(match, p1, p2, offset, mystring) {
    const rank = validate.ranks__str_to_int[p1];

    const ret = String.fromCodePoint(
        fc_solve_2uni_suit_map_num[p2] * 16 + 0x1f0a0 + rank,
    );
    return ret;
}

function fc_solve_2uni_found(match, p1, p2, offset, mystring) {
    return fc_solve_2uni_suit_map[p1] + p2;
}

const card_re = new RegExp("\\b(" + rank_re + ")(" + suit_re + ")\\b", "g");
const found_re = new RegExp("\\b(" + suit_re + ")(-[0A2-9TJQK])\\b", "g");
export class DisplayFilter {
    public is_unicode_cards: boolean;
    public is_unicode_cards_chars: boolean;
    constructor(args) {
        const that = this;
        that.is_unicode_cards = args.is_unicode_cards;
        that.is_unicode_cards_chars = args.is_unicode_cards_chars;
        return;
    }
    public unicode_preprocess(out_buffer: string) {
        const display = this;
        if (!display.is_unicode_cards) {
            return out_buffer;
        }

        return display._replace_found(
            display.is_unicode_cards_chars
                ? display._replace_char_card(out_buffer)
                : display._replace_card(out_buffer),
        );
    }
    private _replace_char_card(s) {
        return s.replace(card_re, fc_solve_2uni_char_card);
    }
    private _replace_card(s) {
        return s.replace(card_re, fc_solve_2uni_card);
    }
    private _replace_found(s) {
        return s.replace(found_re, fc_solve_2uni_found);
    }
}

interface DisplaySolutionArgs {
    displayer: DisplayFilter;
}
interface DisplaySolArgs {
    displayer: DisplayFilter;
    expand: boolean;
}

const ptr_type: string = "i32";
export class FC_Solve {
    public proto_states_and_moves_seq: any[];
    private dir_base: string;
    private string_params: string[];
    private set_status_callback: any;
    private cmd_line_preset: string;
    private current_iters_limit: number;
    private module_wrapper: ModuleWrapper;
    private obj: any;
    private _do_not_alert: boolean;
    private _pre_expand_states_and_moves_seq: any;
    private _post_expand_states_and_moves_seq: any;
    private _state_string_buffer: number;
    private _move_string_buffer: number;
    private _unrecognized_opt: string;

    constructor(args) {
        const that = this;
        that.module_wrapper = args.module_wrapper;
        that._do_not_alert = false;

        that.dir_base = args.dir_base;
        that.string_params = args.string_params ? [args.string_params] : null;
        that.set_status_callback = args.set_status_callback;
        that.cmd_line_preset = args.cmd_line_preset;
        that.current_iters_limit = 0;
        that.obj = (() => {
            const ret_obj = that.module_wrapper.user_alloc();

            // TODO : add an option to customise the limit of the
            // iterations count.

            if (ret_obj === 0) {
                alert(
                    "Could not allocate solver instance " + "(out of memory?)",
                );
                throw "Foo";
            }

            if (that._initialize_obj(ret_obj) !== 0) {
                if (that._do_not_alert) {
                    that._do_not_alert = false;
                } else {
                    alert("Failed to initialize solver (Bug!)");
                }
                that.module_wrapper.user_free(ret_obj);
                throw "Bar";
            }

            return ret_obj;
        })();
        that.proto_states_and_moves_seq = null;
        that._pre_expand_states_and_moves_seq = null;
        that._post_expand_states_and_moves_seq = null;
        that._state_string_buffer = that.module_wrapper.alloc_wrap(
            500,
            "state string buffer",
            "Zam",
        );
        that._move_string_buffer = that.module_wrapper.alloc_wrap(
            200,
            "move string buffer",
            "Plum",
        );

        return;
    }
    public set_status(myclass, mylabel) {
        const that = this;

        return that.set_status_callback(myclass, mylabel);
    }
    public handle_err_code(solve_err_code) {
        const that = this;
        if (solve_err_code === FCS_STATE_INVALID_STATE) {
            const error_string_ptr = that.module_wrapper.alloc_wrap(
                300,
                "state error string",
                "Gum",
            );

            that.module_wrapper.user_get_invalid_state_error_into_string(
                that.obj,
                error_string_ptr,
                1,
            );

            const error_string =
                that.module_wrapper.fc_solve_Pointer_stringify(
                    error_string_ptr,
                );
            that.module_wrapper.c_free(error_string_ptr);

            alert(error_string + "\n");

            throw "Foo";
        } else if (solve_err_code === FCS_STATE_SUSPEND_PROCESS) {
            if (that.current_iters_limit >= upper_iters_limit) {
                that.set_status(
                    "exceeded",
                    "Iterations count exceeded at " + that.current_iters_limit,
                );
                return;
            } else {
                // 50 milliseconds.
                that.set_status(
                    "running",
                    "Running (" + that.current_iters_limit + " iterations)",
                );
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
        const solve_err_code = that.module_wrapper.user_resume_solution(
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

            const board_string = that._process_board_string(proto_board_string);
            const solve_err_code = that.module_wrapper.user_solve_board(
                that.obj,
                board_string,
            );
            that.handle_err_code(solve_err_code);
            return solve_err_code;
        } catch (e) {
            that.set_status("error", "Error");
            return;
        }
    }
    public unicode_preprocess(out_buffer, display: DisplayFilter) {
        const that = this;

        return display.unicode_preprocess(out_buffer);
    }
    public display_solution(args: DisplaySolutionArgs) {
        const that = this;

        const displayer: DisplayFilter = args.displayer;
        let ret;

        try {
            that._calc_states_and_moves_seq();
            that.set_status("solved", "Solved");
            ret = that._display_specific_sol(
                that._pre_expand_states_and_moves_seq,
                displayer,
            );
        } catch (e) {
            return;
        }

        return ret;
    }
    public display_expanded_moves_solution(args: DisplaySolutionArgs) {
        const that = this;

        that._calc_expanded_seq();
        that.set_status("solved", "Solved");
        return that._display_specific_sol(
            that._post_expand_states_and_moves_seq,
            args.displayer,
        );
    }
    public calc_expanded_move(idx) {
        const that = this;

        const states_and_moves_sequence = that.proto_states_and_moves_seq;

        if (!states_and_moves_sequence[idx].exp) {
            states_and_moves_sequence[idx].exp = fc_solve_expand_move(
                8,
                4,
                states_and_moves_sequence[idx - 1].str,
                states_and_moves_sequence[idx].m,
                states_and_moves_sequence[idx + 1].str,
            );
        }
        return states_and_moves_sequence[idx].exp;
    }
    public generic_display_sol(args: DisplaySolArgs) {
        const that = this;

        return args.expand
            ? that.display_expanded_moves_solution(args)
            : that.display_solution(args);
    }
    public get_num_freecells() {
        const that = this;

        return that.module_wrapper.user_get_num_freecells(that.obj);
    }
    public get_num_stacks() {
        const that = this;

        return that.module_wrapper.user_get_num_stacks(that.obj);
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
            states_and_moves_sequence.push({ type: "s", str: s });
        }

        function get_state_str() {
            that.module_wrapper.user_current_state_stringify(
                that.obj,
                that._state_string_buffer,
                1,
                0,
                1,
            );

            return that.module_wrapper.fc_solve_Pointer_stringify(
                that._state_string_buffer,
            );
        }

        _out_state(get_state_str());

        let move_ret_code;
        // 128 bytes are enough to hold a move.
        const move_buffer = that.module_wrapper.alloc_wrap(
            128,
            "a buffer for the move",
            "maven",
        );
        while (
            (move_ret_code = that.module_wrapper.user_get_next_move(
                that.obj,
                move_buffer,
            )) === 0
        ) {
            const state_as_string = get_state_str();
            that.module_wrapper.user_stringify_move_ptr(
                that.obj,
                that._move_string_buffer,
                move_buffer,
                0,
            );
            const move_as_string =
                that.module_wrapper.fc_solve_Pointer_stringify(
                    that._move_string_buffer,
                );

            states_and_moves_sequence.push({
                exp: null,
                is_exp: false,
                m: {
                    str: move_as_string,
                    type: "m",
                },
                type: "m",
            });
            _out_state(state_as_string);
        }

        that.proto_states_and_moves_seq = states_and_moves_sequence;
        that._pre_expand_states_and_moves_seq = states_and_moves_sequence.map(
            (item) => {
                return item.type === "m" ? item.m : item;
            },
        );
        that._post_expand_states_and_moves_seq = null;

        // Cleanup C resources
        that.module_wrapper.c_free(move_buffer);
        that.module_wrapper.user_free(that.obj);
        that.obj = 0;
        that.module_wrapper.c_free(that._state_string_buffer);
        that._state_string_buffer = 0;
        that.module_wrapper.c_free(that._move_string_buffer);
        that._move_string_buffer = 0;

        return;
    }
    private _calc_expanded_seq() {
        const that = this;

        if (that._post_expand_states_and_moves_seq) {
            return;
        }

        that._calc_states_and_moves_seq();

        const states_and_moves_sequence = that.proto_states_and_moves_seq;
        const new_array = [states_and_moves_sequence[0]];
        for (let i = 1; i < states_and_moves_sequence.length - 1; i += 2) {
            Array.prototype.push.apply(new_array, that.calc_expanded_move(i));
            new_array.push(states_and_moves_sequence[i + 1]);
        }

        that._post_expand_states_and_moves_seq = new_array;

        return;
    }
    private _display_specific_sol(seq, displayer: DisplayFilter) {
        const that = this;

        let out_buffer = "";

        function my_append(str) {
            out_buffer += str;
        }

        my_append("-=-=-=-=-=-=-=-=-=-=-=-\n\n");

        for (const x of seq) {
            const t_ = x.type;
            const str = x.str;
            my_append(
                str + (t_ === "s" ? "\n\n====================\n\n" : "\n\n"),
            );
        }

        return that.unicode_preprocess(
            out_buffer.replace(remove_trailing_space_re, ""),
            displayer,
        );
    }
    private _increase_iters_limit() {
        const that = this;

        that.current_iters_limit += iters_step;
        that.module_wrapper.user_limit_iterations_long(
            that.obj,
            that.current_iters_limit,
        );

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
        return s_ptr
            ? that.module_wrapper.fc_solve_Pointer_stringify(s_ptr)
            : "";
    }
    private _initialize_obj(obj) {
        const that = this;
        const cmd_line_preset = that.cmd_line_preset;
        try {
            if (cmd_line_preset !== "default") {
                const error_string_ptr_buf = that.module_wrapper.alloc_wrap(
                    128,
                    "error string buffer",
                    "Foo",
                );
                const preset_ret =
                    that.module_wrapper.user_cmd_line_read_cmd_line_preset(
                        obj,
                        cmd_line_preset,
                        0,
                        error_string_ptr_buf,
                        0,
                        null,
                    );

                const error_string_ptr = that.module_wrapper.Module.getValue(
                    error_string_ptr_buf,
                    ptr_type,
                );

                const error_string =
                    that._stringify_possibly_null_ptr(error_string_ptr);

                that.module_wrapper.c_free(error_string_ptr);
                that.module_wrapper.c_free(error_string_ptr_buf);

                if (preset_ret !== 0) {
                    alert(
                        "Failed to load command line preset '" +
                            cmd_line_preset +
                            "'. Problem is: «" +
                            error_string +
                            "». Should not happen.",
                    );
                    throw "Foo";
                }
            }

            if (that.string_params) {
                const error_string_ptr_buf = that.module_wrapper.alloc_wrap(
                    128,
                    "error string buffer",
                    "Engo",
                );
                // Create a file with the contents of string_params.
                // var base_path = '/' + that.dir_base;
                const base_path = "/";
                const file_basename = "string-params.fc-solve.txt";
                const string_params_file_path = base_path + file_basename;
                that.module_wrapper.Module.FS.writeFile(
                    string_params_file_path,
                    that.string_params[0],
                    {},
                );

                const args_buf = that.module_wrapper.alloc_wrap(
                    4 * 2,
                    "args buf",
                    "Seed",
                );
                // TODO : Is there a memory leak here?
                const read_from_file_str_ptr =
                    that.module_wrapper.fc_solve_allocate_i8(
                        that.module_wrapper.Module.intArrayFromString(
                            "--read-from-file",
                        ),
                    );
                const arg_str_ptr = that.module_wrapper.fc_solve_allocate_i8(
                    that.module_wrapper.Module.intArrayFromString(
                        "0," + string_params_file_path,
                    ),
                );

                that.module_wrapper.Module.setValue(
                    args_buf,
                    read_from_file_str_ptr,
                    ptr_type,
                );
                that.module_wrapper.Module.setValue(
                    args_buf + 4,
                    arg_str_ptr,
                    ptr_type,
                );

                const last_arg_ptr = that.module_wrapper.alloc_wrap(
                    4,
                    "last_arg_ptr",
                    "cherry",
                );

                // Input the file to the solver.
                const args_ret_code =
                    that.module_wrapper.user_cmd_line_parse_args_with_file_nesting_count(
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

                that.module_wrapper.c_free(last_arg_ptr);
                that.module_wrapper.c_free(args_buf);

                const error_string_ptr = that.module_wrapper.Module.getValue(
                    error_string_ptr_buf,
                    ptr_type,
                );

                const error_string =
                    that._stringify_possibly_null_ptr(error_string_ptr);
                that.module_wrapper.c_free(error_string_ptr);
                that.module_wrapper.c_free(error_string_ptr_buf);

                if (args_ret_code !== 0) {
                    const unrecognized_opt_ptr =
                        that.module_wrapper.user_get_unrecognized_cmd_line_flag_status(
                            obj,
                            0,
                        ) == 0
                            ? that.module_wrapper.user_get_unrecognized_cmd_line_flag(
                                  obj,
                                  0,
                              )
                            : 0;
                    let unrecognized_opt_s = "";
                    if (unrecognized_opt_ptr != 0) {
                        that._do_not_alert = true;
                        that._unrecognized_opt =
                            that._stringify_possibly_null_ptr(
                                unrecognized_opt_ptr,
                            );
                        that.module_wrapper.c_free(unrecognized_opt_ptr);
                        let exception_string = "";
                        if (
                            validate.determine_if_string_is_board_like(
                                that.string_params[0],
                            )
                        ) {
                            unrecognized_opt_s =
                                "Did you try inputting the cards' deal in the command-line arguments text box?\n" +
                                "Unrecognized command line flag: «" +
                                that._unrecognized_opt +
                                "».";
                            exception_string =
                                "CommandLineArgsMayContainCardsArrangement";
                        } else {
                            unrecognized_opt_s =
                                "The Command Line arguments' textbox should " +
                                "normally be kept " +
                                "empty. (It is intended for advanced use!) " +
                                "There was an unrecognized command line flag: «" +
                                that._unrecognized_opt +
                                "».";
                            exception_string = "Bar";
                        }
                        alert(unrecognized_opt_s);
                        throw exception_string;
                    }
                    alert(
                        "Failed to process user-specified command " +
                            "line arguments. Problem is: «" +
                            error_string +
                            "».",
                    );
                    throw "Foo";
                }
            }
            return 0;
        } catch (e) {
            that.set_status("error", "Error");
            return -1;
        }
    }
}
