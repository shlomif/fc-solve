import * as base_ui from "./fcs-base-ui";
import * as Module from "./libfcs-wrap";
import * as w from "./web-fc-solve";

const FC_Solve = w.FC_Solve;
const _my_module = Module()({});
const FCS_STATE_SUSPEND_PROCESS = w.FCS_STATE_SUSPEND_PROCESS;
const FCS_STATE_WAS_SOLVED = w.FCS_STATE_WAS_SOLVED;
w.FC_Solve_init_wrappers_with_module(_my_module);

function _increment_move_indices(move_s) {
    return move_s.replace(
        /(stack|freecell)( )(\d+)/g,
        (match, resource_s, sep_s, digit_s) => {
            return resource_s + sep_s + (1 + parseInt(digit_s, 10));
        },
    );
}

class FC_Solve_UI {
    private _instance: w.FC_Solve = null;
    private _solve_err_code = null;
    private _was_iterations_count_exceeded = false;
    private _is_expanded = false;
    private _pristine_outputs = null;
    constructor() {}
    public toggle_expand() {
        const that = this;

        const ret = (that._is_expanded = !that._is_expanded);

        that._webui_set_output();

        return ret;
    }
    public do_solve() {
        const that = this;

        that.clear_output();

        that._pristine_outputs = [null, null];

        that._disable_output_display();
        that._was_iterations_count_exceeded = false;

        that._instance = new FC_Solve({
            cmd_line_preset: that._get_cmd_line_preset(),
            dir_base: "fcs_ui",
            set_status_callback: (myclass, mylabel) => {
                return that._webui_set_status_callback(myclass, mylabel);
            },
            string_params: that._get_string_params(),
        });

        that._solve_err_code = that._instance.do_solve(
            that._calc_initial_board_string(),
        );

        that._handle_err_code();

        return;
    }
    public do_resume() {
        const that = this;

        that._solve_err_code = that._instance.resume_solution();

        that._handle_err_code();

        return;
    }
    public clear_output() {
        const that = this;
        that._webui_output_set_text("");
        that._set_html_output("");
        return;
    }
    public update_output() {
        const that = this;
        const displayer: w.DisplayFilter = that._calcDisplayFilter();

        that._webui_output_set_text(
            that._process_pristine_output(that._calc_pristine_output()),
        );

        if (that._solve_err_code === FCS_STATE_WAS_SOLVED) {
            let html = "";

            html += "<ol>\n";

            const inst = that._instance;
            const seq = inst.proto_states_and_moves_seq;

            function _filt(str) {
                return that._process_pristine_output(
                    inst.unicode_preprocess(str, displayer),
                );
            }

            function _render_state(s, pre_indexes) {
                return (
                    '<li class="state">' +
                    '<strong class="step_idx">' +
                    pre_indexes.map((i) => (i >> 1) + 1 + ".").join("") +
                    "</strong><pre>" +
                    _filt(s.str) +
                    "</pre></li>\n\n"
                );
            }

            function _out_state(i) {
                html += _render_state(seq[i], [i]);
            }

            _out_state(0);
            for (let i = 1; i < seq.length - 1; i += 2) {
                html +=
                    '<li id="move_' +
                    i +
                    '" class="move unexpanded">' +
                    '<span class="mega_move">' +
                    _filt(seq[i].m.str) +
                    '</span>\n<button id="expand_move_' +
                    i +
                    '" class="expand_move">Expand Move</button>\n</li>\n';

                _out_state(i + 1);
            }
            html += "</ol>\n";
            that._set_html_output(html);

            $("#dynamic_output").on("click", "button.expand_move", function(
                event,
            ) {
                const button = $(this);
                const b_id = button.attr("id");
                const idx = parseInt(
                    b_id.match(/^expand_move_([0-9]+)$/)[1],
                    10,
                );

                const move_ctl = $("#move_" + idx);
                const calc_class = "calced";
                if (!move_ctl.hasClass(calc_class)) {
                    const inner_moves = inst.calc_expanded_move(idx);

                    let inner_html = "";

                    inner_html += '<ol class="inner_moves">';

                    function _out_inner_move(i) {
                        inner_html +=
                            '<li class="move">' +
                            '<span class="inner_move">' +
                            _filt(inner_moves[i].str) +
                            "</span>\n</li>\n";
                        return;
                    }

                    for (let i = 0; i < inner_moves.length - 1; i += 2) {
                        _out_inner_move(i);
                        inner_html += _render_state(inner_moves[i + 1], [
                            idx,
                            i,
                        ]);
                    }
                    _out_inner_move(inner_moves.length - 1);
                    inner_html += "</ol>";
                    move_ctl.append(inner_html);
                    move_ctl.toggleClass(calc_class);
                }
                move_ctl.toggleClass("expanded");
                move_ctl.toggleClass("unexpanded");
                button.text(
                    move_ctl.hasClass("expanded")
                        ? "Unexpand move"
                        : "Expand move",
                );
            });
        }
        return;
    }

    private _is_one_based_checked() {
        return $("#one_based").is(":checked");
    }
    private _is_unicode_suits_checked() {
        return $("#unicode_suits__unisuits").is(":checked");
    }
    private _is_unicode_cards_checked() {
        return $("#unicode_suits__unicards").is(":checked");
    }
    private _webui_output_set_text(text) {
        $("#output").val(text);

        return;
    }
    private _one_based_process(text) {
        return text.replace(/^Move[^\n]+$/gm, _increment_move_indices);
    }
    private _process_pristine_output(text) {
        const that = this;

        return that._is_one_based_checked()
            ? that._one_based_process(text)
            : text;
    }
    private _set_html_output(html) {
        $("#dynamic_output").html(html);
        return;
    }

    private _re_enable_output() {
        $("#output").removeAttr("disabled");

        return;
    }
    private _webui_set_status_callback(myclass, mylabel) {
        const that = this;

        const ctl = $("#fc_solve_status");
        ctl.removeClass();
        ctl.addClass(myclass);
        ctl.html(base_ui.escapeHtml(mylabel));

        const is_exceed = myclass === "exceeded";

        if (is_exceed) {
            that._was_iterations_count_exceeded = true;
        }

        if (is_exceed || myclass === "impossible") {
            that._re_enable_output();
        }

        return;
    }
    private _calc_pristine_output() {
        const that = this;

        if (that._solve_err_code === FCS_STATE_WAS_SOLVED) {
            const _expand = that._is_expanded;
            const _k = _expand ? 1 : 0;
            const _o = that._pristine_outputs;
            return (_o[_k] =
                _o[_k] ||
                that._instance.generic_display_sol({
                    displayer: that._calcDisplayFilter(),
                    expand: _expand,
                }));
        } else {
            return "";
        }
    }
    private _webui_set_output() {
        const that = this;

        that._re_enable_output();

        that.update_output();

        return;
    }
    private _enqueue_resume() {
        const that = this;

        setTimeout(() => {
            return that.do_resume();
        }, 50);

        return;
    }
    private _handle_err_code() {
        const that = this;
        if (that._solve_err_code === FCS_STATE_WAS_SOLVED) {
            that._webui_set_output();
        } else if (
            that._solve_err_code === FCS_STATE_SUSPEND_PROCESS &&
            !that._was_iterations_count_exceeded
        ) {
            that._enqueue_resume();
        }

        return;
    }
    private _get_string_params() {
        const text = $("#string_params").val() as string;
        return (
            "--game " +
            $("#game_type").val() +
            " " +
            (text.match(/\S/) ? text : "")
        );
    }
    private _get_cmd_line_preset() {
        return $("#preset").val();
    }
    private _calc_initial_board_string() {
        return ($("#stdin").val() as string).replace(/#[^\r\n]*\r?\n?/g, "");
    }
    private _disable_output_display() {
        $("#output").attr("disabled", "true");

        return;
    }
    private _calcDisplayFilter(): w.DisplayFilter {
        const that = this;
        return new w.DisplayFilter({
            is_unicode_cards:
                that._is_unicode_suits_checked() ||
                that._is_unicode_cards_checked(),
            is_unicode_cards_chars: that._is_unicode_cards_checked(),
        });
    }
}

function _create_bmark_obj() {
    return new base_ui.FC_Solve_Bookmarking({
        bookmark_controls: [
            "stdin",
            "preset",
            "deal_number",
            "one_based",
            "unicode_suits__ascii",
            "unicode_suits__unicards",
            "unicode_suits__unisuits",
            "string_params",
            "game_type",
        ],
        show: [],
    });
}

function on_bookmarking() {
    return _create_bmark_obj().on_bookmarking();
}

function restore_bookmark() {
    return _create_bmark_obj().restore_bookmark();
}

const fcs_ui = new FC_Solve_UI();

function fc_solve_do_solve() {
    return fcs_ui.do_solve();
}

function on_toggle_one_based() {
    if ($("#output").val()) {
        fcs_ui.update_output();
    }
    return;
}

function clear_output() {
    return fcs_ui.clear_output();
}

function toggle_expand_moves() {
    const is_expanded = fcs_ui.toggle_expand();

    $("#expand_moves_toggle").text(
        is_expanded ? "Unexpand Moves" : "Expand Moves",
    );
    return;
}

export function set_up_handlers() {
    $("#populate_input").click(base_ui.populate_input_with_numbered_deal);
    $("#run_do_solve").click(fc_solve_do_solve);
}

export function set_up() {
    restore_bookmark();
    set_up_handlers();
    $("#one_based").click(on_toggle_one_based);
    $("#clear_output").click(clear_output);
    $("#fc_solve_bookmark_button").click(on_bookmarking);
    $('input[name="unicode_suits"]').change(() => {
        fcs_ui.update_output();
    });
}
