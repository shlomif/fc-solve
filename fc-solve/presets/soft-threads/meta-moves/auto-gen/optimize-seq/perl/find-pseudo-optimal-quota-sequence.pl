#!/usr/bin/perl

# This script is working but is very slow. The C# version (under
# (Find-Optimized-Sequence/ ) appears to be consdirably faster.

use strict;
use warnings;

use Getopt::Long;
use IO::All;

use AI::Pathfinding::OptimizeMultiple;

use MyInput;

my @guessed_quotas = ((350) x 300);

my @final_quotas;

my $num_boards = 32_000;

my $optimize_for = "speed";
my $start_board = 1;

my $start_quota = 100;
my $end_quota = 1_000;

my $input_obj = MyInput->new(
    {
        start_board => $start_board,
        num_boards => $num_boards,
    }
);

while (@final_quotas < @guessed_quotas)
{
    my $min_quota;
    my $min_quota_iters = -1;
    for my $quota ($start_quota .. $end_quota)
    {
        if (not $quota % 100)
        {
            warn "Quota == $quota\n";
        }
        my $arbitrator =
            AI::Pathfinding::OptimizeMultiple->new(
                    'quotas' =>
                    [
                        @final_quotas,
                        $quota,
                        @guessed_quotas[@final_quotas .. $#guessed_quotas]
                    ],
                    'selected_scans' => $input_obj->selected_scans(),
                    'num_boards' => $num_boards,
                    'scans_data' => $input_obj->get_scans_data()->copy(),
                    'optimize_for' => $optimize_for,
            );

        $arbitrator->calc_meta_scan();

        my $iters = $arbitrator->total_iters();

        if ($min_quota_iters < 0 || $iters < $min_quota_iters)
        {
            $min_quota = $quota;
            $min_quota_iters = $iters;
        }
    }
    print STDERR "Found $min_quota for No. " . scalar(@final_quotas) . " ($min_quota_iters)\n";
    push @final_quotas, $min_quota;
}

io("optimal-quotas.txt")->print(map { "$_\n" } @final_quotas);
