#!/usr/bin/perl

use strict;
use warnings;

use FindBin ();
use lib "$FindBin::Bin";
use PrepareAppAws;

my @deals = (
    qw/
        20410
        24966
        /
);

PrepareAppAws->new( { deals => [@deals], num_freecells => 1 } )->run;
