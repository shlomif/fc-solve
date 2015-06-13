"use strict";

var impossible_deal = 11982;
var last_deal = 32000;
var preset = 'as';

function test_idx(idx)
{
    $('#deal_idx_update').html("Reached deal idx=" + idx);

    if (idx > last_deal) {
        alert('Success. Yay!');
        return;
    } else if (idx == impossible_deal) {
        return test_idx(idx+1);
    } else {
        jQuery.ajaxQueue({
            url: '/fc-solve-ajax/run-fc-solve/idx=' + idx + '/preset=' + preset + '.json',
            dataType: "json"
        }).done(function( data ) {

            var success = false;

            var instance = new FC_Solve({
                cmd_line_preset: preset,
                set_status_callback: function () { return; },
            });

            var solve_err_code = instance.do_solve(deal_ms_fc_board(idx));

            while (solve_err_code == FCS_STATE_SUSPEND_PROCESS) {
                solve_err_code = instance.resume_solution();
            }

            if (solve_err_code == FCS_STATE_WAS_SOLVED) {
                instance.display_expanded_moves_solution(
                    {
                        output_cb: function (buffer) {
                            success = true;
                            // TEST
                            if (buffer != data.ret_text) {
                                $("#ajax_out").val(data.ret_text);
                                $("#js_fcs_out").val(buffer);
                                alert("Results mismatch for idx " + idx);
                                success = false;
                                throw ("Results mismatch: idx=" + idx);
                            }
                        }
                    }
                );
            }

            // TEST
            if (!success) {
                alert ("success for idx=" + idx + " was not true.");
                throw ("success for idx=" + idx + " was not true.");
            }

            return test_idx(idx+1);
        });
        return;
    }
}

function test_js_fc_solve_class()
{
    return test_idx(1);
}
