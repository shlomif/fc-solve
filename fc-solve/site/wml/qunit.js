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
    console.log(`1..${globalSuite.testCounts.total}`)
    console.log(`# pass ${globalSuite.testCounts.passed}`)
    console.log((`# skip ${globalSuite.testCounts.skipped}`))
    console.log((`# todo ${globalSuite.testCounts.todo}`))
    console.log((`# fail ${globalSuite.testCounts.failed}`))
  }

  logError (error, severity) {
    console.log('  ---')
    console.log(`  message: "${error.message || 'failed'}"`)
    console.log(`  severity: ${severity || 'failed'}`)

    if (error.hasOwnProperty('actual')) {
      var actualStr = error.actual !== undefined ? JSON.stringify(error.actual, null, 2) : 'undefined'
      console.log(`  actual: ${actualStr}`)
    }

    if (error.hasOwnProperty('expected')) {
      var expectedStr = error.expected !== undefined ? JSON.stringify(error.expected, null, 2) : 'undefined'
      console.log(`  expected: ${expectedStr}`)
    }

    if (error.stack) {
      console.log(`  stack: ${error.stack}`)
    }

    console.log('  ...')
  }
}
run( process.argv,  TapReporter);
