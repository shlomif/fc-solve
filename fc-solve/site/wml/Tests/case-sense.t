#!/usr/bin/perl

use strict;
use warnings;
use Test::More tests => 1;

# TEST
is( system(q#find . -print0 | perl -n0e 'die $_ if $h{lc$_}++'#),
    0, "case sensitive filename collision" );

