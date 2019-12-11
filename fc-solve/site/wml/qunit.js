#!/usr/bin/env node

"use strict";

const program = require( "commander" );
const path = require( "path" );

const requireFromCWD = require( "qunit/src/cli/require-from-cwd" );
const requireQUnit = require( "qunit/src/cli/require-qunit" );
const utils = require( "qunit/src/cli/utils" );

let QUnit;

function run( args, options ) {

	// Default to non-zero exit code to avoid false positives
	process.exitCode = 1;

	const files = utils.getFilesFromArgs( args );

	QUnit = requireQUnit();
	// TODO: Enable mode where QUnit is not auto-injected, but other setup is
	// still done automatically.
	global.QUnit = QUnit;

	options.requires.forEach( requireFromCWD );

	options.reporter.init( QUnit );

	for ( let i = 0; i < files.length; i++ ) {
		require( path.resolve( process.cwd(), files[ i ] ));
	}

	let running = true;

	process.on( "exit", function() {
		if ( running ) {
			console.error( "Error: Process exited before tests finished running" );

			const currentTest = QUnit.config.current;
			if ( currentTest && currentTest.semaphore ) {
				const name = currentTest.testName;
				console.error( "Last test to run (" + name + ") has an async hold. " +
					"Ensure all assert.async() callbacks are invoked and Promises resolve. " +
					"You should also set a standard timeout via QUnit.config.testTimeout." );
			}
		}
	} );

	QUnit.start();

}

const JSReporters = require( "js-reporters" );

const options = {
	reporter: JSReporters.TapReporter,
	requires: [],
};

run( process.argv, options );
