#!/usr/bin/perl

use strict;
use warnings;
use autodie;

use AI::Pathfinding::OptimizeMultiple::DataInputObj;
use FC_Solve::TimePresets;

my $opt = (($ARGV[0] eq "--gen-bat") ? shift(@ARGV) : "");

my $id = shift(@ARGV);

my $start_board = 1;
my $num_boards = 32_000;

my $input_obj = AI::Pathfinding::OptimizeMultiple::DataInputObj->new(
    {
        start_board => $start_board,
        num_boards => $num_boards,
    }
);

my $index;

if (!defined($index = $input_obj->lookup_scan_idx_based_on_id($id)))
{
    die "Cannot find scan of scan ID $id";
}

my %params =
(
    %{FC_Solve::TimePresets->calc_params_from_environment()},
    id => $id,
    argv => [split /\s+/, $input_obj->selected_scans->[$index]->cmd_line()],
);

if ($opt eq "--gen-bat")
{
    open my $out_fh, ">", "run_scan_$id.bat";
    print {$out_fh} join(" ", $input_obj->_get_scan_cmd_line(\%params)), "\n\r";
    close ($out_fh);
}
else
{
    unlink("data/$id.data.bin");
    $input_obj->time_scan(\%params);
}
