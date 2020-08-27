import * as test_strings from "./web-fcs-tests-strings";

const solution_for_deal_24__default__with_unicoded_suits =
    test_strings.dict["24_default_unicode_suits"];

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

                function _get_solution(): string {
                    return $("#output").val() as string;
                }
                // TEST
                assert.deepEqual(
                    _get_solution(),
                    "",
                    "solution is empty at start",
                );
                $("#run_do_solve").click();
                window.setTimeout(() => {
                    const sol: string = _get_solution();

                    // TEST
                    assert.ok(
                        /^Move a card from/ms.test(sol),
                        "solution was filled",
                    );
                    // TEST
                    assert.ok(sol.length > 300, "solution is long enough");
                    done();
                }, 200);
            }, 200);
        });
    });

    my_callback();

    return;
}

export function test_js_fc_solve_class(qunit: QUnit, my_callback: () => void) {
    my_func(qunit, my_callback);
    return;
}
