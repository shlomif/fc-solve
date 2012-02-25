#!/usr/bin/perl

use strict;
use warnings;

use IO::All;
use List::Util qw();
use List::MoreUtils qw();
use List::UtilsBy qw(max_by);

my $start_scan = 1;
my $end_scan = 27;
my $num_boards = 32_000;
my $max_num_solvable_boards = $num_boards - 1; # The notorious 11,982.

my $ITERS_IDX = 0;
my $TIME_DELTA_IDX = 1;
my $STATUS_IDX = 2;

my @scans = (map { +{'id' => $_ } } ($start_scan .. $end_scan));
foreach my $scan (@scans)
{
    my $scan_id = $scan->{id};
    print "Inputting Scan $scan_id\n";
    # Read the file.
    my @lines = `unxz < wallclock-timings/$scan_id.dump.xz`;
    chomp(@lines);
    my $last_iters = 0;
    my ($last_time) = ($lines[0] =~ m/at (\d+\.\d+)/);
    my @scan_recs;
    my $status = 1;
    LINES_LOOP:
    foreach my $l_i (1 .. $#lines)
    {
        my $line = $lines[$l_i];
        if ($line =~ m/\A(?:Unsolved|Intractable)/)
        {
            $status = 0;
            next LINES_LOOP;
        }
        my ($new_time, $new_iters) = 
            ($line =~ m/at (\d+\.\d+)\s+\(total_num_iters=(\d+)\)/);
        
        push @scan_recs, [$new_iters-$last_iters, $new_time-$last_time, $status];
        
        ($last_time, $last_iters) = ($new_time, $new_iters);
        $status = 1;
    }
    if (@scan_recs != $num_boards)
    {
        die "Wrong number of records in scan '$scan_id'!";
    }
    $scan->{r} = \@scan_recs;
}

my @duration_quotas = (map { 0.001 } (1 .. 1000));

my $num_boards_solved = 0;

my $quota = 0;
QUOTAS:
while ($num_boards_solved < $max_num_solvable_boards)
{
    $quota += shift(@duration_quotas);

    my @max_scans =
        max_by { scalar grep { $_->[$STATUS_IDX] && ($_->[$TIME_DELTA_IDX] < $quota) } @{$_->{r}} } @scans;

    my $max_scan;
    if (! @max_scans)
    {
        next QUOTAS;
    }
    elsif (1 == @max_scans)
    {
        ($max_scan) = @max_scans;
    }
    else
    {
        # Tie breaker.
        $max_scan = min_by { max_by { $_->[$STATUS_IDX] ? $_->[$TIME_DELTA_IDX] : 0; } @{$_->{r}} } @max_scans;
    }
}

