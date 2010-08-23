#!/usr/bin/perl

use strict;
use warnings;

use MyInput;

my $opt = (($ARGV[0] eq "--gen-bat") ? shift : "");

my $id = shift(@ARGV);

my $start_board = 1;
my $num_boards = 32_000;

my $input_obj = MyInput->new(
    {
        start_board => $start_board,
        num_boards => $num_boards,
    }
);

my $index;

if (!defined($index = $input_obj->get_scan_index_by_id($id)))
{
    die "Cannot find scan of scan ID $id";
}

my %params =
(
    id => $id,
    argv => [split /\s+/, $input_obj->scan_cmd_line_by_index($index)],
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
    unlink("data/$id.data.bin");
    $input_obj->time_scan(\%params);
}
