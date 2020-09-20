define(["tests/fcs-ui", "generic-tests-driver"], function (web_fc_solve_tests, tests_driver) {
    return tests_driver.tests_driver(web_fc_solve_tests, function(_q){return;});
});
