#!/usr/bin/perl

use strict;
use warnings;

use Test::More tests => 3;
use Test::Trap qw( trap $trap :flow:stderr(systemsafe):stdout(systemsafe):warn );

use Carp;

{
    trap {
        # TEST
        ok (!system("perl process.pl > output.got"));
    };

    my $stderr = $trap->stderr();

    # TEST
    is ($stderr, "", "process.pl did not return any errors on stderr");

    # TEST
    ok (!system("cmp", "-s", "output.good", "output.got"),
        "results are identical"
    );
}

