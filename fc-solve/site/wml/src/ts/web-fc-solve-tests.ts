import Module from "./libfcs-wrap";
import * as w from "./web-fc-solve";
import { perl_range } from "./prange";

const FC_Solve_init_wrappers_with_module = w.FC_Solve_init_wrappers_with_module;

const my_func = (qunit: QUnit, _my_mod, my_callback: () => void) => () => {
    _my_mod[0].then((result) => {
        _my_mod[0] = result;
        // console.log(_my_mod[0]);
        FC_Solve_init_wrappers_with_module(_my_mod[0] || this);
        const deal_ms_fc_board = w.deal_ms_fc_board;

        qunit.module("FC_Solve.Algorithmic", () => {
            qunit.test("perl_range", (assert) => {
                assert.expect(1);

                function t(start, end, want, msg) {
                    return assert.deepEqual(perl_range(start, end), want, msg);
                }
                // TEST*1
                t(0, 1, [0, 1], "0->1");
            });
        });

        my_callback();

        return;
    });
};

export function test_js_fc_solve_class(qunit: QUnit, my_callback: () => void) {
    // var _my_mod = Module({});
    const _my_mod = [null];
    _my_mod[0] = Module()({
        onRuntimeInitialized: my_func(qunit, _my_mod, my_callback),
    });
    return;
}
