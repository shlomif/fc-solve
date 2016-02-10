#!/usr/bin/perl

# This script looks for states in the solution with many empty columns,
# as potential ones to start from.

use strict;
use warnings;

use IO::All;

my $fh = io->file("dump.txt");

my $line_num = 0;

sub _read_line
{
    $line_num++;
    return $fh->getline();
}

while (my $line = _read_line())
{
    if ($line =~ m{\A: })
    {
        my @l = ($line);

        while (($line = _read_line()) && $line =~ m{\A: })
        {
            push @l, $line;
        }

        my @empties = grep { /\A:\s*\z/ } @l;
        if (@empties >= 3)
        {
            print "Two empty columns at Line # $line_num\n";
        }
    }
}
continue
{
}
