#!/usr/bin/perl

use strict;
use warnings;

use Test::More tests => 3;

my $range_solver = $ENV{'FCS_PATH'} . "/freecell-solver-range-parallel-solve";

# TEST
ok (!system($range_solver, "1", "20", "10", "-l", "gi"), "Range solver was successful");

# TEST
ok (!system($range_solver, "1", "2", "1", "-opt"), "Range solver with -opt was successful");

# Apparently, the excessive creation of new soft_threads for the optimization
# thread caused next_soft_thread_id to overflow.
# This checks against it.

# TEST
ok (!system($range_solver, "1", "64", "1", "-l", "cool-jives", "-opt"), 
    "Range solver with -opt and a large number of boards was succesful."
);

