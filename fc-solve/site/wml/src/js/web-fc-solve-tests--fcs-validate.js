function test_fcs_validate()
{
    QUnit.module("FC_Solve.Algorithmic");

    test("verify_state Card class tests", function() {
        expect(9);

        {
            var c = fcs_js__card_from_string('TH');

            // TEST
            equal(c.getRank(), 10, "Card(TH).getRank() is fine.");

            // TEST
            equal(c.getSuit(), 0, "Card(TH).getSuit() is fine.");

            // TEST
            equal(c.toString(), 'TH', "Card(TH).toString() works.");
        }

        {
            var c = fcs_js__card_from_string('AH');

            // TEST
            equal(c.getRank(), 1, "Card(AH).getRank() is fine.");

            // TEST
            equal(c.getSuit(), 0, "Card(AH).getSuit() is fine.");

            // TEST
            equal(c.toString(), 'AH', "Card(AH).toString() works.");
        }

        {
            var c = fcs_js__card_from_string('AC');

            // TEST
            equal(c.getRank(), 1, "Card(AC).getRank() is fine.");

            // TEST
            equal(c.getSuit(), 1, "Card(AC).getSuit() is fine.");

            // TEST
            equal(c.toString(), 'AC', "Card(AC).toString() works.");
        }
    });
    test("verify_state Card class tests", function() {
        expect(30);

        {
            var start_char_idx = 10;
            var col_str = 'KS QD';
            var result = fcs_js__column_from_string(start_char_idx, col_str);

            // TEST
            ok (result.is_correct, "Column was parsed correctly.");

            // TEST
            equal(result.start_char_idx, start_char_idx, "start_char_idx is correct.");

            // TEST
            equal(result.getEnd(), start_char_idx + col_str.length);

            var col = result.col;

            // TEST
            equal(col.getLen(), 2, "col.getLen() is fine.");

            // TEST
            equal(result.num_consumed_chars, 'KS QD'.length, "col.consumed is right on success.");

            // TEST
            equal(col.getCard(0).getRank(), 13, "col.getCard(0) is fine.");
            // TEST
            equal(col.getCard(0).getSuit(), 3, "col.getCard(0) is fine.");

            // TEST
            equal(col.getCard(1).getRank(), 12, "col.getCard(1) is fine.");
            // TEST
            equal(col.getCard(1).getSuit(), 2, "col.getCard(1) is fine.");
        }

        {
            var result = fcs_js__column_from_string(0, '3C AH 7D 6S');

            // TEST
            ok (result.is_correct, "Column was parsed correctly.");

            var col = result.col;

            // TEST
            equal(col.getLen(), 4, "col.getLen() is fine.");

            // TEST
            equal(col.getCard(0).toString(), '3C', "col.getCard(0) is fine.");
            // TEST
            equal(col.getCard(1).toString(), 'AH', "col.getCard(1) is fine.");
            // TEST
            equal(col.getCard(2).toString(), '7D', "col.getCard(2) is fine.");
            // TEST
            equal(col.getCard(3).toString(), '6S', "col.getCard(3) is fine.");
        }

        {
            var result = fcs_js__column_from_string(0, '3C HA');

            // TEST
            ok ((! result.is_correct), "Column is incorrectly formatted.");

            // TEST
            equal (result.num_consumed_chars, 3, 'Consumed 3 characters.');

            // TEST
            equal (result.error, 'Wrong card format - should be [Rank][Suit]', 'error is correct');
        }

        {
            var result = fcs_js__column_from_string(0, ': 3D AH KH');

            // TEST
            ok (result.is_correct, "Column was parsed correctly.");

            var col = result.col;

            // TEST
            equal(col.getLen(), 3, "col.getLen() is fine.");

            // TEST
            equal(col.getCard(0).toString(), '3D', "col.getCard(0) is fine.");
            // TEST
            equal(col.getCard(1).toString(), 'AH', "col.getCard(1) is fine.");
            // TEST
            equal(col.getCard(2).toString(), 'KH', "col.getCard(2) is fine.");
        }

        {
            var input_str = '3C AH 7D 6S  # This is a comment.';
            var result = fcs_js__column_from_string(0, input_str);

            // TEST
            ok (result.is_correct, "Column was parsed correctly.");

            // TEST
            equal (result.num_consumed_chars, input_str.length, 'Consumed input_str.length characters.');

            var col = result.col;

            // TEST
            equal(col.getLen(), 4, "col.getLen() is fine.");

            // TEST
            equal(col.getCard(0).toString(), '3C', "col.getCard(0) is fine.");
            // TEST
            equal(col.getCard(1).toString(), 'AH', "col.getCard(1) is fine.");
            // TEST
            equal(col.getCard(2).toString(), '7D', "col.getCard(2) is fine.");
            // TEST
            equal(col.getCard(3).toString(), '6S', "col.getCard(3) is fine.");
        }
    });
}
