#!/usr/bin/perl

use strict;
use warnings;

use MyInput;

my $prev_scans = MyInput::get_prev_scans();

foreach my $scan_rec (@$prev_scans)
{
    my $id = $scan_rec->{'id'};
    my @cmd_line = split(/ /, $scan_rec->{'cmd_line'});
    print "Now doing Scan No. $id - '@cmd_line'\n";
    # Scan was already done.
    if (-e "data/$id.fcs.moves.txt")
    {
        next;
    }
    MyInput::time_scan({ id => $id, argv => \@cmd_line, });
}

