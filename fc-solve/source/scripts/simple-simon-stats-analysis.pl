#!/usr/bin/perl

use strict;
use warnings;

use Statistics::Descriptive;

my @buckets_ids = (qw(solved unsolvable));
my %buckets =
(
    (map { $_ => Statistics::Descriptive::Full->new() } @buckets_ids)
);

my $filename = shift(@ARGV);
open my $in, "<", $filename
    or die "Cannot open file '$filename'";

my @stuck;
while (!eof($in))
{
    # @l = Lines.
    my @l = map { scalar(<$in>) } (1 .. 4);
    chomp(@l);

    my $b;
    if ($l[1] =~ m{\AI could not solve this game})
    {
        $b = "unsolvable";
    }
    elsif ($l[1] =~ m{\AThis game is solveable})
    {
        $b = "solved";
    }
    else
    {
        die "Unknown bucket with line '$l[1]' at game No. '$l[0]'!";
    }

    if ($l[2] !~ m{\ATotal number of states checked is (\d+)\.})
    {
        die "Unknown line format at game No. '$l[0]'"
    }
    my $stat = $1;

    if ($stat == 1_500_000)
    {
        push @stuck, $l[0];
    }
    else
    {
        $buckets{$b}->add_data($stat);
    }
}
close($in);

foreach my $b (@buckets_ids)
{
    my $bucket = $buckets{$b};
    print "$b\n";
    print "--------------\n\n";
    print "Count: ", $bucket->count(), "\n";
    print "Mean: ", $bucket->mean(), "\n";
    print "StdDev: ", $bucket->standard_deviation(), "\n";
    print "Median: ", $bucket->median(), "\n";
    print "Min: ", $bucket->min(), "\n";
    print "Max: ", $bucket->max(), "\n";

    my @limits = (100, 1_000, 10_000, 100_000, 1_000_000, 1_500_000);
    my $f = $bucket->frequency_distribution_ref(\@limits);
    foreach my $l (@limits)
    {
        print "Up to $l: $f->{$l} ("
            . sprintf("%.2f%%", $f->{$l}*100/$bucket->count())
            . ")\n"
            ;
    }
    print "\n\n";
}

print "Stuck:\n--------------\n" . join(",", @stuck) . "\n";
