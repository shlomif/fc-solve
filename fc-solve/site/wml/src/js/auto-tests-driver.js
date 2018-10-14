define(["web-fc-solve-tests", "web-fc-solve-tests--fcs-validate", "qunit"], function (t, fcs_valid, QUnit) {
    return function() {
        QUnit.done(function( details ) {
              alert( "Total: "+ details.total+ " Failed: "+ details.failed+ " Passed: "+ details.passed+ " Runtime: "+ details.runtime );
        });
        QUnit.moduleDone( ( { name, failed, total } ) => {
              alert( `Finished running module : ${name} Failed/total: ${failed}, ${total}` );
        });

        QUnit.module("BeforeMeta");
        QUnit.test("silence_qunit", function(assert) {
            assert.ok(true, "foo");
        });
        var test_js_fc_solve_class = t.test_js_fc_solve_class;
        try {
            test_js_fc_solve_class(QUnit, function () {
                var was_called = false;
                fcs_valid.test_fcs_validate(QUnit);
                was_called = true;
                // $('#qunit-tests').addClass('hidepass');
                return;
            });
        } catch (err) {
            alert(err);
            QUnit.module("Meta");
            QUnit.test("global_failure", function(assert) {
                assert.ok (false, 'test_js_fc_solve_class() or test_fcs_validate() have thrown an exception or are invalid.');
            });
            throw err;
        }
    };
});
