#!/usr/bin/perl

use strict;
use warnings;

use FindBin ();
use lib "$FindBin::Bin";
use PrepareAppAws;

my @deals =
(qw/
504179225
8203007169
/);

PrepareAppAws->new({deals => [@deals],})->run;
