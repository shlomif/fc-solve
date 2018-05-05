#!/usr/bin/perl

use strict;
use warnings;

local $ENV{FC_SOLVE_RANGE_ITERS_LIMIT} = '100000000';

my $dir = "../../meta-moves/auto-gen/";
exec( $^X, "-I$dir", "$dir/retime-scan.pl", @ARGV );
