#!/usr/bin/perl

use strict;
use warnings;

use IO::All;
use List::Util qw(max);
use List::MoreUtils qw(any);
use List::UtilsBy qw(max_by min_by);

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

my @duration_quotas = (map { 0.0001 } (1 .. 20_000));

my $num_boards_solved = 0;

my $quota = 0;
my @prelude;
QUOTAS:
while (any { any { $_->[$STATUS_IDX] } @{$_->{r}} } @scans)
{
    print "Solved $num_boards_solved\n";
    $quota += shift(@duration_quotas);

    my @max_scans =
        max_by { $_->{max} = scalar grep { 
            $_->[$STATUS_IDX] && ($_->[$TIME_DELTA_IDX] < $quota) 
            } @{$_->{r}} 
        } @scans;

    my $max_scan;
    if ((! @max_scans) || (! $max_scans[0]->{max}))
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
        $max_scan = min_by { 
            max(map { $_->[$STATUS_IDX] ? $_->[$TIME_DELTA_IDX] : 0; } 
            @{$_->{r}})
        } @max_scans;
    }

    my $recs = $max_scan->{r};
    my $iters_count = 
        max(map { $_->[$ITERS_IDX] } grep { 
            $_->[$STATUS_IDX] && ($_->[$TIME_DELTA_IDX] < $quota) 
            } @{$recs}
        );
    my $time_delta = 
        max(map { $_->[$TIME_DELTA_IDX] } grep { 
            $_->[$STATUS_IDX] && ($_->[$TIME_DELTA_IDX] < $quota) 
            } @{$recs}
        );


    push @prelude, { iters => $iters_count, scan => $max_scan->{id}, time_delta => $time_delta,};
    
    my @unsolved_indexes = grep { !($recs->[$_]->[$STATUS_IDX] &&
    ($recs->[$_]->[$ITERS_IDX] <= $iters_count )); } keys(@$recs);

    $num_boards_solved += (@$recs - @unsolved_indexes);

    # Get rid of the old records.
    foreach my $scan (@scans)
    {
        my @r = @{$scan->{r}};
        $scan->{r} = [@r[@unsolved_indexes]];
    }

    # Now update the current scan (we can no longer use $recs because it
    # was replaced.
    foreach my $rec (@{ $max_scan->{r} })
    {
        $rec->[$ITERS_IDX] -= $iters_count;
        $rec->[$TIME_DELTA_IDX] -= $time_delta;
    }

    $quota = 0;
}

sub _map_all_but_last
{
    my ($cb, $arr_ref) = (@_);

    return [ (map {$cb->($_)} @$arr_ref[0 .. $#$arr_ref-1]), $arr_ref->[-1] ];
}

my %s_cmds = (map { split(/\t/,$_) } (io("scans.txt")->chomp->getlines()));

my $prelude_s = join(",", map { sprintf('%d@%s', $_->{iters}, $_->{scan}) } @prelude);

1 while ($prelude_s =~ s{(\d+)\@([^,]+),(\d+)\@\2}{($1 + $3) . '@' . $2}e);

io->file("test.scan")->print(
    "freecell-solver-range-parallel-solve 1 32000 1 \\\n" .
    (join "", map { "$_\\\n" } @{_map_all_but_last (sub { "$_-nst " }, [map { sprintf("    %s -step 500 --st-name %s " , $s_cmds{$_}, $_) } map { $_->{id} } @scans])}) .
    "     --prelude \"$prelude_s\"\n"
);
