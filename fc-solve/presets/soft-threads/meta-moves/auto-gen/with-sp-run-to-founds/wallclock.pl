#!/usr/bin/perl

use strict;
use warnings;

use IO::All;

my @scans = (map { [ split /\t/, $_ ] } io("scans.txt")->chomp->getlines());

foreach my $scan (@scans)
{
    my ($id, $cmd_line) = @$scan;

    my $output_path = "wallclock-timings/$id.dump";

    if (! -e $output_path)
    {
        print "Timing $id\n";
        system("freecell-solver-range-parallel-solve 1 32000 1 --total-iterations-limit 100000 $cmd_line > $output_path");
    }
}
