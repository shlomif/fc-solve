#!/usr/bin/env node

"use strict";

const program = require( "commander" );
const path = require( "path" );

const requireFromCWD = require( "qunit/src/cli/require-from-cwd" );
const requireQUnit = require( "qunit/src/cli/require-qunit" );
const utils = require( "qunit/src/cli/utils" );

const IGNORED_GLOBS = [
	".git",
	"node_modules"
].concat( utils.getIgnoreList( process.cwd() ) );

const RESTART_DEBOUNCE_LENGTH = 200;

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

	QUnit.on( "runEnd", function setExitCode( data ) {
		running = false;

		if ( data.testCounts.failed ) {
			process.exitCode = 1;
		} else {
			process.exitCode = 0;
		}
	} );
}

run.restart = function( args ) {
	clearTimeout( this._restartDebounceTimer );

	this._restartDebounceTimer = setTimeout( () => {

		const watchedFiles = utils.findFiles( process.cwd(), {
			match: [ "**/*.js" ],
			ignore: IGNORED_GLOBS
		} );

		watchedFiles.forEach( file => delete require.cache[ path.resolve( file ) ] );

		if ( QUnit.config.queue.length ) {
			console.log( "Finishing current test and restarting..." );
		} else {
			console.log( "Restarting..." );
		}

		run.abort( () => run.apply( null, args ) );
	}, RESTART_DEBOUNCE_LENGTH );
};

const JSReporters = require( "js-reporters" );

const options = {
	reporter: JSReporters.TapReporter,
	requires: [],
};

run( process.argv, options );
