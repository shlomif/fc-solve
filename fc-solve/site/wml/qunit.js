#!/usr/bin/env node

"use strict";

const program = require( "commander" );
const run = require( "qunit/src/cli/run" );
const FindReporter = require( "qunit/src/cli/find-reporter" );
const pkg = require( "qunit/package.json" );

const findReporter = FindReporter.findReporter;
const displayAvailableReporters = FindReporter.displayAvailableReporters;

function collect( val, collection ) {
	collection.push( val );
	return collection;
}

program
	.version( pkg.version )
	.usage( "[options] [files]" )
	.option( "--require <module>", "specify a module to require prior to running " +
		"any tests.", collect, [] )
	.parse( process.argv );

if ( program.reporter === true ) {
	displayAvailableReporters();
}

const args = program.args;
const options = {
	filter: program.filter,
	reporter: findReporter( program.reporter ),
	requires: program.require,
	seed: program.seed
};

run( args, options );
