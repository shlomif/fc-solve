import Module from "./libfcs-wrap";
import * as w from "./web-fc-solve";

const FC_Solve_init_wrappers_with_module = w.FC_Solve_init_wrappers_with_module;

export function test_js_fc_solve_class(qunit: QUnit, my_callback: () => void) {
    // var _my_mod = Module({});
    const _my_mod = [null];
    _my_mod[0] = Module()({
        onRuntimeInitialized: () => {
            FC_Solve_init_wrappers_with_module(_my_mod[0] || this);
        },
    });
    return;
}
