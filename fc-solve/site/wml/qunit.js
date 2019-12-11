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
	.option( "--require <module>", "specify a module to require prior to running " +
		"any tests.", collect, [] )
	.parse( process.argv );

const args = program.args;
const options = {
	reporter: findReporter( program.reporter ),
	requires: program.require,
	seed: program.seed
};

run( args, options );
