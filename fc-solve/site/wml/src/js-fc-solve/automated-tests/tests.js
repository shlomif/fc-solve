require(["web-fc-solve-tests"], function (t) {
    var test_js_fc_solve_class = t.test_js_fc_solve_class;
    $(document).ready(function () {
        try {
            test_js_fc_solve_class(function () {
                var was_called = false;
                require(["web-fc-solve-tests--fcs-validate"], function (fcs_valid) {
                    fcs_valid.test_fcs_validate();
                    was_called = true;
                    $('#qunit-tests').addClass('hidepass');
                    return;
                });
                return;
            });
        }
        catch (err) {
            throw err;
            alert(err);
            QUnit.module("Meta");
            QUnit.test("global_failure", function(assert) {
                assert.ok (false, 'test_js_fc_solve_class() or test_fcs_validate() have thrown an exception or are invalid.');
            });
        }
    });
});
