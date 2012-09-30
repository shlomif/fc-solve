#!/usr/bin/perl

use strict;
use warnings;

while (my $l = <>)
{
    chomp ($l);
    if ($l =~ m{\AFoundations:})
    {
        my $founds_l = $l;
        my $fc_l = <>;
        chomp($fc_l);
        if ($fc_l !~ m{\AFreecells:(.*)})
        {
            die "Wrong freecells line";
        }
        my $fc_s = $1;
        my @freecells = ($fc_s =~ m/(\S+)/g);
        my $normalized_fc_l = "Freecells: " . join(" ", sort { $a cmp $b } @freecells);

        my @cols;
        COLS:
        while($l = <>)
        {
            chomp($l);
            if ($l =~ m{\A:})
            {
                push @cols, $l;
            }
            else
            {
                last COLS;
            }
        }
        print "]" . (join "", map { "$_\\n" } sort { $a cmp $b } (@cols,$normalized_fc_l, $founds_l)) . "\n";
    }
}
