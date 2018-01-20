#!/usr/bin/perl

use strict;
use warnings;

while (my $l = <>)
{
    chomp$l;
    if (my ($seed, $deal) = ($l =~ /\AFound seed = ([0-9]+) for deal = ([0-9]+)\z/))
    {
        print "=== $l\n";
        system("pi-make-microsoft-freecell-board -t $deal | ../scripts/summarize-fc-solve --freecells-num 3 -- --method random-dfs -sp r:tf -to '[0123456789ABCDE]' -seed $seed") and die "failed '$l'";
    }
    else
    {
        die "Improper line '$l'";
    }
}
