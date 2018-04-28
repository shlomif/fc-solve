"use strict";

if (typeof define !== 'function') {
    var define = require('amdefine')(module);
}

define(["fcs-base-ui", "web-fc-solve", "libfreecell-solver.min"], function (base_ui, w, Module) {
    var FC_Solve = w.FC_Solve;
    var _my_module = Module()({});
    var FCS_STATE_SUSPEND_PROCESS = w.FCS_STATE_SUSPEND_PROCESS;
    var FCS_STATE_WAS_SOLVED = w.FCS_STATE_WAS_SOLVED;
    w.FC_Solve_init_wrappers_with_module(_my_module);

function _increment_move_indices(move_s) {
    return move_s.replace(/(stack|freecell)( )(\d+)/g,
        function (match, resource_s, sep_s, digit_s) {
            return (resource_s + sep_s +
                (1 + parseInt(digit_s))
            );
        }
    );
}

class FC_Solve_UI {
    constructor() {
        var that = this;
        that._instance = null;
        that._solve_err_code = null;
        that._was_iterations_count_exceeded = false;
        that._is_expanded = false;
        that._pristine_outputs = null;

        return;
    }

    _is_one_based_checked() {
        return $("#one_based").is(':checked');
    }
    _is_unicode_suits_checked() {
        return $("#unicode_suits").is(':checked');
    }
    _webui_output_set_text(text) {
        $("#output").val(text);

        return;
    }
    _one_based_process(text) {
        return text.replace(/^Move[^\n]+$/mg, _increment_move_indices);
    }
    _process_pristine_output(text) {
        var that = this;

        return (that._is_one_based_checked()
            ? that._one_based_process(text)
            : text
        );
    }
    _update_output() {
        var that = this;

        that._webui_output_set_text(
            that._process_pristine_output(that._calc_pristine_output())
        );

        if (that._solve_err_code == FCS_STATE_WAS_SOLVED ) {
            var html = '';

            html += "<ol>\n";

            var inst = that._instance;
            var seq = inst._proto_states_and_moves_seq;

            var _filt = function(str) {
                return that._process_pristine_output(
                    inst.unicode_preprocess(
                        str
                    )
                );
            };

            var _render_state = function(s) {
                return "<li class=\"state\"><pre>" + _filt(s.str) + "</pre></li>\n\n";
            };

            var _out_state = function(i) {
                html += _render_state(seq[i]);
            };

            _out_state(0);
            for (var i = 1; i < seq.length - 1; i+=2) {
                html += "<li id=\"move_" + i + "\" class=\"move unexpanded\"><span class=\"mega_move\">" + _filt(seq[i].m.str) + "</span>\n<button id=\"expand_move_" + i + "\" class=\"expand_move\">Expand Move</button>\n</li>\n";

                _out_state(i+1);
            }
            $("#dynamic_output").html(html);

            $("#dynamic_output").on("click", "button.expand_move", function (event) {
                var button = $(this);
                var b_id = button.attr('id');
                var idx = parseInt(b_id.match(/^expand_move_([0-9]+)$/)[1]);

                var move_ctl = $("#move_" + idx);
                var calc_class = 'calced';
                if (! move_ctl.hasClass(calc_class)) {
                    var inner_moves = inst._calc_expanded_move(idx);

                    var inner_html = '';

                    inner_html += "<ol class=\"inner_moves\">";

                    var _out_inner_move = function(i) {
                        inner_html += "<li class=\"move\"><span class=\"inner_move\">" + _filt(inner_moves[i].str) + "</span>\n</li>\n";
                        return;
                    }
                    for (var i = 0; i < inner_moves.length-1 ; i += 2) {
                        _out_inner_move(i);
                        inner_html += _render_state(inner_moves[i+1]);
                    }
                    _out_inner_move(inner_moves.length-1);
                    inner_html += "</ol>";
                    move_ctl.append(
                        inner_html
                    );
                    move_ctl.toggleClass(calc_class);
                }
                move_ctl.toggleClass("expanded");
                move_ctl.toggleClass("unexpanded");
                button.text(move_ctl.hasClass("expanded") ? "Unexpand move" : "Expand move");
            }
            );
        }
        return;
    }

    _re_enable_output() {
        $("#output").removeAttr("disabled");

        return;
    }
    _webui_set_status_callback(myclass, mylabel) {
        var that = this;

        var ctl = $("#fc_solve_status");
        ctl.removeClass();
        ctl.addClass(myclass);
        ctl.html(base_ui.escapeHtml(mylabel));

        var is_exceed = (myclass == "exceeded");

        if (is_exceed) {
            that._was_iterations_count_exceeded = true;
        }

        if (is_exceed || (myclass == "impossible")) {
            that._re_enable_output();
        }

        return;
    }
    _calc_pristine_output(buffer) {
        var that = this;

        if (that._solve_err_code == FCS_STATE_WAS_SOLVED ) {
            var _expand = that._is_expanded;
            var _k = _expand ? 1 : 0;
            var _o = that._pristine_outputs;
            return (
                _o[_k] = (_o[_k] || that._instance.generic_display_sol(
                    {
                        expand: _expand,
                    }
                )
                )
            );
        }
        else {
            return "";
        }
    }
    _webui_set_output(buffer) {
        var that = this;

        that._re_enable_output();

        that._update_output();

        return;
    }
    _enqueue_resume() {
        var that = this;

        setTimeout(
            function() { return that.do_resume(); },
            50
        );

        return;
    }
    _handle_err_code() {
        var that = this;
        if (that._solve_err_code == FCS_STATE_WAS_SOLVED ) {
            that._webui_set_output();
        } else if (that._solve_err_code == FCS_STATE_SUSPEND_PROCESS
            && !that._was_iterations_count_exceeded) {
            that._enqueue_resume();
        }

        return;
    }
    do_resume() {
        var that = this;

        that._solve_err_code = that._instance.resume_solution();

        that._handle_err_code();

        return;
    }
    _get_string_params() {
        var text = $("#string_params").val();
        return ('--game ' + $("#game_type").val() + ' ' + (text.match(/\S/) ? text : ''));
    }
    _get_cmd_line_preset() {
        return $("#preset").val();
    }
    _calc_initial_board_string() {
        return $("#stdin").val().replace(/#[^\r\n]*\r?\n?/g, '');
    }
    _disable_output_display() {
        $("#output").attr("disabled", true);

        return;
    }
    do_solve() {
        var that = this;

        that._pristine_outputs = [null,null];

        that._disable_output_display();
        that._was_iterations_count_exceeded = false;

        that._instance = new FC_Solve({
            cmd_line_preset: that._get_cmd_line_preset(),
            set_status_callback: function(myclass, mylabel) {
                return that._webui_set_status_callback(myclass, mylabel);
            },
            is_unicode_cards: that._is_unicode_suits_checked(),
            dir_base: 'fcs_ui',
            string_params: that._get_string_params(),
        });

        that._solve_err_code = that._instance.do_solve(
            that._calc_initial_board_string()
        );

        that._handle_err_code();

        return;
    }
    toggle_expand() {
        var that = this;

        var ret = (that._is_expanded = ! that._is_expanded);

        that._webui_set_output();

        return ret;
    }
}

function toggle_advanced() {
    var ctl = $("#fcs_advanced");
    ctl.toggleClass("disabled");

    var set_text = function (my_text) {
        $("#fcs_advanced_toggle").text(my_text);
    };

    set_text(
        ctl.hasClass("disabled") ? "Advanced ▼" : "Advanced ▲"
    );

    return;
}

function _create_bmark_obj() {
    return new base_ui.FC_Solve_Bookmarking({ bookmark_controls: ['stdin', 'preset', 'deal_number', 'one_based', 'unicode_suits', 'string_params', 'game_type', ], show: [{ id: 'fcs_advanced', deps: ['string_params',], callback: toggle_advanced,}, ],});
}

function on_bookmarking() {
    return _create_bmark_obj().on_bookmarking();
}

function restore_bookmark() {
    return _create_bmark_obj().restore_bookmark();
}

var fcs_ui = new FC_Solve_UI();

function fc_solve_do_solve() {
    return fcs_ui.do_solve();
}

function on_toggle_one_based() {

    if ($("#output").val()) {
        fcs_ui._update_output();
    }

    return;
}

function clear_output() {
    return fcs_ui._webui_output_set_text('');
}

function toggle_expand_moves() {

    var is_expanded = fcs_ui.toggle_expand();

    $("#expand_moves_toggle").text(
        is_expanded ? "Unexpand Moves" : "Expand Moves"
    );
    return;
}

function set_up_handlers() {
    $("#populate_input").click(base_ui.populate_input_with_numbered_deal);
    $("#run_do_solve").click(fc_solve_do_solve);
}

function set_up() {
    restore_bookmark();
    set_up_handlers();
    $("#fcs_advanced_toggle").click(toggle_advanced);
    $("#one_based").click(on_toggle_one_based);
    $("#clear_output").click(clear_output);
    $("#fc_solve_bookmark_button").click(on_bookmarking);
}

    return { set_up: set_up, set_up_handlers: set_up_handlers, };
});
