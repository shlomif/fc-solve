#!/usr/bin/perl

use strict;
use warnings;

use FindBin ();
use lib "$FindBin::Bin";
use PrepareAppAws;

my @deals = (9189909);
PrepareAppAws->new( { deals => [@deals], num_freecells => 2, } )->run;
