#!/usr/bin/perl

use strict;
use warnings;

use Getopt::Long;
use IO::All;

use List::Util qw(first);

use FC_Solve::TimePresets;

my $with_len = 0;

GetOptions(
    "l!" => \$with_len,
);

my $scan_id = shift(@ARGV)
    or die "scan_id not specified on command line.";

my $board_idx = shift(@ARGV)
    or die "board_idx not specified on command line.";

my $input_obj = FC_Solve::TimePresets->new;

my $start_board = $input_obj->start_board();
if ($with_len)
{
    my $at = sub {
        my ($field_idx) = @_;
        return $input_obj->get_scans_lens_iters_pdls()->{$scan_id}->at($board_idx-$start_board, 0, $field_idx);
    };
    print map { $at->($_) , "\n" } (0, 1);
}
else
{
    print $input_obj->get_scans_iters_pdls()->{$scan_id}->at($board_idx-$start_board), "\n";
}
