"use strict";

define(["fcs-base-ui", "web-fc-solve", "libfcs-wrap"],
    function(base_ui, w, Module) {
        let FC_Solve = w.FC_Solve;
        let _my_module = Module()({});
        const FCS_STATE_SUSPEND_PROCESS = w.FCS_STATE_SUSPEND_PROCESS;
        const FCS_STATE_WAS_SOLVED = w.FCS_STATE_WAS_SOLVED;
        w.FC_Solve_init_wrappers_with_module(_my_module);

        function _increment_move_indices(move_s) {
            return move_s.replace(/(stack|freecell)( )(\d+)/g,
                function(match, resource_s, sep_s, digit_s) {
                    return (resource_s + sep_s + (1 + parseInt(digit_s)));
                }
            );
        }

        class FC_Solve_UI {
            constructor() {
                let that = this;
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
                let that = this;

                return (that._is_one_based_checked()
                    ? that._one_based_process(text)
                    : text
                );
            }
            _set_html_output(html) {
                $("#dynamic_output").html(html);
                return;
            }
            _update_output() {
                let that = this;

                that._webui_output_set_text(
                    that._process_pristine_output(that._calc_pristine_output())
                );

                if (that._solve_err_code == FCS_STATE_WAS_SOLVED ) {
                    let html = '';

                    html += "<ol>\n";

                    let inst = that._instance;
                    let seq = inst._proto_states_and_moves_seq;

                    let _filt = function(str) {
                        return that._process_pristine_output(
                            inst.unicode_preprocess(
                                str
                            )
                        );
                    };

                    let _render_state = function(s, pre_indexes) {
                        return "<li class=\"state\">" +
                            "<strong class=\"step_idx\">" +
                    pre_indexes.map((i) => (
                        ((i >> 1)+1)+".")
                    ).join('') + "</strong><pre>" + _filt(s.str) +
                    "</pre></li>\n\n";
                    };

                    let _out_state = function(i) {
                        html += _render_state(seq[i], [i]);
                    };

                    _out_state(0);
                    for (let i = 1; i < seq.length - 1; i+=2) {
                        html += "<li id=\"move_" + i +
                    "\" class=\"move unexpanded\">" +
                    "<span class=\"mega_move\">" + _filt(seq[i].m.str) +
                    "</span>\n<button id=\"expand_move_" + i +
                    "\" class=\"expand_move\">Expand Move</button>\n</li>\n";

                        _out_state(i+1);
                    }
                    html += "</ol>\n";
                    that._set_html_output(html);

                    $("#dynamic_output").on("click", "button.expand_move",
                        function(event) {
                            let button = $(this);
                            let b_id = button.attr('id');
                            let idx = parseInt(b_id.match(
                                /^expand_move_([0-9]+)$/)[1]);

                            let move_ctl = $("#move_" + idx);
                            let calc_class = 'calced';
                            if (! move_ctl.hasClass(calc_class)) {
                                let inner_moves = inst._calc_expanded_move(idx);

                                let inner_html = '';

                                inner_html += "<ol class=\"inner_moves\">";

                                let _out_inner_move = function(i) {
                                    inner_html += "<li class=\"move\">" +
                            "<span class=\"inner_move\">" +
                            _filt(inner_moves[i].str) +
                            "</span>\n</li>\n";
                                    return;
                                };
                                for (let i = 0; i < inner_moves.length-1;
                                    i += 2) {
                                    _out_inner_move(i);
                                    inner_html += _render_state(
                                        inner_moves[i+1],
                                        [idx, i]
                                    );
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
                            button.text(move_ctl.hasClass("expanded") ?
                                "Unexpand move" : "Expand move");
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
                let that = this;

                let ctl = $("#fc_solve_status");
                ctl.removeClass();
                ctl.addClass(myclass);
                ctl.html(base_ui.escapeHtml(mylabel));

                let is_exceed = (myclass == "exceeded");

                if (is_exceed) {
                    that._was_iterations_count_exceeded = true;
                }

                if (is_exceed || (myclass == "impossible")) {
                    that._re_enable_output();
                }

                return;
            }
            _calc_pristine_output(buffer) {
                let that = this;

                if (that._solve_err_code == FCS_STATE_WAS_SOLVED ) {
                    let _expand = that._is_expanded;
                    let _k = _expand ? 1 : 0;
                    let _o = that._pristine_outputs;
                    return (
                        _o[_k] = (_o[_k] || that._instance.generic_display_sol(
                            {
                                expand: _expand,
                            }
                        )
                        )
                    );
                } else {
                    return "";
                }
            }
            _webui_set_output(buffer) {
                let that = this;

                that._re_enable_output();

                that._update_output();

                return;
            }
            _enqueue_resume() {
                let that = this;

                setTimeout(
                    function() {
                        return that.do_resume();
                    },
                    50
                );

                return;
            }
            _handle_err_code() {
                let that = this;
                if (that._solve_err_code == FCS_STATE_WAS_SOLVED ) {
                    that._webui_set_output();
                } else if (that._solve_err_code == FCS_STATE_SUSPEND_PROCESS
            && !that._was_iterations_count_exceeded) {
                    that._enqueue_resume();
                }

                return;
            }
            do_resume() {
                let that = this;

                that._solve_err_code = that._instance.resume_solution();

                that._handle_err_code();

                return;
            }
            _get_string_params() {
                let text = $("#string_params").val();
                return ('--game ' + $("#game_type").val() + ' ' +
                (text.match(/\S/) ? text : ''));
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
            _clear_output() {
                let that = this;
                that._webui_output_set_text('');
                that._set_html_output('');
                return;
            }
            do_solve() {
                let that = this;

                that._clear_output();

                that._pristine_outputs = [null, null];

                that._disable_output_display();
                that._was_iterations_count_exceeded = false;

                that._instance = new FC_Solve({
                    cmd_line_preset: that._get_cmd_line_preset(),
                    set_status_callback: function(myclass, mylabel) {
                        return that._webui_set_status_callback(
                            myclass, mylabel);
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
                let that = this;

                const ret = (that._is_expanded = ! that._is_expanded);

                that._webui_set_output();

                return ret;
            }
        }

        function _create_bmark_obj() {
            return new base_ui.FC_Solve_Bookmarking({
                bookmark_controls: [
                    'stdin', 'preset', 'deal_number', 'one_based',
                    'unicode_suits', 'string_params', 'game_type',
                ],
            });
        }

        function on_bookmarking() {
            return _create_bmark_obj().on_bookmarking();
        }

        function restore_bookmark() {
            return _create_bmark_obj().restore_bookmark();
        }

        let fcs_ui = new FC_Solve_UI();

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
            return fcs_ui._clear_output();
        }

        function toggle_expand_moves() {
            const is_expanded = fcs_ui.toggle_expand();

            $("#expand_moves_toggle").text(
                is_expanded ? "Unexpand Moves" : "Expand Moves"
            );
            return;
        }

        function set_up_handlers() {
            $("#populate_input").click(
                base_ui.populate_input_with_numbered_deal
            );
            $("#run_do_solve").click(fc_solve_do_solve);
        }

        function set_up() {
            restore_bookmark();
            set_up_handlers();
            $("#one_based").click(on_toggle_one_based);
            $("#clear_output").click(clear_output);
            $("#fc_solve_bookmark_button").click(on_bookmarking);
        }

        return {set_up: set_up, set_up_handlers: set_up_handlers};
    });
