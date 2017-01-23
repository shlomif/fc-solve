#!/usr/bin/perl

use strict;
use warnings;

use FindBin ();
use lib "$FindBin::Bin";
use PrepareAppAws;

my @deals =
(qw/
982
/);

PrepareAppAws->new({deals => [@deals], num_freecells => 2, fcc_solver => 1,})->run;
