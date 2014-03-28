#!/usr/bin/perl

use strict;
use warnings;

use AI::Pathfinding::OptimizeMultiple::DataInputObj;

my @params = (1,100);

my $start_board = 1;
my $num_boards = 32_000;

my $input_obj = AI::Pathfinding::OptimizeMultiple::DataInputObj->new(
    {
        start_board => $start_board,
        num_boards => $num_boards,
    }
);

my $data_hash_ref = $input_obj->get_scans_lens_iters_pdls();

my @scan_ids = map { $_->id() } @{$input_obj->selected_scans};

my $scans_lens_data = PDL::cat( @{$data_hash_ref}{@scan_ids} )->xchg(1,3)->clump(2..3);

my $iters = $scans_lens_data->slice(":,:,0");
my $iters_quota = 500;
my $solved = (($iters <= $iters_quota) & ($iters > 0));
my $num_moves = $scans_lens_data->slice(":,:,2");
my $solved_moves = $solved * $num_moves;

sub my_sum_over
{
    my $pdl = shift;

    return $pdl->sumover()->slice(":,(0)");
}

my $solved_moves_sums = my_sum_over($solved_moves);
my $solved_moves_counts = my_sum_over($solved);
my $solved_moves_avgs = $solved_moves_sums / $solved_moves_counts;

# print "$solved\n";
# print "$solved_moves\n";
print "SUMS = $solved_moves_sums\n";
print "COUNTS = $solved_moves_counts\n";
print "AVGS = $solved_moves_avgs\n";
