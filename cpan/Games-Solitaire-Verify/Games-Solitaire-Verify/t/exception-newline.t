#!/usr/bin/env perl

use strict;
use warnings;
use Test::More;

use vars qw/ $trap /;
eval
"use Test::Trap qw( trap \$trap :flow:stderr(systemsafe):stdout(systemsafe):warn );";

if ($@)
{
    plan 'skip_all' => "Failed to load Test::Trap . Skipping.";
}
else
{
    plan tests => 5;
}

{
    my $exit_code;
    trap(
        sub {
            $exit_code = system(
                $^X, "-I", "lib",
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

{
    my $exit_code;
    trap(
        sub {
            $exit_code = system(
                $^X, "-I", "lib",
                "bin/verify-solitaire-solution",
                "t/data/sample-solutions/fcs-seahaven-towers-1977.txt"
            );
        }
    );

    # TEST
    ok( $exit_code,
"[Freecell mode trying to verify Seahaven Towers] Non zero (failure) process exit code."
    );

    # TEST
    like(
        $trap->stderr(),

        # qr#\AWrong.*\n\z#,
        qr#\S#ms,
"[Freecell mode trying to verify Seahaven Towers] Trailing newline in error line.",
    );

    # TEST
    like(
        $trap->stdout(),

        # qr#\AWrong.*\n\z#,
        qr#Wrong#ms,
"[Freecell mode trying to verify Seahaven Towers] stdout says it is wrong.",
    );
}
