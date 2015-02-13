#!/usr/bin/perl

use strict;
use warnings;

use Carp ();

use Test::More tests => 2;

use Test::Differences qw(eq_or_diff);

use PDL;

use AI::Pathfinding::OptimizeMultiple;

# TEST:$c=0;
sub test_based_on_data
{
    local $Test::Builder::Level = $Test::Builder::Level + 1;

    my ($scans_aref, $quotas_aref, $want_results, $stats_factors, $blurb) = @_;

    my $results_aref = [];

    my $selected_scans =
    [
        map
        {
            my $id = $_->{name};

            AI::Pathfinding::OptimizeMultiple::Scan->new(
                id => $id,
                cmd_line => "-l $id",
            )
        }
        @{$scans_aref},
    ];

    my %num_boards = (map { scalar(@{$_->{data}}) => 1 } @$scans_aref);

    my @nums = keys(%num_boards);
    if (@nums != 1)
    {
        Carp::confess( "num is not 1.");
    }

    my $obj = AI::Pathfinding::OptimizeMultiple->new(
        {
            scans =>
            [
                map { +{ name => $_->{name} } } @$scans_aref,
            ],
            num_boards => $nums[0],
            scans_iters_pdls =>
            {
                map { $_->{name} => pdl($_->{data}), } @$scans_aref,
            },
            quotas => $quotas_aref,
            selected_scans => $selected_scans,
            optimize_for => "speed",
            ($stats_factors ? (stats_factors => $stats_factors) : ()),
        }
    );

    $obj->calc_meta_scan();

    my @have =
    (map
        {
            +{
                name => $scans_aref->[$_->scan_idx]->{name},
                iters => $_->iters,
            }
        }
        @{$obj->chosen_scans()}
    );

    # TEST:$c++;
    return eq_or_diff(
        \@have,
        $want_results,
        "$blurb - (eq_or_diff)",
    );
}
# TEST:$test_based_on_data=$c;

{
    # TEST*$test_based_on_data
    test_based_on_data(
        [
            {
                name => "first",
                data => [200,400,500],
            },
            {
                name => "second",
                data => [300,50,1000],
            },
            {
                name => "third",
                data => [10,10,100000],
            },
        ],
        [200, 500, ],
        [
            {
                name => "third",
                iters => 200,
            },
            {
                name => "first",
                iters => 500,
            },
        ],
        undef(),
        'Basic test',
    );
}

{
    # TEST*$test_based_on_data
    test_based_on_data(
        [
            {
                name => "first",
                data => [50_000,100_000,100_000,1_000_000,2_000_000],
            },
            {
                name => "second",
                data => [100,200,200,300,300],
            },
            {
                name => "third",
                data => [2000,2000,2000,2000,2000],
            },
        ],
        [100, 300,],
        [
            {
                name => "first",
                iters => 100_000,
            },
            {
                name => "second",
                iters => 300,
            },
        ],
        {
            first => 1000,
        },
        'Test the stats_factors',
    );
}
