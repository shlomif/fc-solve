#!/usr/bin/perl

use strict;
use warnings;

use MyInput;

my $id = shift(@ARGV);

my $start_board = 1;
my $num_boards = 32_000;

my $input_obj = MyInput->new(
    {
        start_board => $start_board,
        num_boards => $num_boards,
    }
);

foreach my $scan_rec (@{$input_obj->selected_scans()})
{
    my $id = $scan_rec->id();
    my @cmd_line = split(/ /, $scan_rec->cmd_line());
    print "Now doing Scan No. $id - '@cmd_line'\n";
    # Scan was already done.
    if (-e "data/$id.fcs.moves.txt")
    {
        next;
    }
    $input_obj->time_scan({ id => $id, argv => \@cmd_line, });
}

