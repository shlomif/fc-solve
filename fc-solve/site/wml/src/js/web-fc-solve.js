"use strict";
var entityMap = {
    "&": "&amp;",
    "<": "&lt;",
    ">": "&gt;",
    '"': '&quot;',
    "'": '&#39;',
    "/": '&#x2F;'
};

function escapeHtml(string) {
    return String(string).replace(/[&<>"'\/]/g, function (s) {
        return entityMap[s];
    });
}

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

var remove_trailing_space_re = /[ \t]+$/gm;

function _webui_set_output(buffer) {
    $("#output").val(buffer);

    return;
}

function clear_output() {
    return _webui_set_output('');
}

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

var iters_step = 128;
var upper_iters_limit = 128 * 1024;


Class('FC_Solve', {
    has: {
        set_status_callback: { is: ro },
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
                 alert ("Failed to solve board (invalid layout?)");
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
                     setTimeout(
                         function () { return that.resume_solution() },
                         50
                     );
                     return;
                 }
             }
             else if (solve_err_code == FCS_STATE_WAS_SOLVED) {

                 that.set_status("solved", "Solved");

                 that.display_solution();

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

            return that.handle_err_code(solve_err_code);
        },
        do_solve: function (board_string) {
            var that = this;

            var cmd_line_preset = that.cmd_line_preset;

            that.set_status("running", "Running");

            try {
                if (cmd_line_preset != "default") {
                    var preset_ret = freecell_solver_user_cmd_line_read_cmd_line_preset(that.obj, cmd_line_preset, 0, 0, 0, null);

                    if (preset_ret != 0) {
                        alert ("Failed to load command line preset '" + cmd_line_preset + "'. Should not happen.");
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

                return that.handle_err_code(solve_err_code);
            }
            catch (e) {
                that.set_status("error", "Error");
                return;
            }
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
                    var ret_string = Pointer_stringify(ptr);
                    c_free(ptr);
                    return ret_string;
                };

                var out_buffer = '';

                var my_append = function (str) {
                    out_buffer = out_buffer + str;
                };

                (function () {
                    var state_as_string = get_state_str();
                    my_append ( state_as_string + "\n\n");
                })();

                var move_ret_code;
                while ((move_ret_code = freecell_solver_user_get_next_move(that.obj, move_buffer)) == 0) {
                    var state_as_string = get_state_str();
                    var move_as_string_ptr = freecell_solver_user_move_ptr_to_string_w_state(that.obj, move_buffer, 0);

                    if (move_as_string_ptr == 0) {
                        alert ("Failed to retrieve the current move as string (out of memory?)");
                        throw "Foo";
                    }

                    var move_as_string = Pointer_stringify(move_as_string_ptr);
                    c_free (move_as_string_ptr);

                    my_append(move_as_string + "\n\n" + state_as_string + "\n\n");
                }

                // Cleanup C resources
                c_free(move_buffer);
                freecell_solver_user_free(that.obj);
                that.obj = 0;

                that.set_status("solved", "Solved");

                that.set_output(
                    out_buffer.replace(remove_trailing_space_re, '')
                );
                return;
            }
            catch (e) {
                return;
            }
        },
    },
});

function _webui_set_status_callback(myclass, mylabel)
{
    var ctl = $("#fc_solve_status");
    ctl.removeClass();
    ctl.addClass(myclass);
    ctl.html(escapeHtml(mylabel));

    return;
}

function fc_solve_do_solve() {
    var cmd_line_preset = $("#preset").val();
    var board_string = $("#stdin").val().replace(/#[^\r\n]*\r?\n?/g, '');

    var instance = new FC_Solve({
        cmd_line_preset: cmd_line_preset,
        set_status_callback: _webui_set_status_callback,
        set_output: _webui_set_output,
    });

    return instance.do_solve(board_string);
}

// Thanks to Stefan Petrea ( http://garage-coding.com/ ) for inspiring this
// feature.
var previous_deal_idx = 1;

function populate_input_with_numbered_deal() {

    /*
    var new_idx = prompt("Enter MS Freecell deal number:");

    // Prompt returned null (user cancelled).
    if (! new_idx) {
        return;
    }

    previous_deal_idx = parseInt(new_idx);
    */

    previous_deal_idx = parseInt($('#deal_number').val());

    $("#stdin").val(
        "# MS Freecell Deal #" + previous_deal_idx +
        "\n#\n" +
        deal_ms_fc_board(previous_deal_idx)
    );

    return;
}
