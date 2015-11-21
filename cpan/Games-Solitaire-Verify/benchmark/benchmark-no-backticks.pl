#!/usr/bin/perl

use strict;
use warnings;

use lib '../Games-Solitaire-Verify/lib';

use FC_Solve::Solve (qw/fc_solve_init solve/);

use autodie;

use List::MoreUtils qw(first_index true);

use Games::Solitaire::Verify::Solution;
use Games::Solitaire::Verify::App::CmdLine;

if ($ARGV[0] =~ m{\A-h|--help\z})
{
    print <<'EOF';
summarize-fc-solve [Game Params] -- [Extra fc-solve Args]

Display the solution status, the iterations count and the solution length
of the Freecell Solver invocation specified by the arguments. Also verifies
that the solution is true.

The [Game Params] flags (e.g: -g , --freecells-num, --stacks-num, etc.) are also
used to determine the variant used by the solution verifier.
EOF
    exit (0);
}

my @args = @ARGV;

my $separator = first_index { $_ eq '--' } @args;
if ($separator < 0)
{
    die "You must specify both [Game Params] and [Extra fc-solve Args]. See --help";
}

my @game_params = @args[0 .. $separator - 1];
my @fc_solve_args = (@game_params, @args[$separator+1 .. $#args]);

my $FIRST_INDEX = ($ENV{F} || 1);
my $LAST_INDEX = ($ENV{L} || 100);

fc_solve_init(\@fc_solve_args);

foreach my $board_idx ($FIRST_INDEX .. $LAST_INDEX)
{
    solve (\@game_params, $board_idx);
}

