#!/usr/bin/perl

use PDL;

use MyInput;

use strict;

my $start_board = 1;
my $num_boards = 32000;

my $sel_scans = MyInput::get_selected_scan_list($start_board, $num_boards);

my @selected_scans = @$sel_scans;

my $scans_data = MyInput::get_scans_data($start_board, $num_boards, \@selected_scans);

my $seq = sequence(32000);

for(my $limit = 200 ; $limit < 1000 ; $limit++)
{
    my ($max, $max_ind);
    $max = -1;
    for(my $split_at = 0 ; $split_at <= $limit; $split_at++)
    {
        my $num = sum(($scans_data->slice("0,:") > $split_at) & ($scans_data->slice("1,:") > ($limit-$split_at)));
        if ($num > $max)
        {
            $max = $num;
            $max_ind = $split_at;
        }
    }
    print "$limit : $max_ind " . ($limit-$max_ind) . "\n"
}

