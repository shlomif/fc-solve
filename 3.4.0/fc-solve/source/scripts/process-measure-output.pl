#!/usr/bin/perl

use strict;
use warnings;

my @depths;
my $depth;
my $depth_sum = 0;
my $depth_count = 0;
while (my $line = <ARGV>)
{
    chomp($line);
    if ($line =~ m{\ADepth == (\d+)\z})
    {
        my $new_depth = $1;
        if (defined($depth))
        {
            print "$depth " . ($depth_sum / $depth_count) . "\n";
            $depth_sum = $depth_count = 0;
        }
        $depth = $new_depth;
    }
    elsif ($line =~ m{\Aboard\[\d+\]\.iters == (\d+)\z})
    {
        $depth_count++;
        $depth_sum += $1;    
    }
}
print "$depth " . ($depth_sum / $depth_count) . "\n";
