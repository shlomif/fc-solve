
use strict;
use warnings;

use Test::More;
use AI::Pathfinding::OptimizeMultiple::CmdLine;

use vars qw($trap);

eval q{use Test::Trap qw( trap $trap :flow:stderr(systemsafe):stdout(systemsafe):warn );};

if ($@)
{
    plan skip_all => "Test::Trap not found.";
}

plan tests => 3;

{
    trap(sub {
        AI::Pathfinding::OptimizeMultiple::CmdLine->new(
            {
                argv => [qw/--help/],
            },
        )->run();
    });

    # TEST
    like (
        $trap->stdout(),
        qr/--output/,
        "stdout matches --output flag.",
    );

    # TEST
    like (
        $trap->stdout(),
        qr/--help[^\n]*-h[^\n]*displays this help screen/ms,
        "stdout matches --output flag.",
    );

    # TEST
    ok (
        scalar (!$trap->die),
        "No exception was thrown.",
    );
}
