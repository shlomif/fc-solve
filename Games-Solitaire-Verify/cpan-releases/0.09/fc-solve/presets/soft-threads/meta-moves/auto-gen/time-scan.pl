#!/usr/bin/perl

use strict;
use warnings;

use MyInput;

my $opt = (($ARGV[0] eq "--gen-bat") ? shift : "");

my $prev_scans = MyInput::get_prev_scans();

my $scan = join(" ", @ARGV);

if (my @results = (grep { $_->{'cmd_line'} eq $scan } @$prev_scans))
{
    die "The scan already exists with the ID " . $results[0]->{'id'} . "\n";    
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
    MyInput::time_scan(\%params);
}

system("$ENV{'HOME'}/apps/alert/finish-cue");
