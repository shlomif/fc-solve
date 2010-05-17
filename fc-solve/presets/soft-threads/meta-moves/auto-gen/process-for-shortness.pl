#!/usr/bin/perl

use strict;
use warnings;

use Getopt::Long;

use List::Util qw(min);

use Shlomif::FCS::CalcMetaScan;

use MyInput;

my $with_len = 0;

GetOptions(
    "l!" => \$with_len,
);

my ($scan_id, $board_idx) = @ARGV;

my @guessed_quotas = ((350) x 300);

my @final_quotas;

my $start_board = 1;
my $num_boards = 32_000;

my $input_obj = MyInput->new(
    {
        start_board => $start_board,
        num_boards => $num_boards,
    }
);

my $scan_index = 0;

my $data = $input_obj->get_scans_lens_data();

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
print 
(
    join(" \\\n-nf ", map { "--flare-name $_->{id} $_->{cmd_line} -opt" } @results),
    " \\\n --flares-plan '",
    join(",", map { "Run:$_->{quota}\@$_->{id}" } @results),
    "'\n",
);
