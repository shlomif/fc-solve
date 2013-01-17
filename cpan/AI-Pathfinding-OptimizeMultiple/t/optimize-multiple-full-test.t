#!/usr/bin/perl

use strict;
use warnings;

use Test::More tests => 1;

use Test::Differences qw(eq_or_diff);

use PDL;

use AI::Pathfinding::OptimizeMultiple;

# TEST:$c=0;
sub test_based_on_data
{
    local $Test::Builder::Level = $Test::Builder::Level + 1;

    my ($scans_aref, $quotas_aref, $want_results, $blurb) = @_;

    my $results_aref = [];

    my $selected_scans =
    [
        map
        {
            my $id = $_->{name};

            AI::Pathfinding::OptimizeMultiple::Structs::Scan->new(
                id => $id,
                cmd_line => "-l $id",
            )
        }
        @{$scans_aref},
    ];

    my $obj = AI::Pathfinding::OptimizeMultiple->new(
        {
            scans =>
            [
                map { +{ name => $_->{name} } } @$scans_aref,
            ],
            num_boards => 3,
            scans_iters_pdls =>
            {
                map { $_->{name} => pdl($_->{data}), } @$scans_aref,
            },
            quotas => $quotas_aref,
            selected_scans => $selected_scans,
            optimize_for => "speed",
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
        'Basic test',
    );
}

