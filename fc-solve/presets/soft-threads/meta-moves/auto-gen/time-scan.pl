#!/usr/bin/perl -w

use strict;

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
    push @prev_scans, { 'id' => $id, 'cmd_line' => $line };
}
close(I);

my $scan = join(" ", @ARGV);

if (my @results = (grep { $_->{'cmd_line'} eq $scan } @ARGV))
{
    die "The scan already exists with the ID " . $results[0]->{'id'} . "\n";    
}

open O, ">>scans.txt";
print O "$id\t$scan\n";
close(O);

system(qw(freecell-solver-range-parallel-solve 1 32000 20),
    qw(--total-iterations 100000 --binary-output-to), 
    "data/$id.data.bin",
    @ARGV
);
print "\a" x 10;
