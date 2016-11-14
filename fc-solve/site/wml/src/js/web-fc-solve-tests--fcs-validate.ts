import { fcs_js__freecells_from_string, fcs_js__column_from_string, fcs_js__card_from_string, Foundations } from "./fcs-validate";

export function test_fcs_validate()
{
    QUnit.module("FC_Solve.Algorithmic");

    QUnit.test("verify_state Card class tests", function(a : Assert) {
        a.expect(9);

        {
            var c = fcs_js__card_from_string('TH');

            // TEST
            a.equal(c.getRank(), 10, "Card(TH).getRank() is fine.");

            // TEST
            a.equal(c.getSuit(), 0, "Card(TH).getSuit() is fine.");

            // TEST
            a.equal(c.toString(), 'TH', "Card(TH).toString() works.");
        }

        {
            var c = fcs_js__card_from_string('AH');

            // TEST
            a.equal(c.getRank(), 1, "Card(AH).getRank() is fine.");

            // TEST
            a.equal(c.getSuit(), 0, "Card(AH).getSuit() is fine.");

            // TEST
            a.equal(c.toString(), 'AH', "Card(AH).toString() works.");
        }

        {
            var c = fcs_js__card_from_string('AC');

            // TEST
            a.equal(c.getRank(), 1, "Card(AC).getRank() is fine.");

            // TEST
            a.equal(c.getSuit(), 1, "Card(AC).getSuit() is fine.");

            // TEST
            a.equal(c.toString(), 'AC', "Card(AC).toString() works.");
        }
    });
    QUnit.test("verify_state Column class tests", function(a: Assert) {
        a.expect(18);

        {
            var start_char_idx = 10;
            var col_str = 'KS QD';
            var result = fcs_js__column_from_string(start_char_idx, col_str);

            // TEST
            a.ok (result.is_correct, "Column was parsed correctly.");

            // TEST
            a.equal(result.start_char_idx, start_char_idx, "start_char_idx is correct.");

            // TEST
            a.equal(result.getEnd(), start_char_idx + col_str.length);

            var col = result.col;

            // TEST
            a.deepEqual(col.getArrOfStrs(), ['KS', 'QD'],
                        "col contents is fine.");

            // TEST
            a.equal(result.num_consumed_chars, 'KS QD'.length, "col.consumed is right on success.");
        }

        {
            var result = fcs_js__column_from_string(0, '3C AH 7D 6S');

            // TEST
            a.ok (result.is_correct, "Column was parsed correctly.");

            var col = result.col;

            // TEST
            a.deepEqual(col.getArrOfStrs(), ['3C', 'AH', '7D', '6S'], "col contents are fine.");
        }

        {
            var result = fcs_js__column_from_string(0, '3C HA');

            // TEST
            a.ok ((! result.is_correct), "Column is incorrectly formatted.");

            // TEST
            a.equal (result.num_consumed_chars, 3, 'Consumed 3 characters.');

            // TEST
            a.equal (result.error, 'Wrong card format - should be [Rank][Suit]', 'error is correct');
        }

        {
            var result = fcs_js__column_from_string(0, ': 3D AH KH');

            // TEST
            a.ok (result.is_correct, "Column was parsed correctly.");

            // TEST
            a.deepEqual(result.col.getArrOfStrs(), ['3D', 'AH', 'KH'], "col with leading colon contents");
        }

        {
            var input_str = '3C AH 7D 6S  # This is a comment.';
            var result = fcs_js__column_from_string(0, input_str);

            // TEST
            a.ok (result.is_correct, "Column was parsed correctly.");

            // TEST
            a.equal (result.num_consumed_chars, input_str.length, 'Consumed input_str.length characters.');

            // TEST
            a.deepEqual(result.col.getArrOfStrs(), ['3C', 'AH', '7D', '6S'],
                        "col contents w comment is fine.");
        }

        {
            var input_str = "3S AD 7D 6S  # This is a comment.\n";
            var result = fcs_js__column_from_string(0, input_str);

            // TEST
            a.ok (result.is_correct, "Newline terminated Column was parsed correctly.");

            // TEST
            a.equal (result.num_consumed_chars, input_str.length, 'Newline terminated - Consumed input_str.length characters.');

            var col = result.col;

            // TEST
            a.deepEqual(col.getArrOfStrs(),
                ['3S', 'AD', '7D', '6S'],
                "Newline terminated - col contents are fine."
            );
        }
    });
    QUnit.test("verify_state Freecells class tests", function(a: Assert) {
        a.expect(45);

        {
            var start_char_idx = 10;
            var str = "Freecells: 5C 2H 3D 9H";
            var num_freecells = 4;

            var result = fcs_js__freecells_from_string(num_freecells, start_char_idx, str);

            // TEST
            a.ok (result.is_correct, "Column was parsed correctly.");

            // TEST
            a.equal(result.start_char_idx, start_char_idx, "start_char_idx is correct.");

            // TEST
            a.equal(result.getEnd(), start_char_idx + str.length);

            // TEST
            a.deepEqual(result.freecells.getArrOfStrs(), ['5C', '2H', '3D', '9H'],
                        "freecell contents is fine.");

            // TEST
            a.equal(result.num_consumed_chars, str.length, "fc.consumed is right on success.");
        }

        {
            var start_char_idx = 10;
            var str = "Freecells: 5C 2H";
            var num_freecells = 4;

            var result = fcs_js__freecells_from_string(num_freecells, start_char_idx, str);

            // TEST
            a.ok (result.is_correct, "Column was parsed correctly.");

            // TEST
            a.equal(result.start_char_idx, start_char_idx, "start_char_idx is correct.");

            // TEST
            a.equal(result.getEnd(), start_char_idx + str.length);

            // TEST
            a.deepEqual(result.freecells.getArrOfStrs(), ['5C', '2H', '-', '-'],
                        "freecell contents is fine.");

            // TEST
            a.equal(result.num_consumed_chars, str.length, "fc.consumed is right on success.");
        }

        {
            var start_char_idx = 39;
            var str = "Freecells: - TC - 9H";
            var num_freecells = 4;

            var result = fcs_js__freecells_from_string(num_freecells, start_char_idx, str);

            // TEST
            a.ok (result.is_correct, "Column was parsed correctly.");

            // TEST
            a.equal(result.start_char_idx, start_char_idx, "start_char_idx is correct.");

            // TEST
            a.equal(result.getEnd(), start_char_idx + str.length);

            // TEST
            a.deepEqual(result.freecells.getArrOfStrs(), ['-', 'TC', '-', '9H'],
                        "freecell contents is fine.");

            // TEST
            a.equal(result.num_consumed_chars, str.length, "fc.consumed is right on success.");
        }

        {
            const start_char_idx = 39;
            const str = "Freecells: - - 6D 9H";
            const num_freecells = 6;

            var result = fcs_js__freecells_from_string(num_freecells, start_char_idx, str);

            // TEST
            a.ok (result.is_correct, "Column was parsed correctly.");

            // TEST
            a.equal(result.start_char_idx, start_char_idx, "start_char_idx is correct.");

            // TEST
            a.equal(result.getEnd(), start_char_idx + str.length);

            // TEST
            a.deepEqual(result.freecells.getArrOfStrs(), ['-', '-', '6D', '9H', '-', '-'],
                        "freecell contents is fine.");

            // TEST
            a.equal(result.num_consumed_chars, str.length, "fc.consumed is right on success.");
        }

        {
            const start_char_idx = 39;
            const str = "Freecells: - - 6D 9H -";
            const num_freecells = 6;

            var result = fcs_js__freecells_from_string(num_freecells, start_char_idx, str);

            // TEST
            a.ok (result.is_correct, "Trailing Empty FC - Column was parsed correctly.");

            // TEST
            a.equal(result.start_char_idx, start_char_idx, "Trailing Empty FC -  start_char_idx is correct.");

            // TEST
            a.equal(result.getEnd(), start_char_idx + str.length);

            // TEST
            a.deepEqual(result.freecells.getArrOfStrs(), ['-', '-', '6D', '9H', '-', '-'],
                        "Trailing Empty FC - freecell contents is fine.");

            // TEST
            a.equal(result.num_consumed_chars, str.length, "Trailing Empty FC - fc.consumed is right on success.");
        }

        {
            const start_char_idx = 200;
            const str = "Freecells: - JC  - 9H\n";
            const num_freecells = 4;
            const test_name = "With trailing newline. ";

            var result = fcs_js__freecells_from_string(num_freecells, start_char_idx, str);

            // TEST
            a.ok (result.is_correct, test_name + "Column was parsed correctly.");

            // TEST
            a.equal(result.start_char_idx, start_char_idx, test_name + "start_char_idx is correct.");

            // TEST
            a.equal(result.getEnd(), start_char_idx + str.length);

            // TEST
            a.deepEqual(result.freecells.getArrOfStrs(), ['-', 'JC', '-', '9H'],
                        test_name + "freecell contents is fine.");

            // TEST
            a.equal(result.num_consumed_chars, str.length, test_name + "fc.consumed is right on success.");
        }

        {
            const start_char_idx = 200;
            const str = "Freecells: - JC  - 9H  # A comment";
            const num_freecells = 4;
            const test_name = "With comment. ";

            var result = fcs_js__freecells_from_string(num_freecells, start_char_idx, str);

            // TEST
            a.ok (result.is_correct, test_name + "Column was parsed correctly.");

            // TEST
            a.equal(result.start_char_idx, start_char_idx, test_name + "start_char_idx is correct.");

            // TEST
            a.equal(result.getEnd(), start_char_idx + str.length);

            // TEST
            a.deepEqual(result.freecells.getArrOfStrs(), ['-', 'JC', '-', '9H'],
                        test_name + "freecell contents is fine.");

            // TEST
            a.equal(result.num_consumed_chars, str.length, test_name + "fc.consumed is right on success.");
        }

        {
            const start_char_idx = 200;
            const str = "Freecells: - JC  - 9H  # A comment\n";
            const num_freecells = 5;
            const test_name = "With a comment and a newline - ";

            var result = fcs_js__freecells_from_string(num_freecells, start_char_idx, str);

            // TEST
            a.ok (result.is_correct, test_name + "Column was parsed correctly.");

            // TEST
            a.equal(result.start_char_idx, start_char_idx, test_name + "start_char_idx is correct.");

            // TEST
            a.equal(result.getEnd(), start_char_idx + str.length);

            // TEST
            a.deepEqual(result.freecells.getArrOfStrs(), ['-', 'JC', '-', '9H', '-'],
                        test_name + "freecell contents is fine.");

            // TEST
            a.equal(result.num_consumed_chars, str.length, test_name + "fc.consumed is right on success.");
        }

        {
            const start_char_idx = 10;
            const str = "F-Junk: 5C 2H 3D 9H";
            const num_freecells = 4;

            var result = fcs_js__freecells_from_string(num_freecells, start_char_idx, str);

            // TEST
            a.notOk (result.is_correct, "Freecells has wrong prefix.");

            // TEST
            a.equal(result.start_char_idx, start_char_idx, "start_char_idx is correct.");

            // TEST
            a.ok (result.error.match(/^Wrong line prefix/), "err-str");
        }

        {
            var f = new Foundations();

            // TEST
            a.equal(f.getByIdx(0,0), (-1), "founds.getByIdx works.");

            f.setByIdx(0,0,0);
            // TEST
            a.equal(f.getByIdx(0,0), 0, "founds.getByIdx works after assignment.");

        }
    });
}
