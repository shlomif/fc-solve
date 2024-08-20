#!/usr/bin/env perl

use strict;
use warnings;
use 5.014;

use lib './lib';

use FreecellSolver::Site::TTRender ();

my $obj = FreecellSolver::Site::TTRender->new( {} );
$obj->proc("js-fc-solve/text/gui-tests.xhtml");
