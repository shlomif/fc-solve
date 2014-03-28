#!/usr/bin/perl

use strict;
use warnings;

use PDL ();

use AI::Pathfinding::OptimizeMultiple::DataInputObj;

my $input_file = shift || "script.sh";

my $start_board = 1;
my $num_boards = 32000;

my $input_obj = AI::Pathfinding::OptimizeMultiple::DataInputObj->new(
    {
        start_board => $start_board,
        num_boards => $num_boards,
    }
);

my $data_hash_ref = $input_obj->get_scans_lens_iters_pdls();

my @scan_ids = map { $_->id() } @{$input_obj->selected_scans};

my $data = PDL::cat( @{$data_hash_ref}{@scan_ids} )->xchg(1,3)->clump(2..3);

my (@chosen_scans);

open I, "<$input_file";
while (my $line = <I>)
{
    if ($line =~ /--prelude/)
    {
        $line =~ /\"(.*?)\"/;
        my $prelude = $1;
        @chosen_scans = (map { /^(\d+)\@(.+)$/; { 'ind' => $input_obj->lookup_scan_idx_based_on_id($2), 'q' => $1 }} split(/,/, $prelude));
        last;
    }
}



foreach my $board (1 .. $num_boards)
{
    my $total_iters = 0;
    my @info = PDL::list($data->slice(($board-1).",:"));
    print ("\@info=". join(",", @info). "\n");
    foreach my $s (@chosen_scans)
    {
        if (($info[$s->{'ind'}] > 0) && ($info[$s->{'ind'}] <= $s->{'q'}))
        {
            print "\t" . $info[$s->{'ind'}] . " \@ " . $scan_ids[$s->{'ind'}] . "\n";
            $total_iters += $info[$s->{'ind'}];
            last;
        }
        else
        {
            if ($info[$s->{'ind'}] > 0)
            {
                $info[$s->{'ind'}] -= $s->{'q'};
            }
            print "\t" . $s->{'q'} . " \@ " . $scan_ids[$s->{'ind'}] . "\n";
            $total_iters += $s->{'q'};
        }
    }
    print (($board+$start_board-1) . ": $total_iters\n");
}

