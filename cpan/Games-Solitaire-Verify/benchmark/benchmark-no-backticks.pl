#!/usr/bin/perl

use strict;
use warnings;
use autodie;

use lib '../Games-Solitaire-Verify/lib';

use FC_Solve::Solve (qw/fc_solve_init solve/);

use List::MoreUtils qw(first_index true);

use Games::Solitaire::Verify::Solution;
use Games::Solitaire::Verify::App::CmdLine;

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

