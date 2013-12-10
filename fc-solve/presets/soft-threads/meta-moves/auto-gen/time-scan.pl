#!/usr/bin/perl

use strict;
use warnings;

use List::MoreUtils qw(firstidx);
use AI::Pathfinding::OptimizeMultiple::DataInputObj;

my $opt = (($ARGV[0] eq "--gen-bat") ? shift : "");

my @scan_args = @ARGV;
my $scan = join(" ", @scan_args);

my $start_board = 1;
my $num_boards = 32_000;

sub get_scan_index_by_its_cmd_line
{
    my ($prev_scans, $cmd_line) = @_;

    my $sel_scans = $prev_scans->selected_scans;

    return
        (firstidx { $_->cmd_line() eq $cmd_line } @$sel_scans);
}

my $prev_scans = AI::Pathfinding::OptimizeMultiple::DataInputObj->new(
    {
        start_board => $start_board,
        num_boards => $num_boards,
    }
);


if ((my $index = get_scan_index_by_its_cmd_line($prev_scans, $scan)) >= 0)
{
    die "The scan already exists with the ID "
        . $prev_scans->scan_id_by_index($index)
        . "\n";
}

my $id = $prev_scans->get_next_id();
{
    open my $out_fh, ">>", "scans.txt";
    print {$out_fh} "$id\t$scan\n";
    close ($out_fh);
}

my %params =
(
    id => $id,
    argv => [@scan_args],
);

if (exists($ENV{FC_NUM}))
{
    $params{freecells_num} = $ENV{FC_NUM};
}
if (exists($ENV{FC_VARIANT}))
{
    $params{variant} = $ENV{FC_VARIANT};
}

if ($opt eq "--gen-bat")
{
    open my $out, ">", "run_scan_$id.bat"
        or die "Could not open run_scan_$id.bat for writing";

    print {$out} join(" ", @{$prev_scans->get_scan_cmd_line(\%params)}), "\n\r";
    close($out);
}
else
{
    $prev_scans->time_scan(\%params);
}
