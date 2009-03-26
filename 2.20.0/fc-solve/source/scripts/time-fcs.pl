#!/usr/bin/perl

# This script takes an ./mptest dump (such as:
#
#    ./mptest 1 32000 500 -l gi > dump.txt
#
# And calculates the time elapsing from start to finish.

use strict;
use warnings;

my @lines = <ARGV>;

sub get_time
{
    my $l = shift;
    if ($l =~ m{ at (\d+\.\d+)})
    {
        return $1;
    }
    else
    {
        die "Cannot find a number at line '$l'";
    }
}

print +(get_time($lines[-1]) - get_time($lines[0])), "\n";
