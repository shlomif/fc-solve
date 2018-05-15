define(["web-fc-solve-tests", "web-fc-solve-tests--fcs-validate", "qunit"], function (t, fcs_valid, QUnit) {
    return function() {
        var test_js_fc_solve_class = t.test_js_fc_solve_class;
        $(document).ready(function () {
            try {
                test_js_fc_solve_class(function () {
                    var was_called = false;
                    fcs_valid.test_fcs_validate();
                    was_called = true;
                    $('#qunit-tests').addClass('hidepass');
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
        });
    };
});
