#!/usr/bin/perl

use strict;
use warnings;

use MyInput;

my $opt = (($ARGV[0] eq "--gen-bat") ? shift : "");

my $id = shift(@ARGV);

my $prev_scans = MyInput::get_prev_scans();

my @results;
if ((@results = (grep { $_->{'id'} eq $id } @$prev_scans)) != 1)
{
    die "Cannot find scans or too many of them.";
}

my %params =
(
    id => $id,
    argv => [split /\s+/, $results[0]->{'cmd_line'}]
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
    MyInput::time_scan(\%params);
}

# system("$ENV{'HOME'}/apps/alert/finish-cue");
