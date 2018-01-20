#!/usr/bin/perl

use strict;
use warnings;

while ( my $l = <> )
{
    chomp $l;
    if ( my ( $seed, $deal ) =
        ( $l =~ /\AFound seed = ([0-9]+) for deal = ([0-9]+)\z/ ) )
    {
        print "=== $l\n";
        my $verdict =
`pi-make-microsoft-freecell-board -t $deal | ../scripts/summarize-fc-solve --freecells-num 3 -- --method random-dfs -sp r:tf -to '[0123456789ABCDE]' -seed $seed`;
        if ( $? or $verdict !~ /\AVerdict: Solved / )
        {
            die "failed '$l'";
        }
        print "\t$verdict";
    }
    else
    {
        die "Improper line '$l'";
    }
}
