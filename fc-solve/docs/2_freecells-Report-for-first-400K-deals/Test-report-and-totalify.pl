#!/usr/bin/perl

use strict;
use warnings;

use IO::All;

my $text = io->file("Report-2fc-freecell-solvability.txt")->slurp();

print $text;

my $total_solved = 0;
my $total_imp = 0;
my $total_intr = 0;

sub commify {
    my ( $sign, $int, $frac ) = ( $_[0] =~ /^([+-]?)(\d*)(.*)/ );
    my $commified = (
        scalar reverse join ',',
        unpack '(A3)*',
        scalar reverse $int
    );
    return $sign . $commified . $frac;
}

pos($text) = 0;
while (
    $text =~ m{\G\n*([\d,]+) - ([\d,]+):\n-+\n\nSolved: ([\d,]+) ; Impossible: ([\d,]+) ; Intractable: ([\d+,]+)(?: \([^\)]+\))? \.}gms
)
{
    my ($start, $end, $solved, $impossible, $intractable) = ($1, $2, $3, $4, $5);
    tr/,//d for $start, $end, $solved, $impossible, $intractable;

    if ($solved+$impossible+$intractable != $end-$start+1)
    {
        die "Mismatch : ($start, $end, $solved, $impossible, $intractable)";
    }
    $total_solved += $solved;
    $total_imp += $impossible;
    $total_intr += $intractable;
}

print "\nTotal:\n------\nSolved: @{[commify($total_solved)]} ; Impossible: @{[commify($total_imp)]} ; Intractable: @{[commify($total_intr)]} .\n\n";
