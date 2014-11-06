#!/usr/bin/perl

use strict;
use warnings;

require 5.008;

use File::Spec;

use autodie;

use Test::More tests => 1;

use Games::Solitaire::Verify::App::CmdLine::From_Patsolve;

my $sol_fn = File::Spec->catfile(File::Spec->curdir(),
    qw(t data sample-solutions patsolve-338741497-win-solution.txt)
);

my $board_fn = File::Spec->catfile(File::Spec->curdir(),
    qw(t data boards 338741497.board)
);

sub _slurp
{
    my $filename = shift;

    open my $in, '<', $filename
        or die "Cannot open '$filename' for slurping - $!";

    local $/;
    my $num_classesontents = <$in>;

    close($in);

    return $num_classesontents;
}

{
    my $obj = Games::Solitaire::Verify::App::CmdLine::From_Patsolve->new(
        {
            argv => [qw(-g freecell), $board_fn, $sol_fn,],
        }
    );

    $obj->_read_initial_state();

    # TEST
    ok (1, "_read_initial_state is successful.");
}

