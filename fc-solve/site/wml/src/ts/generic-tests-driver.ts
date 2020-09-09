export function tests_driver(web_fc_solve_tests, extra_test) {
    QUnit.config.autostart = false;
    function run_tests() {
        const _log_cb =
            window.location.href.indexOf("alert") >= 0 ? alert : console.log;
        if (false) {
            QUnit.done(function (details) {
                //alert( "Total: "+ details.total+ " Failed: "+ details.failed+ " Passed: "+ details.passed+ " Runtime: "+ details.runtime );
                _log_cb(
                    "Total: " +
                        details.total +
                        " Failed: " +
                        details.failed +
                        " Passed: " +
                        details.passed +
                        " Runtime: " +
                        details.runtime,
                );
            });
            QUnit.moduleDone(({ name, failed, total }) => {
                //alert( `Finished running module : ${name} Failed/total: ${failed}, ${total}` );
                _log_cb(
                    `Finished running module : ${name} Failed/total: ${failed}, ${total}`,
                );
            });
        }

        const test_js_fc_solve_class =
            web_fc_solve_tests.test_js_fc_solve_class;
        let was_called = false;
        try {
            test_js_fc_solve_class(QUnit, function () {
                QUnit.start();
                extra_test(QUnit);
                was_called = true;
                return;
            });
        } catch (err) {
            alert(err);
            QUnit.module("Meta");
            QUnit.test("global_failure", function (assert) {
                assert.ok(
                    false,
                    "test_js_fc_solve_class() or test_fcs_validate() have thrown an exception or are invalid.",
                );
            });
            throw err;
        }
    }
    const delay_len = 500;
    function run_after_delay(cb) {
        setTimeout(cb, delay_len);
        return;
    }
    return { run_tests, run_after_delay };
}
