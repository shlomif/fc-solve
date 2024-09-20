import * as s2i from "./s2ints_js";
import * as BaseApi from "./web-fcs-api-base";
import * as base_ui from "./fcs-base-ui";
import Module from "./libfind-deal-wrap";
import * as w from "./find-fc-deal";

let _module_wrapper: w.ModuleWrapper;

export function init_module(cb: (mw: BaseApi.ModuleWrapper) => any): any {
    const _my_module = Module({
        onRuntimeInitialized: () => {
            _my_module.then((result) => {
                const module_wrapper =
                    w.FC_Solve_init_wrappers_with_module(result);
                _module_wrapper = module_wrapper;
                cb(module_wrapper);
                return 0;
            });
        },
    });
}

function _create_bmark_obj() {
    return new base_ui.FC_Solve_Bookmarking({
        bookmark_controls: ["stdin", "deal_number"],
        show: [],
    });
}

function on_bookmarking() {
    return _create_bmark_obj().on_bookmarking();
}

function restore_bookmark() {
    return _create_bmark_obj().restore_bookmark();
}
// Taken from https://stackoverflow.com/questions/2901102/
// thanks.
function numberWithCommas(x: string): string {
    return x.replace(/\B(?=(\d{3})+(?!\d))/g, ",");
}

function getRounderNumber(str: string): string {
    return str.replace(/(000)1$/, "$10");
}

export function find_deal_ui(): void {
    const deal_str = ($("#stdin").val() as string)
        .replace(/#[^\r\n]*\r?\n?/g, "")
        .replace(/\r+\n/, "\n")
        .replace(/([^\n])$/, "$1\n");
    const ints = s2i.find_index__board_string_to_ints(deal_str);
    const ints_s = ints
        .map((i) => {
            const ret = i.toString();
            return " ".repeat(10 - ret.length) + ret;
        })
        .join("");
    const df = new w.Freecell_Deal_Finder({ module_wrapper: _module_wrapper });
    df.fill(ints_s);
    const ctl = $("#fc_solve_status");
    function _is_null(x: any): boolean {
        return typeof x === "undefined" || x === undefined || x === null;
    }
    const has_ls: boolean = !_is_null(localStorage);
    let chunk_s = null;
    if (has_ls) {
        const _key_name: string = "fc-solve-find-deal-chunk-size";
        const in_storage_s: string = localStorage.getItem(_key_name);

        if (!_is_null(in_storage_s)) {
            chunk_s = in_storage_s;
        }
    }
    const start = new Date().getTime();
    df.run(
        1,
        "8589934591",
        (args) => {
            ctl.html(
                base_ui.escapeHtml(
                    "Reached No. " +
                        numberWithCommas(
                            getRounderNumber(args.start.toString()),
                        ),
                ),
            );
            return;
        },
        chunk_s,
    );

    function resume() {
        const ret_Deal = df.cont();
        if (ret_Deal.found) {
            const elapsed = new Date().getTime() - start;
            const val_td = '<td class="val">';

            ctl.html(
                '<table class="found_result">' +
                    "<tr>" +
                    "<td>" +
                    "Found:" +
                    "</td>" +
                    val_td +
                    ' <input id="found_result" ' +
                    'name="found_result" value="' +
                    ret_Deal.result.toString() +
                    '" readonly="readonly" />' +
                    "<br/>" +
                    "</td>" +
                    "</tr>" +
                    "<tr>" +
                    "<td>" +
                    "<b>Elapsed:</b>" +
                    "</td>" +
                    val_td +
                    (elapsed / 1000).toString() +
                    " seconds" +
                    "</td>" +
                    "</tr>" +
                    "<tr>" +
                    "<td>" +
                    "<b>Iteration step size:</b>" +
                    "</td>" +
                    val_td +
                    df.getChunk().toString() +
                    "</td>" +
                    "</tr>" +
                    "</table>",
            );
        } else if (ret_Deal.cont) {
            setTimeout(() => {
                resume();
            }, 1);
        } else {
            ctl.html("No such deal");
        }
    }

    resume();

    return;
}

export let on__run_do_solve__click: () => void = null;
export let populate_input_pressed: () => void = null;
export function set_up_handlers(module_wrapper: w.ModuleWrapper): void {
    populate_input_pressed = () => {
        return base_ui.populate_input_with_numbered_deal(module_wrapper);
    };
    on__run_do_solve__click = find_deal_ui;
    base_ui.set_up__capitalize_cards();

    return;
}

export function set_up(module_wrapper: w.ModuleWrapper): void {
    restore_bookmark();
    set_up_handlers(module_wrapper);
    const fc_solve_bookmark_button = $("#fc_solve_bookmark_button");
    fc_solve_bookmark_button.off("click");
    fc_solve_bookmark_button.on("click", on_bookmarking);

    return;
}
