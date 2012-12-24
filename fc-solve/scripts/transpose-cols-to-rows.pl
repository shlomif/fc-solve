#!/usr/bin/perl

use strict;
use warnings;

my @table;
while (my $l = <>)
{
    $l =~ s/10/T/g;
    push @table, [split ' ', $l];
}

use List::Util qw(max);

print
    map
    {
        my $i = $_;
        ": " . join(
            ' ',
            map { $_->[$i] ? $_->[$i] : () } @table
        )
        . "\n"
    } (0 .. max(map { scalar(@$_) } @table)-1);
