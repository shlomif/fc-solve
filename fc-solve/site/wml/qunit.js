#!/usr/bin/env node

"use strict";

const program = require( "commander" );
const path = require( "path" );
const resolve = require( "resolve" );

function run( args, reporter ) {

	// Default to non-zero exit code to avoid false positives
	process.exitCode = 1;

	global.QUnit = require("qunit");
	// TODO: Enable mode where QUnit is not auto-injected, but other setup is
	// still done automatically.

	reporter.init( global.QUnit );

    require( path.resolve( process.cwd(), args[2] ));

	global.QUnit.start();
}

const JSReporters = require( "js-reporters" );
run( process.argv,  JSReporters.TapReporter,);
