#!/usr/bin/perl

use strict;
use warnings;

use MyInput;

my $opt = (($ARGV[0] eq "--gen-bat") ? shift : "");

my $scan = join(" ", @ARGV);

my $start_board = 1;
my $num_boards = 32_000;

my $prev_scans = MyInput->new(
    {
        start_board => $start_board,
        num_boards => $num_boards,
    }
);

if (my $index = $input_obj->get_scan_index_by_its_cmd_line($cmd_line))
{
    die "The scan already exists with the ID "
        . $input_obj->scan_id_by_index($index)
        . "\n";
}

my $id = MyInput::get_next_id();
open O, ">>", "scans.txt";
print O "$id\t$scan\n";
close(O);

my %params =
(
    id => $id,
    argv => \@ARGV,
);

if (exists($ENV{FC_NUM}))
{
    $params{freecells_num} = $ENV{FC_NUM};
}

if ($opt eq "--gen-bat")
{
    open O, ">run_scan_$id.bat";
    print O join(" ", @{MyInput::get_scan_cmd_line(\%params)}), "\n\r";
    close(O);
}
else
{
    $input_obj->time_scan(\%params);
}
