#!/usr/bin/env node

"use strict";

const program = require( "commander" );
const run = require( "qunit/src/cli/run" );
const FindReporter = require( "qunit/src/cli/find-reporter" );
const pkg = require( "qunit/package.json" );

const findReporter = FindReporter.findReporter;
const displayAvailableReporters = FindReporter.displayAvailableReporters;

const description = `Runs tests using the QUnit framework.

  Files should be a space-separated list of file/directory paths and/or glob
  expressions. Defaults to 'test/**/*.js'.

  For more info on working with QUnit, check out http://qunitjs.com.`;

function collect( val, collection ) {
	collection.push( val );
	return collection;
}

program._name = "qunit";
program
	.version( pkg.version )
	.usage( "[options] [files]" )
	.description( description )
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

if ( program.watch ) {
	run.watch( args, options );
} else {
	run( args, options );
}