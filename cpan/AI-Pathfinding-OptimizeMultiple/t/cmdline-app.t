
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

plan tests => 6;

# TEST:$num_subs=2;
foreach my $sub_ref (
    sub {
        AI::Pathfinding::OptimizeMultiple::CmdLine->new(
            {
                argv => [qw/--help/],
            },
        )->run();
    },
    sub {
        system($^X, "bin/optimize-game-ai-multi-tasking", "--help");
    },
)
{
    trap($sub_ref);

    # TEST*$num_subs
    like (
        $trap->stdout(),
        qr/--output/,
        "stdout matches --output flag.",
    );

    # TEST*$num_subs
    like (
        $trap->stdout(),
        qr/--help[^\n]*-h[^\n]*displays this help screen/ms,
        "stdout matches --output flag.",
    );

    # TEST*$num_subs
    ok (
        scalar (!$trap->die),
        "No exception was thrown.",
    );
}
