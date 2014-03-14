"use strict";

var freecell_solver_user_alloc = Module.cwrap('freecell_solver_user_alloc', 'number', []);
var freecell_solver_user_solve_board = Module.cwrap('freecell_solver_user_solve_board', 'number', ['number', 'string']);
var freecell_solver_user_resume_solution = Module.cwrap('freecell_solver_user_resume_solution', 'number', ['number']);
var freecell_solver_user_cmd_line_read_cmd_line_preset = Module.cwrap('freecell_solver_user_cmd_line_read_cmd_line_preset', 'number', ['number', 'string', 'number', 'number', 'number', 'string']);
var malloc = Module.cwrap('malloc', 'number', ['number']);
var c_free = Module.cwrap('free', 'number', ['number']);
var freecell_solver_user_get_next_move = Module.cwrap('freecell_solver_user_get_next_move', 'number', ['number', 'number']);
var freecell_solver_user_current_state_as_string = Module.cwrap('freecell_solver_user_current_state_as_string', 'number', ['number', 'number', 'number', 'number']);
var freecell_solver_user_move_ptr_to_string_w_state = Module.cwrap('freecell_solver_user_move_ptr_to_string_w_state', 'number', ['number', 'number', 'number']);
var freecell_solver_user_free = Module.cwrap('freecell_solver_user_free', 'number', ['number']);
var freecell_solver_user_limit_iterations = Module.cwrap('freecell_solver_user_limit_iterations', 'number', ['number', 'number']);
var freecell_solver_user_get_invalid_state_error_string = Module.cwrap('freecell_solver_user_get_invalid_state_error_string', 'number', ['number', 'number']);
var freecell_solver_user_cmd_line_parse_args = Module.cwrap('freecell_solver_user_cmd_line_parse_args', 'number', ['number', 'number', 'number', 'number', 'number', 'number', 'number', 'number', 'number']);

var remove_trailing_space_re = /[ \t]+$/gm;

var    FCS_STATE_WAS_SOLVED = 0;
var    FCS_STATE_IS_NOT_SOLVEABLE = 1;
var    FCS_STATE_ALREADY_EXISTS = 2;
var    FCS_STATE_EXCEEDS_MAX_NUM_TIMES = 3;
var    FCS_STATE_BEGIN_SUSPEND_PROCESS = 4;
var    FCS_STATE_SUSPEND_PROCESS = 5;
var    FCS_STATE_EXCEEDS_MAX_DEPTH = 6;
var    FCS_STATE_ORIGINAL_STATE_IS_NOT_SOLVEABLE = 7;
var    FCS_STATE_INVALID_STATE = 8;
var    FCS_STATE_NOT_BEGAN_YET = 9;
var    FCS_STATE_DOES_NOT_EXIST = 10;
var    FCS_STATE_OPTIMIZED = 11;
var    FCS_STATE_FLARES_PLAN_ERROR = 12;

var iters_step = 1000;
var upper_iters_limit = 128 * 1024;

var fc_solve_2uni_suit_map = { 'H': '♥', 'C': '♣', 'D': '♦', 'S': '♠' };

function fc_solve_2uni_card(match, p1, p2, offset, mystring) {
    return p1 + fc_solve_2uni_suit_map[p2];
}

function fc_solve_2uni_found(match, p1, p2, offset, mystring) {
    return fc_solve_2uni_suit_map[p1] + p2;
}

Class('FC_Solve', {
    has: {
        dir_base: { is: ro },
        string_params: { is: ro, init: null, },
        set_status_callback: { is: ro },
        is_unicode_cards: { is: ro, init: false, },
        cmd_line_preset: { is: ro },
        current_iters_limit: { is: rw, init: 0 },
        set_output: { is: ro },
        obj: {
            is: rw,
            init: function() {
                var ret_obj = freecell_solver_user_alloc();

                // TODO : add an option to customise the limit of the iterations count.

                if (ret_obj == 0) {
                    alert ("Could not allocate solver instance (out of memory?)");
                    throw "Foo";
                }

                return ret_obj;
            },
        },
    },
    methods: {
        set_status: function (myclass, mylabel) {
            var that = this;

            return that.set_status_callback(myclass, mylabel);
        },
        handle_err_code: function(solve_err_code) {
             var that = this;
             if (solve_err_code == FCS_STATE_INVALID_STATE) {
                 var error_string_ptr = freecell_solver_user_get_invalid_state_error_string(
                     that.obj, 1
                 );

                 var error_string = that._stringify_possibly_null_ptr(error_string_ptr);
                 c_free ( error_string_ptr );

                 alert (error_string + "\n");

                 throw "Foo";
             }
             else if (solve_err_code == FCS_STATE_SUSPEND_PROCESS) {
                 if (that.current_iters_limit >= upper_iters_limit) {
                     that.set_status("exceeded",
                         "Iterations count exceeded at " + that.current_iters_limit
                         );
                     return;
                 } else {
                     // 50 milliseconds.
                     that.set_status("running", "Running (" + that.current_iters_limit + " iterations)");
                     return;
                 }
             }
             else if (solve_err_code == FCS_STATE_WAS_SOLVED) {

                 that.set_status("solved", "Solved");

                 that.display_solution();

                 return;
             }
             else if (solve_err_code == FCS_STATE_IS_NOT_SOLVEABLE) {

                 that.set_status("impossible", "Could not solve.");

                 return;
             }
             else {
                 alert ("Unknown Error code " + solve_err_code + "!");
                 throw "Foo";
             }
        },
        resume_solution: function() {
            var that = this;

            that.current_iters_limit += iters_step;

            freecell_solver_user_limit_iterations(that.obj, that.current_iters_limit);

            var solve_err_code = freecell_solver_user_resume_solution( that.obj );

            that.handle_err_code(solve_err_code);

            return solve_err_code;
        },
        // Ascertain that the string contains a trailing newline.
        // Without the trailing newline, the parser is sometimes confused.
        _process_board_string: function (proto_board_string) {
            var that = this;

            if (proto_board_string.match(/\n$/)) {
                return proto_board_string + "";
            }
            else {
                return proto_board_string + "\n";
            }
        },
        _stringify_possibly_null_ptr: function (s_ptr) {
            var that = this;

            var ret = '';

            if (s_ptr) {
                ret = Module.Pointer_stringify(s_ptr);
            }

            return ret;
        },
        do_solve: function (proto_board_string) {
            var that = this;

            var board_string = that._process_board_string(proto_board_string);
            var cmd_line_preset = that.cmd_line_preset;

            that.set_status("running", "Running");

            try {
                if (cmd_line_preset != "default") {

                    var error_string_ptr_buf = malloc(128);

                    if (error_string_ptr_buf == 0) {
                        alert ("Failed to allocate (out of memory?).");
                        throw "Foo";
                    }

                    var preset_ret = freecell_solver_user_cmd_line_read_cmd_line_preset(that.obj, cmd_line_preset, 0, error_string_ptr_buf, 0, null);

                    var error_string_ptr = getValue(error_string_ptr_buf, '*');

                    var error_string = that._stringify_possibly_null_ptr(error_string_ptr);

                    c_free(error_string_ptr);
                    c_free(error_string_ptr_buf);

                    if (preset_ret != 0) {
                        alert ("Failed to load command line preset '" + cmd_line_preset + "'. Problem is: «" + error_string + "». Should not happen.");
                        throw "Foo";
                    }
                }

                if (that.string_params) {

                    var error_string_ptr_buf = malloc(128);

                    if (error_string_ptr_buf == 0) {
                        alert ("Failed to allocate (out of memory?).");
                        throw "Foo";
                    }

                    // Create a file with the contents of string_params.
                    var base_path = '/' + that.dir_base;
                    var base_dh = FS.createFolder('/', that.dir_base, true, true);
                    var file_basename = 'string-params.fc-solve.txt';
                    var string_params_file_path = base_path + '/' + file_basename;
                    FS.createDataFile(base_dh, file_basename, that.string_params,
                        true, true);


                    var args_buf = malloc(4*2);
                    if (args_buf == 0) {
                        alert ("Failed to allocate (out of memory?).");
                        throw "Foo";
                    }

                    // TODO : Is there a memory leak here?
                    var read_from_file_str_ptr = allocate(intArrayFromString("--read-from-file"), 'i8', ALLOC_STACK);
                    var arg_str_ptr = allocate(intArrayFromString("0," + string_params_file_path), 'i8', ALLOC_STACK);

                    setValue(args_buf, read_from_file_str_ptr, '*');
                    setValue(args_buf+4, arg_str_ptr, '*');

                    // Input the file to the solver.
                    var args_ret_code = freecell_solver_user_cmd_line_parse_args(
                        that.obj,
                        2,
                        args_buf,
                        0,
                        0,
                        0,
                        0,
                        error_string_ptr_buf,
                        0
                    );

                    c_free(args_buf);

                    var error_string_ptr = getValue(error_string_ptr_buf, '*');

                    var error_string = that._stringify_possibly_null_ptr(error_string_ptr);
                    c_free(error_string_ptr);
                    c_free(error_string_ptr_buf);

                    if (args_ret_code != 0) {
                        alert ("Failed to process user-specified command line arguments. Problem is: «" + error_string + "».");
                        throw "Foo";
                    }
                }

                that.current_iters_limit += iters_step;

                freecell_solver_user_limit_iterations(that.obj, that.current_iters_limit);

                // Removed for debugging purposes.
                // alert("preset_ret = " + preset_ret);

                var solve_err_code = freecell_solver_user_solve_board(
                    that.obj, board_string
                );

                that.handle_err_code(solve_err_code);

                return solve_err_code;
            }
            catch (e) {
                that.set_status("error", "Error");
                return;
            }
        },
        _replace_card: function(s) {
            return s.replace(/\b([A2-9TJQK])([HCDS])\b/g,
                fc_solve_2uni_card
            );
        },
        _replace_found: function (s) {
            return s.replace(/\b([HCDS])(-[0A2-9TJQK])\b/g,
                fc_solve_2uni_found
            );
        },
        unicode_preprocess: function(out_buffer) {
            var that = this;

            if (! that.is_unicode_cards) {
                return out_buffer;
            }

            return that._replace_found(that._replace_card(out_buffer));
        },
        display_solution: function() {
            var that = this;

            try {
                // 128 bytes are enough to hold a move.
                var move_buffer = malloc(128);

                if (move_buffer == 0) {
                    alert ("Failed to allocate a buffer for the move (out of memory?)");
                    throw "Foo";
                }

                var get_state_str = function () {
                    var ptr = freecell_solver_user_current_state_as_string(that.obj, 1, 0, 1);

                    if (ptr == 0) {
                        alert ("Failed to retrieve the current state (out of memory?)");
                        throw "Foo";
                    }
                    var ret_string = Module.Pointer_stringify(ptr);
                    c_free(ptr);
                    return ret_string;
                };

                var out_buffer = '';

                var my_append = function (str) {
                    out_buffer = out_buffer + str;
                };

                my_append("-=-=-=-=-=-=-=-=-=-=-=-\n\n");

                var _out_state = function (state_as_string) {
                    my_append ( state_as_string +
                        "\n\n====================\n\n"
                    );
                };

                _out_state (get_state_str() );


                var move_ret_code;
                while ((move_ret_code = freecell_solver_user_get_next_move(that.obj, move_buffer)) == 0) {
                    var state_as_string = get_state_str();
                    var move_as_string_ptr = freecell_solver_user_move_ptr_to_string_w_state(that.obj, move_buffer, 0);

                    if (move_as_string_ptr == 0) {
                        alert ("Failed to retrieve the current move as string (out of memory?)");
                        throw "Foo";
                    }

                    var move_as_string = Module.Pointer_stringify(move_as_string_ptr);
                    c_free (move_as_string_ptr);

                    my_append(move_as_string + "\n\n");
                    _out_state(state_as_string);
                }

                // Cleanup C resources
                c_free(move_buffer);
                freecell_solver_user_free(that.obj);
                that.obj = 0;

                that.set_status("solved", "Solved");

                that.set_output(
                    that.unicode_preprocess(
                        out_buffer.replace(remove_trailing_space_re, '')
                    )
                );
                return;
            }
            catch (e) {
                return;
            }
        },
    },
});

