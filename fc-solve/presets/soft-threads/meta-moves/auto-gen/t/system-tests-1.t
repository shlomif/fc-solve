#!/usr/bin/perl

use strict;
use warnings;

use Test::More tests => 3;
use Test::Trap qw( trap $trap :flow:stderr(systemsafe):stdout(systemsafe):warn );

use Test::Differences qw( eq_or_diff );
use IO::All qw(io);

use Carp;

{
    trap {
        # TEST
        ok (!system("perl process.pl"));
    };

    my $stderr = $trap->stderr();
    my $got = $trap->stdout();

    # TEST
    is ($stderr, "", "process.pl did not return any errors on stderr");

    my $expected = io->file('output.good')->all;

    # TEST
    eq_or_diff(
        $got,
        $expected,
        "perl process.pl results are identical"
    );
}

