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

function _webui_output_set_text(text) {
    $("#output").val(text);

    return;
}

function _is_one_based_checked() {
    return $("#one_based").is(':checked');
}

function _one_based_process(text) {
    return text.replace(/^Move[^\n]+$/mg, function (move_s) {
        return move_s.replace(/(stack|freecell)( )(\d+)/g,
            function (match, resource_s, sep_s, digit_s) {
                return (resource_s + sep_s + (1 + parseInt(digit_s)));
            }
        );
    });
}

function _process_pristine_output(text) {
    return _is_one_based_checked() ? _one_based_process(text) : text;
}

var _pristine_output;

function _update_output () {
    _webui_output_set_text(_process_pristine_output(_pristine_output));

    return;
}

function on_toggle_one_based() {

    if ($("#output").val()) {
        _update_output();
    }

    return;
}


function clear_output() {
    return _webui_output_set_text('');
}

Class('FC_Solve_UI',
    {
        has: {
            _instance: { is: rw },
            _solve_err_code: { is: rw },
        },
        methods: {
            _re_enable_output: function() {
                $("#output").removeAttr("disabled");

                return;
            },
            _webui_set_status_callback: function(myclass, mylabel) {
                var that = this;

                var ctl = $("#fc_solve_status");
                ctl.removeClass();
                ctl.addClass(myclass);
                ctl.html(escapeHtml(mylabel));

                if (myclass == "exceeded") {
                    that._re_enable_output();
                }

                return;
            },
            _webui_set_output: function(buffer) {
                var that = this;

                _pristine_output = buffer;

                that._re_enable_output();

                _update_output();

                return;
            },
            _enqueue_resume: function () {
                var that = this;

                setTimeout(
                    function() { return that.do_resume(); },
                    50
                );

                return;
            },
            _handle_err_code: function() {
                var that = this;
                if (that._solve_err_code == FCS_STATE_SUSPEND_PROCESS) {
                    that._enqueue_resume();
                }

                return;
            },
            do_resume: function() {
                var that = this;

                that._solve_err_code = that._instance.resume_solution();

                that._handle_err_code();

                return;
            },
            _get_cmd_line_preset: function() {
                return $("#preset").val();
            },
            _calc_initial_board_string: function() {
                return $("#stdin").val().replace(/#[^\r\n]*\r?\n?/g, '');
            },
            _disable_output_display: function() {
                $("#output").attr("disabled", true);

                return;
            },
            do_solve: function() {
                var that = this;

                that._disable_output_display();

                that._instance = new FC_Solve({
                    cmd_line_preset: that._get_cmd_line_preset(),
                    set_status_callback: function(myclass, mylabel) {
                        return that._webui_set_status_callback(myclass, mylabel);
                    },
                    set_output: function(buffer) {
                        return that._webui_set_output(buffer);
                    },
                });

                that._solve_err_code = that._instance.do_solve(
                    that._calc_initial_board_string()
                );

                that._handle_err_code();

                return;
            },
        },
    }
);

var fcs_ui = new FC_Solve_UI();

function fc_solve_do_solve() {
    return fcs_ui.do_solve();
}

// Thanks to Stefan Petrea ( http://garage-coding.com/ ) for inspiring this
// feature.
var previous_deal_idx = 1;

function populate_input_with_numbered_deal() {

    var input_s = $('#deal_number').val();
    /*
    var new_idx = prompt("Enter MS Freecell deal number:");

    // Prompt returned null (user cancelled).
    if (! new_idx) {
        return;
    }

    previous_deal_idx = parseInt(new_idx);
    */

    if (! input_s.match(/^[1-9][0-9]*$/)) {
        alert("Wrong input - please enter a positive integer.");
        return;
    }

    previous_deal_idx = parseInt(input_s);

    $("#stdin").val(
        "# MS Freecell Deal #" + previous_deal_idx +
        "\n#\n" +
        deal_ms_fc_board(previous_deal_idx)
    );

    return;
}

function _get_loc() {
    return window.location;
}

function _get_base_url() {
    var loc = _get_loc();
    return loc.protocol + '//' + loc.host + loc.pathname;
}

var _bookmark_controls = ['stdin', 'preset', 'deal_number', 'one_based'];

function on_bookmarking() {
    var get_v = function(myid) {
        var ctl = $('#' + myid);
        return ctl.is(':checkbox') ?  (ctl.is(':checked') ? '1' : '0') : ctl.val();
    };

    var control_values = {};

    _bookmark_controls.forEach(function (myid) {
        control_values[myid] = get_v(myid);
    });

    var bookmark_string = _get_base_url() + '?' + $.querystring(control_values);

    $("#fcs_bm_results_input").val(bookmark_string);

    var a_elem = $("#fcs_bm_results_a");
    // Too long to be effective.
    // a_elem.text(bookmark_string);
    a_elem.attr('href', bookmark_string);

    $("#fcs_bookmark_wrapper").removeClass("disabled");

    return;
}

function restore_bookmark() {
    var qs = _get_loc().search;

    if (! qs.length) {
        return;
    }

    // Remove trailing 1.
    var params = $.querystring(qs.substr(1));

    _bookmark_controls.forEach(function (myid) {
        var ctl = $('#' + myid);
        if (ctl.is(':checkbox')) {
            ctl.prop('checked', ((params[myid] == "1") ? true : false));
        }
        else {
            ctl.val(params[myid]);
        }
    });

    return;
}
