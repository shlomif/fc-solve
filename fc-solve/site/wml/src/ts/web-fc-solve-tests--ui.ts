import * as s2i from "./s2ints_js";
import * as test_strings from "./web-fcs-tests-strings";

const ms_deal_24 = `4C 2C 9C 8C QS 4S 2H
5H QH 3C AC 3H 4H QD
QC 9S 6H 9H 3S KS 3D
5D 2S JC 5C JH 6D AS
2D KD TH TC TD 8D
7H JS KH TS KC 7C
AH 5S 6S AD 8H JD
7S 6C 7D 4D 8S 9D
`;
const solution_for_deal_24__default = test_strings.dict["24_default_ascii"];
const solution_for_deal_24__default__with_unicoded_suits =
    test_strings.dict["24_default_unicode_suits"];
const solution_for_deal_24__default__with_unicoded_card_chars =
    test_strings.dict["24_default_unicode_cards"];

const pysol_simple_simon_deal_24 = `4C QH 3C 8C AD JH 8S KS
5H 9S 6H AC 4D TD 4S 6D
QC 2S JC 9H QS KC 4H 8D
5D KD TH 5C 3H 8H 7C
2D JS KH TC 3S JD
7H 5S 6S TS 9D
AH 6C 7D 2H
7S 9C QD
2C 3D
AS
`;

const solution_for_pysol_simple_simon_deal_24__default =
    test_strings.dict.solution_for_pysol_simple_simon_deal_24__default;
const board_without_trailing_newline__proto = `Freecells:  2S  KC  -  -
Foundations: H-A C-A D-0 S-0
8H 7C JH 2C 2H 4C
2D 9S 5S 4S 9C 8D KS
TC 9H 6C 8S KH 7H 4H
JS 3C TD 5C 6S 6H TS
JD AS QH 5H 3H KD
9D 7S AD 8C 3S JC
QC QD 6D 4D 3D
TH 7D QS 5D`;
const board_without_trailing_newline = String(
    board_without_trailing_newline__proto,
).replace(/\n+$/, "");

const solution_for_board_without_trailing_newline =
    test_strings.dict.solution_for_board_without_trailing_newline;
const solution_for_deal_24__expanded_moves =
    test_strings.dict.solution_for_deal_24__expanded_moves;

function my_func(qunit: QUnit, my_callback: () => void) {
    qunit.module("FC_Solve.WebUI", () => {
        qunit.test("populate_deal", (assert) => {
            assert.expect(4);

            let done = assert.async(1);

            $("#deal_number").val("624");
            $("#populate_input").click();
            window.setTimeout(() => {
                const board: string = $("#stdin").val() as string;
                // alert(board);
                // const board = $("#deal_number").text();
                // assert.equal(board, "KC", "foo");
                // TEST
                assert.ok(
                    /^(?:\: )?KC 6H 4C QS 2D 4S AS$/ms.test(board),
                    "got the text",
                );
                {
                    const sol: string = $("#output").val() as string;
                    // TEST
                    assert.deepEqual(sol, "", "solution is empty at start");
                }
                $("#run_do_solve").click();
                window.setTimeout(() => {
                    const sol: string = $("#output").val() as string;

                    // TEST
                    assert.ok(
                        /^Move a card from/ms.test(sol),
                        "solution was filled",
                    );
                    // TEST
                    assert.ok(sol.length > 300, "solution is long enough");
                    done();
                }, 1000);
            }, 1000);
        });
    });

    my_callback();

    return;
}

export function test_js_fc_solve_class(qunit: QUnit, my_callback: () => void) {
    my_func(qunit, my_callback);
    return;
}
