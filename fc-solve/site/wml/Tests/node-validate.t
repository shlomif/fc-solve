#!/usr/bin/env perl

use strict;
use warnings;
use lib './lib';
use FreecellSolver::QunitWrap ();
FreecellSolver::QunitWrap->new( { tests => "lib/for-node/test-code.js" } )->run;
