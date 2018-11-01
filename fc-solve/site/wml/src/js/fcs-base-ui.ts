import { jq_querystring } from "./jq_qs";
import * as w from "./web-fc-solve";
// import jq_querystring as jq_querystring;

const entityMap = {
    '"': "&quot;",
    "&": "&amp;",
    "'": "&#39;",
    "/": "&#x2F;",
    "<": "&lt;",
    ">": "&gt;",
};

export function escapeHtml(str: string): string {
    return String(str).replace(/[&<>"'\/]/g, (s) => entityMap[s]);
}

// Thanks to Stefan Petrea ( http://garage-coding.com/ ) for inspiring this
// feature.
export function populate_input_with_numbered_deal(): void {
    const input_s: string = $("#deal_number").val() as string;
    if (!input_s.match(/^[1-9][0-9]*$/)) {
        alert("Wrong input - please enter a positive integer.");
        return;
    }

    const previous_deal_idx = parseInt(input_s, 10);

    $("#stdin").val(
        "# MS Freecell Deal #" +
            previous_deal_idx +
            "\n#\n" +
            w.deal_ms_fc_board(previous_deal_idx),
    );

    return;
}

export class FC_Solve_Bookmarking {
    private bookmark_controls: [string];
    private show;
    constructor(args) {
        const that = this;

        that.bookmark_controls = args.bookmark_controls;
        that.show = args.show;

        return;
    }
    public on_bookmarking(): void {
        const that = this;

        function get_v(myid: string): string {
            const ctl = $("#" + myid);
            return that._is_toggley(ctl)
                ? ctl.is(":checked")
                    ? "1"
                    : "0"
                : (ctl.val() as string);
        }

        const control_values = {};

        that._each_control((myid) => {
            control_values[myid] = get_v(myid);
        });

        const bookmark_string =
            that._get_base_url() + "?" + jq_querystring(control_values);

        $("#fcs_bm_results_input").val(bookmark_string);

        const a_elem = $("#fcs_bm_results_a");
        // Too long to be effective.
        // a_elem.text(bookmark_string);
        a_elem.attr("href", bookmark_string);

        $("#fcs_bookmark_wrapper").removeClass("disabled");

        return;
    }
    public restore_bookmark() {
        const that = this;

        const qs = that._get_loc().search;

        if (!qs.length) {
            return;
        }

        // Remove trailing 1.
        const params = jq_querystring(qs.substr(1));

        that._each_control((myid) => {
            const ctl = $("#" + myid);
            if (that._is_toggley(ctl)) {
                ctl.prop(
                    "checked",
                    String(params[myid]) === "1" ? true : false,
                );
            } else {
                ctl.val(params[myid]);
            }
        });

        that.show.forEach((rec) => {
            const id = rec.id;
            const deps = rec.deps;

            let should_toggle = false;
            deps.forEach((d) => {
                if (($("#" + d).val() as string).length > 0) {
                    should_toggle = true;
                }
            });

            if (should_toggle) {
                if ($("#" + id).hasClass("disabled")) {
                    rec.callback();
                }
            }
        });

        return;
    }
    private _is_toggley(ctl) {
        return ctl.is(":checkbox") || ctl.is(":radio");
    }
    private _get_loc() {
        return window.location;
    }
    private _get_base_url() {
        const that = this;

        const loc = that._get_loc();
        return loc.protocol + "//" + loc.host + loc.pathname;
    }
    private _each_control(cb: ((string) => void)): void {
        const that = this;

        that.bookmark_controls.forEach(cb);

        return;
    }
}
