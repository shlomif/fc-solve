#!/usr/bin/perl -w

use strict;

my $opt = (($ARGV[0] eq "--gen-bat") ? shift : "");

my (%scans);

my $id;

open I, "<next-id.txt";
$id = <I>;
chomp($id);
close(I);
open O, ">next-id.txt";
print O ($id+1);
close(O);

my @prev_scans;
open I, "<scans.txt";
while (my $line = <I>)
{
    chomp($line);
    my ($id, $cmd_line) = split(/\t/, $line);
    push @prev_scans, { 'id' => $id, 'cmd_line' => $cmd_line };
}
close(I);

my $scan = join(" ", @ARGV);

if (my @results = (grep { $_->{'cmd_line'} eq $scan } @prev_scans))
{
    die "The scan already exists with the ID " . $results[0]->{'id'} . "\n";    
}

open O, ">>scans.txt";
print O "$id\t$scan\n";
close(O);

my @cmd_line = (qw(freecell-solver-range-parallel-solve 1 32000 20),
        qw(--total-iterations-limit 100000 --binary-output-to), 
        "data/$id.data.bin", qw(--freecells-num 2),
        @ARGV);

if ($opt eq "--gen-bat")
{
    open O, ">run_scan_$id.bat";
    print O join(" ", @cmd_line), "\n\r";
    close(O);
}
else
{
    system(@cmd_line);
}
print "\a" x 10;
