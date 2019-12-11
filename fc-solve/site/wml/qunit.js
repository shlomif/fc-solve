#!/usr/bin/env node

"use strict";

const program = require( "commander" );
const path = require( "path" );

const requireFromCWD = require( "qunit/src/cli/require-from-cwd" );
const requireQUnit = require( "qunit/src/cli/require-qunit" );

let QUnit;

function run( args, reporter ) {

	// Default to non-zero exit code to avoid false positives
	process.exitCode = 1;

	QUnit = requireQUnit();
	// TODO: Enable mode where QUnit is not auto-injected, but other setup is
	// still done automatically.
	global.QUnit = QUnit;

	reporter.init( QUnit );

    require( path.resolve( process.cwd(), args[2] ));

	QUnit.start();
}

const JSReporters = require( "js-reporters" );
run( process.argv,  JSReporters.TapReporter,);
