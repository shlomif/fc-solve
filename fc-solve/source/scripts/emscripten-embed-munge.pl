#!/usr/bin/perl

use strict;
use warnings;
use 5.014;

my $dir = shift(@ARGV);

my @output;
foreach my $fn (@ARGV)
{
    push @output, $fn =~ s#\A(\Q$dir\E(/.*))\z#$1\@$2#r;
}
print join(" ", @output), "\n";
