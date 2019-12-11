#!/usr/bin/env node

"use strict";

const program = require( "commander" );
const run = require( "qunit/src/cli/run" );
const FindReporter = require( "qunit/src/cli/find-reporter" );

const options = {
	reporter: FindReporter.findReporter( program.reporter ),
	requires: [],
};

run( process.argv, options );
