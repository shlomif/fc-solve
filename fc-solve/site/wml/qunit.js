#!/usr/bin/env node

"use strict";

const program = require( "commander" );
const run = require( "qunit/src/cli/run" );
const JSReporters = require( "js-reporters" );

const options = {
	reporter: JSReporters.TapReporter,
	requires: [],
};

run( process.argv, options );
