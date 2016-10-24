var requirejs = require('requirejs');

requirejs.config({
    //Pass the top-level main.js/index.js require
    //function to requirejs so that node modules
    //are loaded relative to the top-level JS file.
    nodeRequire: require
});

if (false) {
requirejs(['fcs-validate', ],
    function   (fcs_valid) {
        requirejs(['web-fc-solve-tests--fcs-validate'],
            function (test_valid) {
                test_valid.test_fcs_validate();
                return;
            });
        return;
    });

}
var test_valid = require('./web-fc-solve-tests--fcs-validate');
test_valid.test_fcs_validate();
