#!/usr/bin/perl

use strict;
use warnings;

use Statistics::Descriptive;

my $num_fields = 2;

my @stats = 
    (map 
        { Statistics::Descriptive::Full->new(); } 
        (1 .. $num_fields)
    );

while(my $line = <ARGV>)
{
    chomp($line);
    my @vals = split(/\t/, $line);
    foreach my $f (0 .. $num_fields-1)
    {
        $stats[$f]->add_data($vals[$f]);
    }
}

foreach my $f (0 .. $num_fields-1)
{
    my $s = $stats[$f];
    print "Field No. $f\n";
    print "---------------------------\n";
    print "Min: " , $s->min(), "\n";
    print "Max: " , $s->max(), "\n";
    print "Average: " , $s->mean(), "\n";
    print "StdDev: " , $s->standard_deviation(), "\n";
    print "Median: " , $s->median(), "\n";
    print "\n";
}
