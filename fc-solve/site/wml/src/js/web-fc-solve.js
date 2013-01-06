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

function do_solve() {

    set_status("running", "Running");

    try {
        var out = $("#output");

        // Clear to get a fresh solution.
        // out.val("");
        // out.text("");

        // TODO : add an option to customise the limit of the iterations count.
        var obj = freecell_solver_user_alloc();

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

        freecell_solver_user_limit_iterations(obj, 128*1024);

        // Removed for debugging purposes.
        // alert("preset_ret = " + preset_ret);

        var solve_err_code = freecell_solver_user_solve_board(
            obj, $("#stdin").val()
        );

        if (solve_err_code != 0) {
            alert ("Failed to solve board (invalid layout?)");
            throw "Foo";
        }

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
        set_status("error", "Error");
    }

    return;
}

// Thanks to Stefan Petrea ( http://garage-coding.com/ ) for inspiring this
// feature.
function populate_input_with_sample_board() {
    $("#stdin").val(": 9D 6S 4D AH 5C 4H KH\n: JH JC JS 7S 6D AC 4S\n: AD 8D AS 2H KC KD 7H\n: KS 10S 9S 8H JD QD 3H\n: 10C 4C 2S 5D 3S 6H\n: 3C QC 5S QS QH 7D\n: 9H 5H 7C 6C 3D 10D\n: 2C 8C 8S 10H 2D 9C\n");
}
