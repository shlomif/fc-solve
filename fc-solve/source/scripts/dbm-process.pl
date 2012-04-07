#!/usr/bin/perl

use strict;
use warnings;

my %found;
while (my $line = <ARGV>)
{
    if ($line =~ /\A([0-9A-F]+)\|/)
    {
        my $id = $1;
        if (! exists($found{$id}))
        {
            print $line;
            $found{$id} = undef();
        }
    }
}
