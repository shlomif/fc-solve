#!/usr/bin/perl

use strict;
use warnings;

use PDL ();

use AI::Pathfinding::OptimizeMultiple::DataInputObj;

my $above_how_much = shift(@ARGV) || 2000;

my $start_board = 1;
my $num_boards = 32000;

my $input_obj = AI::Pathfinding::OptimizeMultiple::DataInputObj->new(
    {
        start_board => $start_board,
        num_boards => $num_boards,
    }
);

my $data_hash_ref = $input_obj->get_scans_iters_pdls();
my $scan_ids = $input_obj->get_scan_ids_aref;

my $count_of_scans = @$scan_ids;

my $scans_data = PDL::cat( @{$data_hash_ref}{@$scan_ids} );

my $which_boards = PDL::which((($scans_data > $above_how_much) | ($scans_data < 0))->xchg(0,1)->sumover() == $count_of_scans)+1;

print "The indexes of the tests above $above_how_much are:\n";
foreach my $board (PDL::list($which_boards))
{
    print "$board: " . join(",", sort { (($b <=> 0) <=> ($a <=> 0)) || ($a <=> $b) } PDL::list($scans_data->slice(($board-1).",:"))) . "\n";
}


