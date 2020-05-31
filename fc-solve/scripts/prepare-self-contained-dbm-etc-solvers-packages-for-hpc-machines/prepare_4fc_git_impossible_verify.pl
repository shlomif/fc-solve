#!/usr/bin/perl

use strict;
use warnings;

use Path::Tiny qw/ path tempdir tempfile cwd /;

use FindBin ();
use lib "$FindBin::Bin";
use PrepareAppAws;

my @deals =
    path("../B/freecell-pro-impossible-deals/logs/intractable1.txt")->lines_raw;
chomp @deals;
PrepareAppAws->new(
    { deals => [@deals], deal_num_width => 11, disable_threading => 1, } )->run;
