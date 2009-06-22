#!/usr/bin/perl -w

use strict;

use PDL;

use MyInput;

my $start_board = 1;

my $num_boards = 32000;

my $above_how_much = shift || 2000;

my $sel_scans = MyInput::get_selected_scan_list($start_board, $num_boards);

my @selected_scans = @$sel_scans;

my $scans_data = 
    MyInput::get_scans_data(
        $start_board, 
        $num_boards, 
        \@selected_scans
    );

my $which_boards = which((($scans_data > $above_how_much) | ($scans_data < 0))->xchg(0,1)->sumover() == scalar(@selected_scans))+1;

print "The indexes of the tests above $above_how_much are:\n";
foreach my $board (list($which_boards))
{
    print "$board: " . join(",", sort { (($b <=> 0) <=> ($a <=> 0)) || ($a <=> $b) } list($scans_data->slice(($board-1).",:"))) . "\n";
}


