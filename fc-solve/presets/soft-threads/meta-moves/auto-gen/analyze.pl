#!/usr/bin/perl -w

use strict;

use PDL;

use MyInput;

my $num_boards = 32000;

my $above_how_much = shift || 2000;

my @scans;

open I, "<scans.txt";
while (my $line = <I>)
{
    chomp($line);
    my ($id, $cmd_line) = split(/\t/, $line);
    push @scans, { 'id' => $id, 'cmd_line' => $cmd_line };
}
close(I);

my @selected_scans = 
    grep 
    { 
        my @stat = stat("./data/".$_->{'id'}.".data.bin");
        scalar(@stat) && ($stat[7] == 12+$num_boards*4);
    }
    @scans;

my $scans_data = MyInput::get_scans_data($num_boards, \@selected_scans);

my $which_boards = which(($scans_data > $above_how_much)->xchg(0,1)->sumover() == scalar(@selected_scans))+1;

print "The indexes of the tests above $above_how_much are:\n";
foreach my $board (list($which_boards))
{
    print "$board: " . join(",", list($scans_data->slice(($board-1).",:"))) . "\n";
}


