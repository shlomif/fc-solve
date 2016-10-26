var requirejs = require('requirejs');

requirejs.config({
    //Pass the top-level main.js/index.js require
    //function to requirejs so that node modules
    //are loaded relative to the top-level JS file.
    nodeRequire: require
});

var test_valid = require('web-fc-solve-tests--fcs-validate');
test_valid.test_fcs_validate();
