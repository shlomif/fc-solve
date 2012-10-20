#!/usr/bin/perl

use strict;
use warnings;

use IO::All;
use List::MoreUtils qw(none);

foreach my $scan (io("../scans.txt")->chomp->getlines())
{
    if (my ($l) = $scan =~ /\t(.*)/)
    {
        if (none { /\t\Q$l\E\z/ } (io("scans.txt")->chomp->getlines()))
        {
            system("perl time-scan.pl $l");
        }
    }
}
