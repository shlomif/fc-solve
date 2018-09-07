"use strict";

define(["fcs-base-ui", "web-fc-solve", "libfcs-wrap",
    'dist/fc_solve_find_index_s2ints'], function(base_ui, w, Module, s2i) {
    let _my_module = Module()({});
    w.FC_Solve_init_wrappers_with_module(_my_module);

    function _create_bmark_obj() {
        return new base_ui.FC_Solve_Bookmarking({
            bookmark_controls: ['stdin', 'deal_number'],
            show: [],
        });
    }

    function on_bookmarking() {
        return _create_bmark_obj().on_bookmarking();
    }

    function restore_bookmark() {
        return _create_bmark_obj().restore_bookmark();
    }
    // Taken from https://stackoverflow.com/questions/2901102/how-to-print-a-number-with-commas-as-thousands-separators-in-javascript
    // thanks.
    const numberWithCommas = (x) => {
        return x.toString().replace(/\B(?=(\d{3})+(?!\d))/g, ",");
    };

    function find_deal_ui() {
        const deal_str = $("#stdin").val().replace(/#[^\r\n]*\r?\n?/g, '').
            replace(/\r+\n/, "\n").replace(/([^\n])$/, "$1\n");
        const ints = s2i.find_index__board_string_to_ints(deal_str);
        const ints_s = ints.map((i) => {
            let ret = i.toString();
            return " ".repeat(10-ret.length) + ret;
        }).join('');
        let df = new w.Freecell_Deal_Finder({});
        df.fill(ints_s);
        let ctl = $("#fc_solve_status");
        df.run(1, '8589934591',
            (args) => {
                ctl.html(base_ui.escapeHtml(
                    "Reached No. " + numberWithCommas(args.start)
                ));
                return;
            }
        );

        function resume() {
            const ret_Deal = df.cont();
            if (ret_Deal.found) {
                ctl.html("Found " + ret_Deal.result.toString());
            } else if (ret_Deal.cont) {
                setTimeout(() => {
                    resume();
                }, 1);
            } else {
                ctl.html("No such deal");
            }
        }

        resume();
    }

    function set_up_handlers() {
        $("#populate_input").click(base_ui.populate_input_with_numbered_deal);
        $("#run_do_solve").click(find_deal_ui);
    }

    function set_up() {
        restore_bookmark();
        set_up_handlers();
        $("#fc_solve_bookmark_button").click(on_bookmarking);
    }

    return {
        find_deal_ui: find_deal_ui,
        set_up: set_up,
        set_up_handlers: set_up_handlers,
    };
});
