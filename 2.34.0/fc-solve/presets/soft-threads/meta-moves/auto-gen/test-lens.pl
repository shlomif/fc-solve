#!/usr/bin/perl

use strict;
use warnings;

use MyInput;

use vars qw($scans_lens_data);

my @params = (1,100);
$scans_lens_data = MyInput::get_scans_lens_data(@params, 
    MyInput::get_selected_scan_list(@params));

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

print "$solved\n";
print "$solved_moves\n";
print "SUMS = $solved_moves_sums\n";
print "COUNTS = $solved_moves_counts\n";
print "AVGS = $solved_moves_avgs\n";

print "5\n";
# while(1)
# {
#    sleep(1);
# }
