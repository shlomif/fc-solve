#!/usr/bin/env node

"use strict";

const program = require( "commander" );
const path = require( "path" );

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

class TapReporter {
  constructor (runner) {
  }

  static init (runner) {
    return new TapReporter(runner)
  }

  onRunStart (globalSuite) {
  }

  onTestEnd (test) {
  }

  onRunEnd (globalSuite) {
  }

  logError (error, severity) {
  }
}
run( process.argv,  TapReporter);
