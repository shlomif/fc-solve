import { jq_querystring } from "../jq_qs";
import * as test_strings from "../web-fcs-tests-strings";

import {
    get_flipped_deal_with_comment_prefix,
    get_flipped_deal_with_leading_empty_line,
    get_non_flipped_ms240_deal_deal,
} from "./fcs-common-constants";

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

function _get_solution(): string {
    return $("#output").val() as string;
}
class Ops {
    constructor(
        public assert,
        public done,
        public on_stdin_change,
        public on_output_change,
    ) {
        return;
    }
    public wrap_stdin() {
        const that = this;
        return that.on_stdin_change(that, that.assert, that.done);
    }
    public wrap_output() {
        const that = this;
        return that.on_output_change(that, that.assert, that.done);
    }
}
let count: number = 0;
function _get_stdin(): string {
    return $("#stdin").val() as string;
}
const on_flip_stdin = (my_operations: Ops, assert, done) => {
    assert.deepEqual(
        _get_stdin(),
        get_non_flipped_ms240_deal_deal(),
        "got the double-flipped text",
    );
    done();
    if (++count < 2) {
        $("#stdin").val(get_flipped_deal_with_leading_empty_line());
        $("#run_do_solve").trigger("click");
    }
};
const on_flip_output = (my_operations: Ops, assert, done) => {
    const board_parse__wrap = $("#board_parse__wrap");

    assert.deepEqual(
        board_parse__wrap.find("h2").text(),
        "Board Input Parsing Warnings:",
        "warnings",
    );
    my_operations.on_stdin_change = on_flip_stdin;
    if (count > 0) {
        assert.deepEqual(
            _get_stdin(),
            get_flipped_deal_with_leading_empty_line(),
            "got the single-flipped leading lins text",
        );
    }
    {
        const paras_in_spans = $("span p");
        // TEST*2
        assert.equal(
            paras_in_spans.length,
            0,
            "There are no paragraphs as children or descendents of <span/>s",
        );
    }
    const widget = $("#status_iters_count.status_info");
    // TEST*2
    assert.equal(widget.length, 1, "There is a status_iters_count widget");
    const input_widget = widget.find("input#status_iters_count_result");
    const iters_num_proto = input_widget.val();
    const iters_num: string = iters_num_proto + "";
    // The number of iterations of solving deal 240 (not "24"!).
    // Has a slight chance of changing in the future.
    const EXPECTED_NUM_TIMES__FOR_DEAL_240: number = 97;
    const EXPECTED_NUM_STATES: number = 160;
    // TEST*2
    assert.equal(
        iters_num,
        "" + EXPECTED_NUM_TIMES__FOR_DEAL_240 + "",
        "There is a valid iters_num",
    );
    const states_widget = $("#status_states_in_collection_count.status_info");
    // TEST*2
    assert.equal(
        states_widget.length,
        1,
        "There is a status_states_in_collection_count widget",
    );
    const states_input_widget = states_widget.find(
        "input#status_states_in_collection_count_result",
    );
    const states_in_collection_num_proto = states_input_widget.val();
    const states_in_collection_num: string =
        states_in_collection_num_proto + "";
    // TEST*2
    assert.equal(
        states_in_collection_num,
        "" + EXPECTED_NUM_STATES + "",
        "There is a valid states_in_collection_num",
    );
    $("#fcs_flip_deal").trigger("click");
    done();
};
const on_stdin_initial_layout = (my_operations: Ops, assert, done) => {
    // TEST
    assert.deepEqual(
        _get_stdin()
            .replace(/^#[^\n]*\n/gms, "")
            .replace(/^: /gms, ""),
        ms_deal_24,
        "got the initial layout text",
    );

    // TEST
    assert.deepEqual(_get_solution(), "", "solution is empty at start");
    $("#run_do_solve").trigger("click");
    done();
};
const on_change_initial_layout = (my_operations: Ops, assert, done) => {
    const sol: string = _get_solution();

    // TEST
    assert.deepEqual(sol, solution_for_deal_24__default, "solution was filled");
    my_operations.on_stdin_change = (a, b, c) => {};
    my_operations.on_output_change = on_flip_output;
    $("#stdin").val(get_flipped_deal_with_comment_prefix());
    $("#run_do_solve").trigger("click");

    done();
};
function my_func(qunit: QUnit, my_callback: () => void) {
    qunit.module("FC_Solve.WebUI", () => {
        qunit.test("populate_deal", (assert) => {
            assert.expect(19);

            const hash = {
                z: "26",
                a: "1",
                b: "2",
            };
            const got = jq_querystring(hash);
            assert.deepEqual(got, "a=1&b=2&z=26", "jq_querystring");

            let done = assert.async(7);
            const my_operations: Ops = new Ops(
                assert,
                done,
                on_stdin_initial_layout,
                on_change_initial_layout,
            );
            $("#stdin").change(() => {
                return my_operations.wrap_stdin();
            });
            $("#output").change(() => {
                return my_operations.wrap_output();
            });
            $("#deal_number").val("24");
            $("#preset").val("default");
            $("#one_based").prop("checked", false);
            $("#populate_input").trigger("click");
            done();
        });
    });

    my_callback();

    return;
}

export function test_js_fc_solve_class(qunit: QUnit, my_callback: () => void) {
    my_func(qunit, my_callback);
    return;
}
