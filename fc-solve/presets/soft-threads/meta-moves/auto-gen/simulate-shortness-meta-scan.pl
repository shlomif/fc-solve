#!/usr/bin/perl

use strict;
use warnings;

use Getopt::Long;

use PDL ();

use List::Util qw(min first);

use AI::Pathfinding::OptimizeMultiple::DataInputObj;

use IO::Handle;

STDOUT->autoflush(1);

my $preset;

GetOptions(
    "preset|p=s" => \$preset,
);

if (!defined($preset))
{
    die "You must specify a preset using --preset or -p";
}

my @final_quotas;

my $start_board = 1;
my $num_boards = 32_000;

my $input_obj = AI::Pathfinding::OptimizeMultiple::DataInputObj->new(
    {
        start_board => $start_board,
        num_boards => $num_boards,
    }
);

my $scan_index = 0;

my $data_hash_ref = $input_obj->get_scans_lens_iters_pdls();

my @scan_ids = map { $_->id() } @{$input_obj->selected_scans};

my %scan_ids_to_indexes = (map { $scan_ids[$_] => $_ } keys @scan_ids);

my $data = PDL::cat( @{$data_hash_ref}{@scan_ids} )->xchg(1,3)->clump(2..3);

my @results;
foreach my $scan (@{$input_obj->selected_scans()})
{
    my $scan_id = $scan->id();
    my $cmd_line = $scan->cmd_line();

    my $vec = $data->slice(":,$scan_index,0");
    $vec = $vec->where($vec > 0);

    my $sorted = $vec->flat()->qsort();

    # The step should be the 90% percentile, but not higher than
    # 10,000 iterations.
    my $quota = min($sorted->at(int(($sorted->dims())[0] * 0.9)), 10_000);

    push @results, { cmd_line => $cmd_line, id => $scan_id, quota => $quota };
}
continue
{
    $scan_index++;
}


# Flair is our temporary name for these independently evaluated instances
# print join(" \\\n--next-flair ", map { "--flair-id $_->{id} --flair-quota $_->{quota} $_->{cmd_line}" } @results);

{
    my $x = $data->slice(":,:,1")->clump(1..2);

    $x = ($x >= 0) * $x + ($x < 0) * PDL->ones($x->dims()) * 100_000;

    my $histograms =
        $x->xchg(0,1)->qsorti()->xchg(0,1)->histogram(1,0,scalar(@results));

    # print $histograms;

    my $min = $x->xchg(0,1)->minimum();

    my $average = $min->avg();

    print "Average min len = $average\n";

    while (length($preset))
    {
        if ($preset =~ s{\ARun:([0-9]+)\@([0-9]+),?}{})
        {
            my ($iters_quota, $scan_id) = ($1, $2);

            my $scan_index = $scan_ids_to_indexes{$scan_id};

            if (!defined ($scan_index))
            {
                die "Unknown Scan ID: '$scan_id'";
            }

            my $iters_vec = $data->slice(":,($scan_index),(0)");
            my $sol_len_vec = $data->slice(":,($scan_index),(1)");

            my $which_solved =
                ( ($iters_vec > 0 ) & ($iters_vec < $iters_quota) )->which();

            my $num_solved = $which_solved->nelem();

            my $solved_lens = $sol_len_vec->index($which_solved);

            print "Num solved = $num_solved\n";
            print "Average solved solution length = ",
                $solved_lens->avg(), "\n";

            print "Minimal solutions = ",
                ($solved_lens == $min->index($which_solved))->sum(),
                "\n";
        }
    }
}

