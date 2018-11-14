import * as s2i from "./dist/fc_solve_find_index_s2ints.js";
import * as Module from "./libfcs-wrap";
import * as w from "./web-fc-solve";
import * as test_strings from "./web-fcs-tests-strings";

const FC_Solve = w.FC_Solve;
const FC_Solve_init_wrappers_with_module = w.FC_Solve_init_wrappers_with_module;
const FCS_STATE_SUSPEND_PROCESS = w.FCS_STATE_SUSPEND_PROCESS;
const FCS_STATE_WAS_SOLVED = w.FCS_STATE_WAS_SOLVED;
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
const my_func = (QUnit, _my_mod, my_callback) => () => {
    FC_Solve_init_wrappers_with_module(_my_mod[0] || this);
    const deal_ms_fc_board = w.deal_ms_fc_board;

    QUnit.module("FC_Solve.Algorithmic", () => {
        function test_for_equal(
            assert,
            instance: w.FC_Solve,
            is_unicode_cards: boolean,
            is_unicode_cards_chars: boolean,
            board,
            expected_sol,
            blurb,
        ) {
            let success = false;

            let solve_err_code = instance.do_solve(board);

            while (solve_err_code === FCS_STATE_SUSPEND_PROCESS) {
                solve_err_code = instance.resume_solution();
            }

            if (solve_err_code === FCS_STATE_WAS_SOLVED) {
                const buffer = instance.display_solution({
                    displayer: new w.DisplayFilter({
                        is_unicode_cards,
                        is_unicode_cards_chars,
                    }),
                });
                success = true;
                assert.equal(buffer, expected_sol, blurb);
            }

            return success;
        }

        QUnit.test("FC_Solve main test", (assert) => {
            assert.expect(3);

            // TEST
            assert.ok(true, "True is, well, true.");

            const instance: w.FC_Solve = new FC_Solve({
                cmd_line_preset: "default",
                set_status_callback: () => {
                    return;
                },
            });

            // TEST*2
            assert.ok(
                test_for_equal(
                    assert,
                    instance,
                    false,
                    false,
                    ms_deal_24,
                    solution_for_deal_24__default,
                    "Solution is right.",
                ),
                "do_solve ASCII was successful.",
            );
        });

        QUnit.test("FC_Solve unicoded solution", (assert) => {
            assert.expect(2);

            const instance: w.FC_Solve = new FC_Solve({
                cmd_line_preset: "default",
                set_status_callback: () => {
                    return;
                },
            });

            // TEST*2
            assert.ok(
                test_for_equal(
                    assert,
                    instance,
                    true,
                    false,
                    ms_deal_24,
                    solution_for_deal_24__default__with_unicoded_suits,
                    "Unicoded solution was right",
                ),
                "do_solve unicoded was successful.",
            );
        });

        QUnit.test("FC_Solve unicode cards solution", (assert) => {
            assert.expect(2);

            const instance: w.FC_Solve = new FC_Solve({
                cmd_line_preset: "default",
                set_status_callback: () => {
                    return;
                },
            });

            // TEST*2
            assert.ok(
                test_for_equal(
                    assert,
                    instance,
                    true,
                    true,
                    ms_deal_24,
                    solution_for_deal_24__default__with_unicoded_card_chars,
                    "Unicoded cards chars solution was right",
                ),
                "do_solve Unicoded cards  chars was successful.",
            );
        });

        QUnit.test(
            "FC_Solve arbitrary parameters " + "- Solve Simple Simon.",
            (assert) => {
                assert.expect(2);

                const instance: w.FC_Solve = new FC_Solve({
                    cmd_line_preset: "default",
                    dir_base: "fcs1",
                    set_status_callback: () => {
                        return;
                    },
                    string_params: "-g simple_simon",
                });

                // TEST*2
                assert.ok(
                    test_for_equal(
                        assert,
                        instance,
                        false,
                        false,
                        pysol_simple_simon_deal_24,
                        solution_for_pysol_simple_simon_deal_24__default,
                        "Simple Simon Solution is right",
                    ),
                    "do_solve simpsim was successful.",
                );
            },
        );

        QUnit.test(
            "FC_Solve solve board without a trailing newline",
            (assert) => {
                assert.expect(3);

                // TEST
                assert.ok(true, "True is, well, true.");

                const instance: w.FC_Solve = new FC_Solve({
                    cmd_line_preset: "as",
                    set_status_callback: () => {
                        return;
                    },
                });

                // TEST*2
                assert.ok(
                    test_for_equal(
                        assert,
                        instance,
                        true,
                        false,
                        board_without_trailing_newline,
                        solution_for_board_without_trailing_newline,
                        "Board without a trailing newline solution is right.",
                    ),
                    "do_solve wo trailing newline was successful.",
                );
            },
        );

        QUnit.test("FC_Solve Expanded Moves test", (assert) => {
            assert.expect(4);

            // TEST
            assert.ok(true, "True is, well, true.");

            let success = false;

            const instance: w.FC_Solve = new FC_Solve({
                cmd_line_preset: "default",
                set_status_callback: () => {
                    return;
                },
            });

            if (true) {
                const ints = s2i.find_index__board_string_to_ints(ms_deal_24);
                const ints_s = ints
                    .map((i) => {
                        const ret = i.toString();
                        return " ".repeat(10 - ret.length) + ret;
                    })
                    .join("");
                const df = new w.Freecell_Deal_Finder({});
                df.fill(ints_s);
                df.run(1, 1000, (args) => {
                    return;
                });
                const ret_Deal = df.cont();
                // TEST
                assert.equal(ret_Deal.result, "24", "Freecell_Deal_Finder");
            } else {
                assert.ok(true, "skipped.");
            }

            let solve_err_code = instance.do_solve(ms_deal_24);

            while (solve_err_code === FCS_STATE_SUSPEND_PROCESS) {
                solve_err_code = instance.resume_solution();
            }

            const buffer = instance.display_expanded_moves_solution({
                displayer: new w.DisplayFilter({
                    is_unicode_cards: false,
                    is_unicode_cards_chars: false,
                }),
            });

            success = true;
            // TEST
            assert.equal(
                buffer,
                solution_for_deal_24__expanded_moves,
                "Expanded-moves Solution is right",
            );

            // TEST
            assert.ok(success, "do_solve expanded moves was successful.");
        });

        QUnit.test("FC_Solve get_num_stacks #1", (assert) => {
            assert.expect(1);

            const instance: w.FC_Solve = new FC_Solve({
                cmd_line_preset: "default",
                set_status_callback: () => {
                    return;
                },
            });

            // TEST
            assert.equal(
                instance.get_num_stacks(),
                8,
                "get_num_stacks() Returns 8.",
            );
        });

        QUnit.test("FC_Solve get_num_stacks simple_simon", (assert) => {
            assert.expect(1);

            const instance: w.FC_Solve = new FC_Solve({
                cmd_line_preset: "default",
                set_status_callback: () => {
                    return;
                },
                string_params: "-g simple_simon",
            });

            // TEST
            assert.equal(
                instance.get_num_stacks(),
                10,
                "get_num_stacks() Returns 10 for Simple Simon.",
            );
        });

        QUnit.test(
            "FC_Solve get_num_stacks command line settings",
            (assert) => {
                assert.expect(1);

                const instance: w.FC_Solve = new FC_Solve({
                    cmd_line_preset: "default",
                    dir_base: "fcs2",
                    set_status_callback: () => {
                        return;
                    },
                    string_params: "--stacks-num 6",
                });

                // TEST
                assert.equal(
                    instance.get_num_stacks(),
                    6,
                    "get_num_stacks() Returns 6 after command line.",
                );
            },
        );

        QUnit.test("FC_Solve get_num_freecells #1", (assert) => {
            assert.expect(1);

            const instance: w.FC_Solve = new FC_Solve({
                cmd_line_preset: "default",
                set_status_callback: () => {
                    return;
                },
            });

            // TEST
            assert.equal(
                instance.get_num_freecells(),
                4,
                "get_num_freecells() returns 4.",
            );
        });

        QUnit.test("FC_Solve get_num_freecells #1", (assert) => {
            assert.expect(1);

            const instance: w.FC_Solve = new FC_Solve({
                cmd_line_preset: "default",
                dir_base: "fcs3",
                set_status_callback: () => {
                    return;
                },
                string_params: "--freecells-num 2",
            });

            // TEST
            assert.equal(
                instance.get_num_freecells(),
                2,
                "get_num_freecells() returns 2 after command line.",
            );
        });

        QUnit.test("FC_Solve deal_ms_fc_board", (assert) => {
            assert.expect(2);

            // TEST
            assert.equal(
                deal_ms_fc_board(3000000000),
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
                deal_ms_fc_board(6000000000),
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

        QUnit.test("FC_Solve custom baker's game preset twice", (assert) => {
            // This is microsoft deal #10 which is
            // impossible to solve
            // in Baker's Game.
            const ms10_deal = `5S KD JC TS 9D KH 8D
        5H 2S 9H 7H TD AD 6D
        6H QD 6C TC AH 8S TH
        6S 2D 7C QC QS 7D 3H
        5D AS 7S KC 3D AC
        4D 9C QH 4H 4C 5C
        2H 3S 8H 9S JS 4S
        JH JD 3C KS 2C 8C
        `;
            assert.expect(2);

            for (let mytry = 1; mytry <= 2; ++mytry) {
                const instance: w.FC_Solve = new FC_Solve({
                    cmd_line_preset: "default",
                    set_status_callback: () => {
                        return;
                    },
                    string_params: "--game bakers_game -to 01ABCDE",
                });

                // TEST*2
                assert.ok(
                    !test_for_equal(
                        assert,
                        instance,
                        false,
                        false,
                        ms10_deal,
                        "",
                        "",
                    ),
                    "do_solve failed try=" + mytry,
                );
            }
        });
        my_callback();
    });

    return;
};

export function test_js_fc_solve_class(QUnit, my_callback) {
    // var _my_mod = Module({});
    const _my_mod = [null];
    _my_mod[0] = Module()({
        onRuntimeInitialized: my_func(QUnit, _my_mod, my_callback),
    });
    return;
}
