#!/usr/bin/perl

use strict;
use warnings;

use FC_Solve::TimePresets;

my @params = (1,100);

my $input_obj = FC_Solve::TimePresets->new;
my $scans_lens_data = $input_obj->calc_scans_lens_data;

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
