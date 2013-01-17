
use strict;
use warnings;

use Test::More;
use AI::Pathfinding::OptimizeMultiple::App::CmdLine;

use vars qw($trap);

eval q{use Test::Trap qw( trap $trap :flow:stderr(systemsafe):stdout(systemsafe):warn );};

if ($@)
{
    plan skip_all => "Test::Trap not found.";
}

plan tests => 6;

my @running_modes =
(
    {
        blurb_base => 'modulino',
        sub_ref => sub {
            my ($flags) = @_;
            AI::Pathfinding::OptimizeMultiple::App::CmdLine->new(
                {
                    argv => [@$flags],
                },
            )->run();
        },
    },
    {
        blurb_base => 'cmd_line',
        sub_ref => sub {
            my ($flags) = @_;
            system($^X, "bin/optimize-game-ai-multi-tasking", @$flags);
        },
    },
);

# TEST:$num_subs=2;
foreach my $mode (@running_modes)
{
    my $blurb_base = $mode->{blurb_base};

    trap(sub { return $mode->{sub_ref}->([qw(--help)]); } );

    # TEST*$num_subs
    like (
        $trap->stdout(),
        qr/--output/,
        "stdout matches --output flag. ($blurb_base)",
    );

    # TEST*$num_subs
    like (
        $trap->stdout(),
        qr/--help[^\n]*-h[^\n]*displays this help screen/ms,
        "stdout matches --output flag. ($blurb_base)",
    );

    # TEST*$num_subs
    ok (
        scalar (!$trap->die),
        "No exception was thrown. ($blurb_base)",
    );
}
