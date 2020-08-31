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

function my_func(qunit: QUnit, my_callback: () => void) {
    qunit.module("FC_Solve.WebUI", () => {
        qunit.test("populate_deal", (assert) => {
            assert.expect(2);

            let done = assert.async(2);
            (() => {
                function _get_solution(): string {
                    return $("#output").val() as string;
                }

                if (false) {
                    $("#output").change(() => {
                        const sol: string = _get_solution();

                        // TEST
                        assert.deepEqual(
                            sol,
                            solution_for_deal_24__default,
                            "solution was filled",
                        );
                        done();
                    });
                }
                $("#stdin").change(() => {
                    const board: string = $("#stdin").val() as string;
                    // alert(board);
                    // const board = $("#deal_number").text();
                    // assert.equal(board, "KC", "foo");
                    // TEST
                    assert.deepEqual(
                        board
                            .replace(/^#[^\n]*\n/gms, "")
                            .replace(/^: /gms, ""),
                        ms_deal_24,
                        "got the initial layout text",
                    );

                    // TEST
                    assert.deepEqual(
                        _get_solution(),
                        "",
                        "solution is empty at start",
                    );
                    // $("#run_do_solve").click();
                    done();
                });
                $("#deal_number").val("24");
                $("#preset").val("default");
                $("#one_based").prop("checked", false);
                $("#populate_input").click();
                done();
            })();
        });
    });

    my_callback();

    return;
}

export function test_js_fc_solve_class(qunit: QUnit, my_callback: () => void) {
    my_func(qunit, my_callback);
    return;
}
