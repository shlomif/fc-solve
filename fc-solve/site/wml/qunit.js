#!/usr/bin/env node

"use strict";

const program = require( "commander" );
const path = require( "path" );
const resolve = require( "resolve" );

function requireQUnit() {
	try {

		// First we attempt to find QUnit relative to the current working directory.
		const localQUnitPath = resolve.sync( "qunit", { basedir: process.cwd() } );
		delete require.cache[ localQUnitPath ];
		return require( localQUnitPath );
	} catch ( e ) {
		try {

			// Second, we use the globally installed QUnit
			delete require.cache[ resolve.sync( "../../qunit/qunit" ) ];
			// eslint-disable-next-line node/no-missing-require, node/no-unpublished-require
			return require( "../../qunit/qunit" );
		} catch ( e ) {
			if ( e.code === "MODULE_NOT_FOUND" ) {

				// Finally, we use the local development version of QUnit
				delete require.cache[ resolve.sync( "../../dist/qunit" ) ];
				// eslint-disable-next-line node/no-missing-require, node/no-unpublished-require
				return require( "../../dist/qunit" );
			}

			throw e;
		}
	}
};

function run( args, reporter ) {

	// Default to non-zero exit code to avoid false positives
	process.exitCode = 1;

	global.QUnit = requireQUnit();
	// TODO: Enable mode where QUnit is not auto-injected, but other setup is
	// still done automatically.

	reporter.init( global.QUnit );

    require( path.resolve( process.cwd(), args[2] ));

	global.QUnit.start();
}

const JSReporters = require( "js-reporters" );
run( process.argv,  JSReporters.TapReporter,);
