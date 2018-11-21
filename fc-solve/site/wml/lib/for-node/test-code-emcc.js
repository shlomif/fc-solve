if (process.env.SKIP_EMCC != '1') {
    const test_code = require('web-fc-solve-tests');
    test_code.test_js_fc_solve_class(QUnit, function() {
        return;
    });
} else {
    QUnit.module("SKIP_EMCC");
    QUnit.test("dummy", (a) => {
        a.expect(1);

        a.ok(true, 'skipped');
    });
    QUnit.skip("SKIP_EMCC was set so skip emcc tests", (a) => {
        a.expect(1);

        a.ok(true, 'skipped');
    });
}
