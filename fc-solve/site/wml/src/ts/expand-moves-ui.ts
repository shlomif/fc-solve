import * as s2i from "./s2ints_js";
import * as BaseApi from "./web-fcs-api-base";
import * as base_ui from "./fcs-base-ui";
import * as expander from "./web-fc-solve--expand-moves";
import Module from "./libfcs-wrap";
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

export function expand_ui(): void {
    const input_str = ($("#stdin").val() as string)
        .replace(/#[^\r\n]*\r?\n?/g, "")
        .replace(/\r+\n/, "\n")
        .replace(/([^\n])$/, "$1\n");
    function _calc(defval: number, id: string): number {
        const option_value = $("#" + id).val() as string;
        if (option_value === "default") {
            return defval;
        }
        if (option_value.match(/[^0-9]/)) {
            throw "foo";
        }
        return parseInt(option_value, 10);
    }
    const num_columns = _calc(8, "num_columns");
    const num_freecells = _calc(4, "num_freecells");

    const ret_str = expander.fc_solve_expand_moves_filter_solution_text(
        num_columns,
        num_freecells,
        input_str,
    );
    $("#output").val(ret_str);

    return;
}

export let on__expand__click: () => void = null;
export let populate_input_pressed: () => void = null;
export function set_up_handlers(module_wrapper: w.ModuleWrapper): void {
    populate_input_pressed = () => {
        return base_ui.populate_input_with_numbered_deal(module_wrapper, w);
    };
    on__expand__click = expand_ui;
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
