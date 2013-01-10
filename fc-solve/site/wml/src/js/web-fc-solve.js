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

function set_status(myclass, mylabel) {
    var ctl = $("#fc_solve_status");
    ctl.removeClass();
    ctl.addClass(myclass);
    ctl.html(escapeHtml(mylabel));

    return;
}

function clear_output() {
    $("#output").val('');

    return;
}

var iters_step = 128;
var upper_iters_limit = 128 * 1024;
var current_iters_limit;

function reset_iterations()
{
    current_iters_limit = 0;
}

reset_iterations();

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

var obj;

function handle_err_code(solve_err_code) {
    if (solve_err_code == FCS_STATE_INVALID_STATE) {
        alert ("Failed to solve board (invalid layout?)");
        throw "Foo";
    }
    else if (solve_err_code == FCS_STATE_SUSPEND_PROCESS) {
        // 50 milliseconds.
        set_status("running", "Running (" + current_iters_limit + " iterations)");
        setTimeout(resume_solution, 50);
        return;
    }
    else if (solve_err_code == FCS_STATE_WAS_SOLVED) {

        set_status("solved", "Solved");

        display_solution();

        return;
    }
    else {
        alert ("Unknown Error code " + solve_err_code + "!");
        throw "Foo";
    }
}

function resume_solution() {
    current_iters_limit += iters_step;

    freecell_solver_user_limit_iterations(obj, current_iters_limit);

    var solve_err_code = freecell_solver_user_resume_solution( obj );

    return handle_err_code(solve_err_code);
}

function do_solve() {

    set_status("running", "Running");

    try {
        // Clear to get a fresh solution.
        // out.val("");
        // out.text("");
        obj = freecell_solver_user_alloc();

        // TODO : add an option to customise the limit of the iterations count.

        if (obj == 0) {
            alert ("Could not allocate solver instance (out of memory?)");
            throw "Foo";
        }

        var cmd_line_preset = $("#preset").val();

        if (cmd_line_preset != "default") {
            var preset_ret = freecell_solver_user_cmd_line_read_cmd_line_preset(obj, cmd_line_preset, 0, 0, 0, null);

            if (preset_ret != 0) {
                alert ("Failed to load command line preset '" + cmd_line_preset + "'. Should not happen.");
                throw "Foo";
            }
        }

        current_iters_limit += iters_step;

        freecell_solver_user_limit_iterations(obj, current_iters_limit);

        // Removed for debugging purposes.
        // alert("preset_ret = " + preset_ret);

        var solve_err_code = freecell_solver_user_solve_board(
            obj, $("#stdin").val()
        );

        return handle_err_code(solve_err_code);
    }
    catch (e) {
        set_status("error", "Error");
    }
}

function display_solution() {
    try {
        reset_iterations();

        var out = $("#output");

        // 128 bytes are enough to hold a move.
        var move_buffer = malloc(128);

        if (move_buffer == 0) {
            alert ("Failed to allocate a buffer for the move (out of memory?)");
            throw "Foo";
        }

        var get_state_str = function () {
            var ptr = freecell_solver_user_current_state_as_string(obj, 1, 0, 1);

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
            // out.append(escapeHtml(str));
        };

        (function () {
            var state_as_string = get_state_str();
            my_append ( state_as_string + "\n\n");
        })();

        var move_ret_code;
        while ((move_ret_code = freecell_solver_user_get_next_move(obj, move_buffer)) == 0) {
            var state_as_string = get_state_str();
            var move_as_string_ptr = freecell_solver_user_move_ptr_to_string_w_state(obj, move_buffer, 0);

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
        freecell_solver_user_free(obj);

        set_status("solved", "Solved");
        out.val(out_buffer.replace(remove_trailing_space_re, ''));

    }
    catch (e) {
        return;
    }
}

// Thanks to Stefan Petrea ( http://garage-coding.com/ ) for inspiring this
// feature.
var previous_deal_idx = 1;

function populate_input_with_numbered_deal() {

    var new_idx = prompt("Enter deal Index:");

    // Prompt returned null (user cancelled).
    if (! new_idx) {
        return;
    }

    previous_deal_idx = parseInt(new_idx);

    $("#stdin").val(
        deal_ms_fc_board(previous_deal_idx)
    );

    return;
}
