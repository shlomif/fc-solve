import { jq_querystring } from "./jq_qs";
import * as BaseApi from "./web-fcs-api-base";
import { capitalize_cards } from "./fcs-validate";

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
export function populate_input_with_numbered_deal(
    module_wrapper: BaseApi.ModuleWrapper,
    w: any,
): void {
    const input_s: string = $("#deal_number").val() as string;
    if (!input_s.match(/^[1-9][0-9]*$/)) {
        alert("Wrong input - please enter a positive integer.");
        return;
    }
    const MAX = 8589934591;
    const previous_deal_idx = parseInt(input_s, 10);
    if (input_s.length > 10 || previous_deal_idx > MAX) {
        alert("Wrong input - deal number must be in the range 1 to " + MAX);
        return;
    }

    const stdin = $("#stdin");
    stdin.val(
        "# MS Freecell Deal #" +
            previous_deal_idx +
            "\n#\n" +
            BaseApi.deal_ms_fc_board(module_wrapper, previous_deal_idx),
    );
    // For tests:
    stdin.trigger("change");

    return;
}

export function set_up__capitalize_cards(): void {
    const capitalize_cards_widget = $("#capitalize_cards");
    capitalize_cards_widget.off("click");
    capitalize_cards_widget.click(() => {
        const ctl = $("#stdin");
        ctl.val(capitalize_cards(ctl.val() as string));
        return;
    });
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
    public restore_bookmark(): void {
        const that = this;

        const qs = that._get_loc().search;

        if (!qs.length) {
            return;
        }

        // Remove trailing 1.
        const params = jq_querystring(qs.substr(1));

        that._each_control((myid) => {
            const ctl = $("#" + myid);
            const val = params[myid];
            if (that._is_toggley(ctl)) {
                ctl.prop("checked", String(val) === "1" ? true : false);
            } else {
                ctl.val(val);
            }
        });

        for (const rec of that.show) {
            const id = rec.id;
            const deps = rec.deps;

            const should_toggle = deps.some((d) => {
                return ($("#" + d).val() as string).length > 0;
            });

            if (should_toggle) {
                if ($("#" + id).hasClass("disabled")) {
                    rec.callback();
                }
            }
        }

        return;
    }
    private _is_toggley(ctl) {
        return ctl.is(":checkbox") || ctl.is(":radio");
    }
    private _get_loc() {
        return window.location;
    }
    private _get_base_url(): string {
        const that = this;

        const loc = that._get_loc();
        return loc.protocol + "//" + loc.host + loc.pathname;
    }
    private _each_control(cb: (string) => void): void {
        const that = this;

        that.bookmark_controls.forEach(cb);

        return;
    }
}
