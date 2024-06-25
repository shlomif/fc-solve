/*const s2i = {
    find_index__board_string_to_ints: (s) => {
        return [5, 6];
    },
};*/
// import * as s2i from "./dist/fc_solve_find_index_s2ints.js";
import Module from "../libfcs-wrap";
import { perl_range } from "../prange";
import { deal_ms_fc_board } from "../web-fcs-purejs-rand";

const ms_deal_24_with_colons = `: 4C 2C 9C 8C QS 4S 2H
: 5H QH 3C AC 3H 4H QD
: QC 9S 6H 9H 3S KS 3D
: 5D 2S JC 5C JH 6D AS
: 2D KD TH TC TD 8D
: 7H JS KH TS KC 7C
: AH 5S 6S AD 8H JD
: 7S 6C 7D 4D 8S 9D
`;

function my_func(qunit: QUnit, _my_mod, my_callback: () => void) {
    const _module = _my_mod[0] || this;
    const module_wrapper = null;

    qunit.module("Pure JavaScript Randomizer", () => {
        qunit.test("perl_range", (assert) => {
            assert.expect(7);

            function t(start, end, want, msg) {
                return assert.deepEqual(perl_range(start, end), want, msg);
            }
            // TEST*7
            t(0, 0, [0], "0->0");
            t(0, 1, [0, 1], "0->1");
            t(0, 2, [0, 1, 2], "0->2");
            t(1, 6, [1, 2, 3, 4, 5, 6], "0->2");
            t(-1, 1, [-1, 0, 1], "negative");
            t(-5, -3, [-5, -4, -3], "negative");
            t(4, 3, [], "reverse range");
        });

        qunit.test("FC_Solve deal_ms_fc_board", (assert) => {
            assert.expect(3);

            // TEST
            assert.equal(
                deal_ms_fc_board("24"),
                ms_deal_24_with_colons,
                "deal_ms_fc_board for 24 ( A small, non-overflowing deal-index)",
            );

            // TEST
            assert.equal(
                deal_ms_fc_board("3000000000"),
                `: 8D TS JS TD JH JD JC
: 4D QS TH AD 4S TC 3C
: 9H KH QH 4C 5C KD AS
: 9D 5D 8S 4H KS 6S 9S
: 6H 2S 7H 3D KC 2C
: 9C 7C QC 7S QD 7D
: 6C 3H 8H AC 6D 3S
: 8C AH 2H 5H 2D 5S
`,
                "deal_ms_fc_board for 3e9",
            );

            // TEST
            assert.equal(
                deal_ms_fc_board("6000000000"),
                `: 2D 2C QS 8D KD 8C 4C
: 3D AH 2H 4H TS 6H QD
: 4D JS AD 6S JH JC JD
: KH 3H KS AS TC 5D AC
: TD 7C 9C 7H 3C 3S
: QH 9H 9D 5S 7S 6C
: 5C 5H 2S KC 9S 4S
: 6D QC 8S TH 7D 8H
`,
                "deal_ms_fc_board for 6e9",
            );
        });
    });

    my_callback();

    return;
}

export function test_js_fc_solve_class(qunit: QUnit, my_callback: () => void) {
    const _my_mod = [null];
    _my_mod[0] = Module({
        onRuntimeInitialized: () => {
            _my_mod[0].then((result) => {
                _my_mod[0] = result;
                return my_func(qunit, _my_mod, my_callback);
            });
        },
    });
    return;
}
