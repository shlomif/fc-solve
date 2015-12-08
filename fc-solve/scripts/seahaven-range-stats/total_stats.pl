#!/usr/bin/perl

use strict;

my @limits = qw(100 1000 10000 100000 1000000 2000000 10000000);

my $total_num = 0;

my $max_num_iterations = 1_500_000;

my %stats =
(
    'solved' =>
    {
        'all' => [],
        'all_iters' => [],
        'stats' => [ (map { [] } @limits) ],
        'sum' => 0,
        'sum_squares' => 0,
    },
    'not_solved' =>
    {
        'all' => [],
        'all_iters' => [],
        'stats' => [ (map { [] } @limits) ],
        'sum' => 0,
        'sum_squares' => 0,
    },
    'stuck' => [],
);

open I, "<total_dump.txt";

my ($line);

while (!eof(I))
{
    if ($line !~ /^SEED=(\d+)/)
    {
        $line = <I>;
    }
    if ($line =~ /^SEED=(\d+)/)
    {
        my $num = $1;
        $total_num += 1;
        $line = <I>;
        while ($line =~ /^The number of iterations/)
        {
        	$line = <I>;
        }
        my $which;
        if ($line =~ /^This game is solveable/)
        {
            $which = $stats{'solved'};
        }
        elsif ($line =~ /^I could not solve this game/)
        {
            $which = $stats{'not_solved'};
        }
        else
        {
            push @{$stats{'stuck'}}, $num;
            next;
        }
        $line = <I>;
        $line =~ /(\d+)/;
        my $num_iters = $1;
        if ($num_iters == $max_num_iterations)
        {
            push @{$stats{'stuck'}}, $num;
            next;
        }

        push @{$which->{'all'}}, $num;
        my $bucket = 0;
        while ($limits[$bucket] < $num_iters)
        {
            $bucket++;
        }
        push @{$which->{'stats'}->[$bucket]}, $num;
        $which->{'sum'} += $num_iters;
        $which->{'sum_squares'} += ($num_iters ** 2);
        push @{$which->{'all_iters'}}, $num_iters;
    }
}

close(I);

print "Total Number of Games Checked: $total_num\n";
print "\n";

foreach my $is_solved (1, 0)
{
    my $which = $stats{$is_solved ? "solved" : "not_solved"};
    $which->{'all_iters'} = [ (sort { $a <=> $b } @{$which->{'all_iters'}}) ];
    print ($is_solved ? "Solved:\n-------\n\n" : "Not-Solved:\n-----------\n\n");


    my $num = scalar(@{$which->{'all'}});

    print "Num: " . $num . " (" . sprintf("%.1f", ($num/$total_num*100)) . "%)\n";
    print "Average Iters Num.: " . sprintf("%.2f", ($which->{'sum'}/$num)) . "\n";
    print "Median Iters Num.: " . $which->{'all_iters'}->[int($num/2)] . "\n";
    print "Std. Dev.: " . sprintf("%.2f", sqrt($which->{'sum_squares'}/$num - ($which->{'sum'}/$num)**2)) . "\n";
    for my $bucket (0 .. $#limits)
    {
        my $num_in_bucket = scalar(@{$which->{'stats'}->[$bucket]});
        print "In the range " .
            sprintf("%8s", (($bucket == 0) ? 0 : $limits[$bucket-1])) . " - " .
            sprintf("%8s", $limits[$bucket]) . " : " .
            sprintf("%5s", $num_in_bucket) .
            " (" . sprintf("%5.1f", ($num_in_bucket/$num*100)) . "%)"."\n";
    }
    print "\n\n";
}

print "Stuck:\n------\n\n" . scalar(@{$stats{'stuck'}}) . " (" . join(", ", @{$stats{'stuck'}}) . ")\n";

#use Data::Dumper;
#
#my $d = Data::Dumper->new([ \%stats], [ "\$stats" ]);
#print $d->Dump();


