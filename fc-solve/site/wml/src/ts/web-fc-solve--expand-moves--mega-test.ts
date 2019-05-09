import * as w from "./web-fc-solve";
import * as Module from "./libfcs-wrap";
"use strict";

const impossible_deal = 11982;
const last_deal = 32000;
const preset = "as";

function test_idx(ajaxQueue, idx) {
    $("#deal_idx_update").html("Reached deal idx=" + idx);

    if (idx > last_deal) {
        alert("Success. Yay!");
        return;
    } else if (idx === impossible_deal) {
        return test_idx(ajaxQueue, idx + 1);
    } else {
        ajaxQueue({
            url:
                "/fc-solve-ajax/run-fc-solve/idx=" +
                idx +
                "/preset=" +
                preset +
                ".json",
            dataType: "json",
        }).done((data) => {
            let success = false;

            const instance = new w.FC_Solve({
                cmd_line_preset: preset,
                set_status_callback: () => {
                    return;
                },
            });

            let solve_err_code = instance.do_solve(w.deal_ms_fc_board(idx));

            while (solve_err_code === w.FCS_STATE_SUSPEND_PROCESS) {
                solve_err_code = instance.resume_solution();
            }

            if (solve_err_code === w.FCS_STATE_WAS_SOLVED) {
                const buffer = instance.display_expanded_moves_solution({
                    displayer: new w.DisplayFilter({
                        is_unicode_cards: false,
                        is_unicode_cards_chars: false,
                    }),
                });
                success = true;
                // TEST
                if (buffer !== data.ret_text) {
                    $("#ajax_out").val(data.ret_text);
                    $("#ajax_out").val();
                    $("#js_fcs_out").val(buffer);
                    alert("Results mismatch for idx " + idx);
                    success = false;
                    throw "Results mismatch: idx=" + idx;
                }
            }

            // TEST
            if (!success) {
                const msg = "idx=" + idx + " was not succesful.";
                alert(msg);
                throw msg;
            }

            return test_idx(ajaxQueue, idx + 1);
        });
        return;
    }
}

export function test_js_fc_solve_class(ajaxQueue) {
    const _my_module = Module()({});
    w.FC_Solve_init_wrappers_with_module(_my_module);
    return test_idx(ajaxQueue, 1);
}
