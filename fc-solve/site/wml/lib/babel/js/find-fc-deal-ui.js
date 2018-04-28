"use strict";

if (typeof define !== 'function') {
    var define = require('amdefine')(module);
}

define(["web-fc-solve", "libfreecell-solver.min", 'dist/fc_solve_find_index_s2ints'], function (w, Module, s2i) {

    var FC_Solve_init_wrappers_with_module = w.FC_Solve_init_wrappers_with_module;
    var _my_module = Module()({});
    var deal_ms_fc_board = w.deal_ms_fc_board;
    FC_Solve_init_wrappers_with_module(_my_module);
    function set_up() {
        set_up_handlers();
    }

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
    // Taken from https://stackoverflow.com/questions/2901102/how-to-print-a-number-with-commas-as-thousands-separators-in-javascript
    // thanks.
    const numberWithCommas = (x) => {
        return x.toString().replace(/\B(?=(\d{3})+(?!\d))/g, ",");
    };

    function find_deal_ui() {
        const deal_str = $("#stdin").val().replace(/#[^\r\n]*\r?\n?/g, '').replace(/\r+\n/, "\n").replace(/([^\n])$/, "$1\n");
        const ints = s2i.find_index__board_string_to_ints(deal_str);
        const ints_s = ints.map((i) => { let ret = i.toString(); return " ".repeat(10-ret.length) + ret; }).join('');
        let df = new w.Freecell_Deal_Finder({});
        df.fill(ints_s);
        let ctl = $("#fc_solve_status");
        df.run(1, '8589934591',
            (args) => {
                ctl.html(escapeHtml("Reached No. " + numberWithCommas(args.start)));
                return;
            }
        );

        function resume() {
            const ret_Deal = df.cont();
            if (ret_Deal.found) {
                ctl.html("Found " + ret_Deal.result.toString());
            } else if (ret_Deal.cont) {
                setTimeout(() => { resume(); }, 1);
            } else {
                ctl.html("No such deal");
            }
        }

        resume();
    }

    function set_up_handlers() {
        $("#populate_input").click(populate_input_with_numbered_deal);
        $("#run_do_solve").click(find_deal_ui);
    }

    return { find_deal_ui: find_deal_ui, set_up: set_up, set_up_handlers: set_up_handlers};
});
