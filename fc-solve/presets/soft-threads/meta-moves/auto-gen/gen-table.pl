#!/usr/bin/perl

use strict;
use warnings;

use Getopt::Long;
use IO::All;

use List::Util qw(first);

use Text::Table;
use Shlomif::FCS::CalcMetaScan;

use MyInput;

my $scan_ids = "1,2,3";
my $top_board_idx = 10;

GetOptions(
    "scans=s" => \$scan_ids,
    "top=i" => \$top_board_idx,
);

my @scans = ($scan_ids =~ m{(\d+)}g);

my $num_boards = 32_000;

my $start_board = 1;

my $input_obj = MyInput->new(
    {
        start_board => $start_board,
        num_boards => $num_boards,
    }
);

my @scan_indexes =
(
    map {
        my $scan_id = $_;
        first { $input_obj->selected_scans()->[$_]->id() == $scan_id }
        (0 .. $#{$input_obj->selected_scans()})
    } (@scans)
);


my $tb = 
    Text::Table->new(
        "Deal No.", 
            (map { (\" | "), "$_ Iters", (\" | "),  "$_ Len" } @scans),
            (\" |"),
    );

my $pdl = $input_obj->get_scans_lens_data();
foreach my $board_idx (1 .. $top_board_idx)
{
    my $at = sub {
        my ($scan_idx, $stat) = @_;

        return $pdl->at($board_idx-$start_board, $scan_idx, $stat);
    };
    
    $tb->add($board_idx, (map { $at->($_,0), $at->($_,1) } @scan_indexes));
}

print $tb;

