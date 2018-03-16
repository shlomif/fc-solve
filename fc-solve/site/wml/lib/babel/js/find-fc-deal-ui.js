"use strict";

if (typeof define !== 'function') {
    var define = require('amdefine')(module);
}

define(["web-fc-solve", "libfreecell-solver.min", 'dist/fc_solve_find_index_s2ints'], function (w, Module, s2i) {

    var FC_Solve_init_wrappers_with_module = w.FC_Solve_init_wrappers_with_module;
    var _my_module = Module()({});
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

    function find_deal_ui() {
        const deal_str = $("#stdin").val().replace(/#[^\r\n]*\r?\n?/g, '');
        const ints = s2i.find_index__board_string_to_ints(deal_str);
        const ints_s = ints.map((i) => { let ret = i.toString(); return " ".repeat(10-ret.length) + ret; }).join('');
        let df = new w.Freecell_Deal_Finder({});
        df.fill(ints_s);
        let ctl = $("#fc_solve_status");
        df.run(1, '8589934591',
            (args) => {
                ctl.html(escapeHtml("Reached No. " + args.start.toString()));
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
        $("#run_do_solve").click(find_deal_ui);
    }

    return { find_deal_ui: find_deal_ui, set_up: set_up, set_up_handlers: set_up_handlers};
});
