#!/usr/bin/perl

use strict;
use warnings;

local $ENV{FC_SOLVE_RANGE_ITERS_LIMIT} = '5000000';

my $dir = "../../meta-moves/auto-gen/";
exec( $^X, "-I$dir", "$dir/time-scan.pl", @ARGV );
