#!/usr/bin/env perl

use strict;
use warnings;
use Test::More tests => 2;

use vars qw/ $trap /;
eval
"use Test::Trap qw( trap \$trap :flow:stderr(systemsafe):stdout(systemsafe):warn );";

if ($@)
{
    plan 'skip_all' => "No Test::Trap!";
}

{
    my $exit_code;
    trap(
        sub {
            $exit_code = system( $^X, "-I", "lib",
                "bin/verify-solitaire-solution",
                "t/data/sample-solutions/1.fc-wrong.sol.txt"
            );
        }
    );

    # TEST
    ok( $exit_code, "Non zero (failure) process exit code." );

    # TEST
    like( $trap->stderr(), qr#\AWrong.*\n\z#, "Trailing newline in error line.",
    );
}
