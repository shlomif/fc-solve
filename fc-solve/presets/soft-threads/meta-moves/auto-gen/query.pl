#!/usr/bin/perl

use strict;
use warnings;

use Getopt::Long;
use IO::All;

use List::Util qw(first);

use Shlomif::FCS::CalcMetaScan;

use MyInput;

my $with_len = 0;

GetOptions(
    "l!" => \$with_len,
);

my ($scan_id, $board_idx) = @ARGV;

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

my $scan_idx =
    first { $input_obj->selected_scans()->[$_]->id() == $scan_id }
    (0 .. $#{$input_obj->selected_scans()})
    ;


if ($with_len)
{
    my $at = sub {
        return $input_obj->get_scans_lens_data()->at($board_idx-$start_board, $scan_idx, shift());
    };
    print map { $at->($_) , "\n" } (0, 1);
}
else
{
    print $input_obj->get_scans_data()->at($board_idx-$start_board, $scan_idx), "\n";
}

