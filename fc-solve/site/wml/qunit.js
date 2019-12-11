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
    this.testCount = 0

    runner.on('runStart', this.onRunStart.bind(this))
    runner.on('testEnd', this.onTestEnd.bind(this))
    runner.on('runEnd', this.onRunEnd.bind(this))
  }

  static init (runner) {
    return new TapReporter(runner)
  }

  onRunStart (globalSuite) {
    console.log('TAP version 13')
  }

  onTestEnd (test) {
    this.testCount = this.testCount + 1

  }

  onRunEnd (globalSuite) {
  }

  logError (error, severity) {
  }
}
run( process.argv,  TapReporter);
