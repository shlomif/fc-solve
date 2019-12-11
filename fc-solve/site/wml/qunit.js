#!/usr/bin/env node

"use strict";

const program = require( "commander" );
const run = require( "qunit/src/cli/run" );
const JSReporters = require( "js-reporters" );
const utils = require( "qunit/src/cli/utils" );

function findReporter( reporterName ) {
	if ( !reporterName ) {
		return JSReporters.TapReporter;
	}

	// First, check if the reporter is one of the standard js-reporters ones

	const capitalizedName = utils.capitalize( reporterName );
	const jsReporterName = `${capitalizedName}Reporter`;

	if ( JSReporters[ jsReporterName ] ) {
		return JSReporters[ jsReporterName ];
	}

	// Second, check if the reporter is an npm package
	try {
		return require( reporterName );
	} catch ( e ) {
		if ( e.code !== "MODULE_NOT_FOUND" ) {
			throw e;
		}
	}

	// If we didn't find a reporter, display the available reporters and exit
	displayAvailableReporters( reporterName );
}

function displayAvailableReporters( inputReporterName ) {
	const message = [];

	if ( inputReporterName ) {
		message.push( `No reporter found matching "${inputReporterName}".` );
	}

	const jsReporters = getReportersFromJSReporters();

	message.push( `Available reporters from JS Reporters are: ${jsReporters.join( ", " )}` );

	utils.error( message.join( "\n" ) );
}

const options = {
	reporter: findReporter( program.reporter ),
	requires: [],
};

run( process.argv, options );
