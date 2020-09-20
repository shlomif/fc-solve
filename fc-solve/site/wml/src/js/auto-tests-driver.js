define(["tests/fcs-core", "tests/fcs-validate", "generic-tests-driver"], function (web_fc_solve_tests, fcs_valid, tests_driver) {
    return tests_driver.tests_driver(web_fc_solve_tests, fcs_valid.test_fcs_validate);
});
