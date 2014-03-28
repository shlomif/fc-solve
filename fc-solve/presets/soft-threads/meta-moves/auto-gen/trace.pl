#!/usr/bin/perl

use strict;
use warnings;
use autodie;

use AI::Pathfinding::OptimizeMultiple;
use AI::Pathfinding::OptimizeMultiple::ScanRun;

use PDL ();

use FC_Solve::TimePresets;

my $input_filename = shift || "script.sh";

my $input_obj = FC_Solve::TimePresets->new;
my $scans_lens_data = $input_obj->calc_scans_lens_data;

my $scan_ids = $input_obj->get_scan_ids_aref;

sub _read_chosen_scans
{
    open my $input_fh, '<', $input_filename;

    while (my $line = <$input_fh>)
    {
        if (my ($prelude) = $line =~ /--prelude\s*"([^"]+)"/)
        {
            close ($input_fh);
            return [
                map { _calc_scan_run($_) } split(/,/, $prelude)
            ];
        }
    }

    close($input_fh);

    die "Could not find --prelude line in file '$input_filename'";
}

my $chosen_scans = _read_chosen_scans();

sub _calc_scan_run
{
    my ($s) = @_;

    if (my ($quota, $scan_id) = ($s =~ /^(\d+)\@(.+)$/))
    {
        return AI::Pathfinding::OptimizeMultiple::ScanRun->new(
            {
                iters => $quota,
                scan_idx => $input_obj->lookup_scan_idx_based_on_id($scan_id),
            },
        );
    }
    else
    {
        die "Unknown format '$_'!";
    }
}


my $runner = AI::Pathfinding::OptimizeMultiple->new(
    {
        'scans' =>
        [
            map { +{ name => $_->id() } }
            @{$input_obj->selected_scans},
        ],
        # Does not matter.
        'quotas' => [500],
        'selected_scans' => $input_obj->selected_scans(),
        'num_boards' => $input_obj->num_boards(),
        'scans_iters_pdls' => $input_obj->get_scans_iters_pdls(),
        'optimize_for' => 'speed',
    },
);

my $start_board = $input_obj->start_board();
foreach my $board (0 .. $input_obj->num_boards()-1)
{
    my @info = PDL::list($scans_lens_data->slice(($board).",:"));
    my $results = $runner->simulate_board($board,
        {
            chosen_scans => $chosen_scans,
        }
    );
    print ("\@info=". join(",", @info). "\n");
    foreach my $s (@{ $results->scan_runs })
    {
        print "\t" . $s->iters() . " \@ " . $scan_ids->[$s->scan_idx()] . "\n";
    }
    print (($board+$start_board) . ": ", $results->total_iters(), "\n");
}

