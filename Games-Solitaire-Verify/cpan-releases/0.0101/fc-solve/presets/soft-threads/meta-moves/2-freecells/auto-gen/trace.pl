#!/usr/bin/perl -w

use strict;

use PDL;

use MyInput;

my $input_file = shift || "script.sh";

my $start_board = 1;
my $num_boards = 32000;

my $sel_scans = MyInput::get_selected_scan_list($start_board, $num_boards);
my @selected_scans = @$sel_scans;
    
#my $scans_data = [];
#my $scans_data = zeroes($num_boards, scalar(@selected_scans));

my $scans_data = MyInput::get_scans_data($start_board, $num_boards, \@selected_scans);

my (@chosen_scans);

my %ids_to_inds = (map { $selected_scans[$_]->{'id'} => $_ } (0 .. $#selected_scans));

open I, "<$input_file";
while (my $line = <I>)
{
    if ($line =~ /--prelude/)
    {
        $line =~ /\"(.*?)\"/;
        my $prelude = $1;
        @chosen_scans = (map { /^(\d+)\@(.+)$/; { 'ind' => $ids_to_inds{$2}, 'q' => $1 }} split(/,/, $prelude));
        last;
    }
}



foreach my $board (1 .. $num_boards)
{
    my $total_iters = 0;
    my @info = list($scans_data->slice(($board-1).",:"));
    print ("\@info=". join(",", @info). "\n");
    foreach my $s (@chosen_scans)
    {
        if (($info[$s->{'ind'}] > 0) && ($info[$s->{'ind'}] <= $s->{'q'}))
        {
            print "\t" . $info[$s->{'ind'}] . " \@ " . $selected_scans[$s->{'ind'}]->{'id'} . "\n";
            $total_iters += $info[$s->{'ind'}];
            last;
        }
        else
        {
            if ($info[$s->{'ind'}] > 0)
            {
                $info[$s->{'ind'}] -= $s->{'q'};
            }
            print "\t" . $s->{'q'} . " \@ " . $selected_scans[$s->{'ind'}]->{'id'} . "\n";
            $total_iters += $s->{'q'};
        }
    }
    print (($board+$start_board-1) . ": $total_iters\n");
}

