/// <reference path="typings/index.d.ts" />
/// <reference path="fcs-validate.ts" />

function test_fcs_validate()
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
    QUnit.test("verify_state Card class tests", function(a: Assert) {
        a.expect(25);

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

            var col = result.col;

            // TEST
            a.equal(col.getLen(), 3, "col.getLen() is fine.");

            // TEST
            a.equal(col.getCard(0).toString(), '3D', "col.getCard(0) is fine.");
            // TEST
            a.equal(col.getCard(1).toString(), 'AH', "col.getCard(1) is fine.");
            // TEST
            a.equal(col.getCard(2).toString(), 'KH', "col.getCard(2) is fine.");
        }

        {
            var input_str = '3C AH 7D 6S  # This is a comment.';
            var result = fcs_js__column_from_string(0, input_str);

            // TEST
            a.ok (result.is_correct, "Column was parsed correctly.");

            // TEST
            a.equal (result.num_consumed_chars, input_str.length, 'Consumed input_str.length characters.');

            var col = result.col;

            // TEST
            a.equal(col.getLen(), 4, "col.getLen() is fine.");

            // TEST
            a.equal(col.getCard(0).toString(), '3C', "col.getCard(0) is fine.");
            // TEST
            a.equal(col.getCard(1).toString(), 'AH', "col.getCard(1) is fine.");
            // TEST
            a.equal(col.getCard(2).toString(), '7D', "col.getCard(2) is fine.");
            // TEST
            a.equal(col.getCard(3).toString(), '6S', "col.getCard(3) is fine.");
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
}
