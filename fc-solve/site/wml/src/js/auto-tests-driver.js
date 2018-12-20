define(["web-fc-solve-tests", "web-fc-solve-tests--fcs-validate", "qunit"], function (t, fcs_valid, QUnit) {
    QUnit.config.autostart = false;
    return function() {
        const _log_cb = ((window.location.href.indexOf("alert") >= 0) ? alert : console.log);
        QUnit.done(function( details ) {
              //alert( "Total: "+ details.total+ " Failed: "+ details.failed+ " Passed: "+ details.passed+ " Runtime: "+ details.runtime );
              _log_cb( "Total: "+ details.total+ " Failed: "+ details.failed+ " Passed: "+ details.passed+ " Runtime: "+ details.runtime );
        });
        QUnit.moduleDone( ( { name, failed, total } ) => {
              //alert( `Finished running module : ${name} Failed/total: ${failed}, ${total}` );
              _log_cb( `Finished running module : ${name} Failed/total: ${failed}, ${total}` );
        });

        /*
        QUnit.module("BeforeMeta");
        QUnit.test("silence_qunit", function(assert) {
            assert.ok(true, "foo");
        });
        */
        var test_js_fc_solve_class = t.test_js_fc_solve_class;
        try {
            test_js_fc_solve_class(QUnit, function () {
                QUnit.start();
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
