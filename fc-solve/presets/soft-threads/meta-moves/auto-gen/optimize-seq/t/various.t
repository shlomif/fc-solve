#!/usr/bin/perl

use strict;
use warnings;

use Test::More tests => 3;
use Test::Trap qw( trap $trap :flow:stderr(systemsafe):stdout(systemsafe):warn );

{
    trap {    
        # TEST
        ok (!system("mono find_opt.exe test_blacklist"));
    };

    my $stderr = $trap->stderr();

    # TEST
    is ($stderr, "", "process.pl did not return any errors on stderr");

    # TEST
    is ($trap->stdout(), "7\n8\n", "Black list is OK.");
}
