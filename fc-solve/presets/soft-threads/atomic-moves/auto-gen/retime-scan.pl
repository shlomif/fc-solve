#!/usr/bin/perl

use strict;
use warnings;

my $dir = "../../meta-moves/auto-gen/";
exec($^X, "-I$dir", "$dir/retime-scan.pl", @ARGV);
