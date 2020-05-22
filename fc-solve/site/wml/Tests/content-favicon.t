#!/usr/bin/env perl

use strict;
use warnings;
use Test::More tests => 1;

{
    # TEST
    cmp_ok( scalar( -s "./dest/favicon.ico" ), ">=", 50, "favicon.ico" );
}
