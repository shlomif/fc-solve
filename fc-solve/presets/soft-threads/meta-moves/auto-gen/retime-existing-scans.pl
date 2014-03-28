#!/usr/bin/perl

use strict;
use warnings;

use AI::Pathfinding::OptimizeMultiple::DataInputObj;

my $start_board = 1;
my $num_boards = 32_000;

my $input_obj = AI::Pathfinding::OptimizeMultiple::DataInputObj->new(
    {
        start_board => $start_board,
        num_boards => $num_boards,
    }
);

my %params;
if (exists($ENV{FC_NUM}))
{
    $params{freecells_num} = $ENV{FC_NUM};
}
if (exists($ENV{FC_VARIANT}))
{
    $params{variant} = $ENV{FC_VARIANT};
}

SCANS_LOOP:
foreach my $scan_rec (@{$input_obj->selected_scans()})
{
    my $id = $scan_rec->id();
    my @cmd_line = split(/ /, $scan_rec->cmd_line());
    print "Now doing Scan No. $id - '@cmd_line'\n";
    # Scan was already done.
    if (-e "data/$id.fcs.moves.txt")
    {
        next SCANS_LOOP;
    }
    $input_obj->time_scan({ %params, id => $id, argv => \@cmd_line, });
}

