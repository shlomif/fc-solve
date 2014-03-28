#!/usr/bin/perl

use strict;
use warnings;

use Getopt::Long;
use IO::All;

use FC_Solve::TimePresets;

use List::Util qw(first);

use Text::Table;

my $scan_ids = "1,2,3";
my $top_board_idx = 10;

GetOptions(
    "scans=s" => \$scan_ids,
    "top=i" => \$top_board_idx,
);

my @scans = ($scan_ids =~ m{([0-9]+)}g);

my $input_obj = FC_Solve::TimePresets->new;

my @scan_indexes = (map { $input_obj->lookup_scan_idx_based_on_id($_) } @scans);

my $tb =
    Text::Table->new(
        "Deal No.",
            (map { (\" | "), "$_ Iters", (\" | "),  "$_ Len" } @scans),
            (\" |"),
    );

my $data_hash_ref = $input_obj->get_scans_lens_iters_pdls();
my $scans_lens_data = PDL::cat( @{$data_hash_ref}{
        @{ $input_obj->get_scan_ids_aref }
        })->xchg(1,3)->clump(2..3);

foreach my $board_idx (1 .. $top_board_idx)
{
    my $at = sub {
        my ($scan_idx, $stat) = @_;

        return $scans_lens_data->at(
            $board_idx- $input_obj->start_board,
            $scan_idx,
            $stat
        );
    };

    $tb->add($board_idx, (map { $at->($_,0), $at->($_,1) } @scan_indexes));
}

print $tb;

