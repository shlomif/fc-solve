#!/usr/bin/perl

use strict;
use warnings;

use Test::More tests => 2;
use Carp;
use Data::Dumper;
use String::ShellQuote;

my $range_solver = $ENV{'FCS_PATH'} . "/freecell-solver-range-parallel-solve";

# TEST
ok (!system($range_solver, "1", "20", "10", "-l", "gi"), "Range solver was successful");

# TEST
ok (!system($range_solver, "1", "2", "1", "-opt"), "Range solver with -opt was successful");

