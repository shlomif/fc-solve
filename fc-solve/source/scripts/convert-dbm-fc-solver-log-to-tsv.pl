#!/usr/bin/perl

use strict;
use warnings;

my $line_idx = 1;
my $last_reached = 0;
my $reached;
my $time;
my $last_time;
my $in_collection;
my $last_in_collection;
my $ratio;
my $last_ratio;
my $in_queue;
my $last_in_queue;

sub print_tsv_line_generic
{
    my $callback = shift;
    my $hash_ref = shift;

    my @vals;
    foreach my $key ('Iterations', 'InColl', 'InQueue', 'Time', 'TimeDelta', 'TimeDeltaByInColl', 'LogTimeDelta', 'LogTimeDeltaByInColl', 'InQueueVsInCollRatio', 'RatioLog', 'RatioDelta', 'QueueDelta', )
    {
        my $val = $callback->($hash_ref,$key);
        if (! defined($val))
        {
            die "Hash reference does not contain '$key'";
        }
        push @vals, $val;
    }
    print join("\t", @vals), "\n";
}

print_tsv_line_generic(sub { my ($hash_ref, $key) = @_; return $key }, {});

my $is_finished = 0;

my $LOG2_BASER = 1/log(2);

LINES:
while(my $l = <>)
{
    chomp($l);
    if (my ($func, $stage) = $l =~ m{\A(instance_run_all_threads|instance_run_solver_thread|handle_and_destroy_instance_solution) (start|end)\z})
    {
        if ($func eq 'handle_and_destroy_instance_solution'
                and $stage eq 'start')
        {
            $is_finished = 1;
        }
        next LINES;
    }
    elsif ($l eq 'Could not solve successfully.')
    {
        next LINES;
    }
    elsif ($l =~ m{\A(?:Running threads for|Finished running threads for|Start mark-and-sweep cleanup for|Finish mark-and-sweep cleanup for) curr_depth=\d+\z})
    {
        next LINES;
    }
    elsif ($l =~ m{\AMark\+Sweep Progress - \d+/\d+\z})
    {
        next LINES;
    }


    if (not (($reached, $in_collection, $time) = $l =~
            m{\AReached (\d+) ; States-in-collection: (\d+) ; Time: (\d+\.\d+)\z}))
    {
        die "Wrong syntax in line $line_idx Line is <<$l>>.";
    }

    if (($last_reached + 100_000 != $reached) && (!$is_finished))
    {
        die "Reached on line $line_idx is wrong.";
    }

    my $queue_l = <>;
    chomp($queue_l);
    $line_idx++;

    $in_queue = $in_collection-$reached;

    my $expected_queue_l = ">>>Queue Stats: inserted=$in_collection items_in_queue=$in_queue extracted=$reached";
    if (0) # if ($queue_l ne $expected_queue_l)
    {
        die "Wrong queue line on line No. $line_idx . Expected: <<<$expected_queue_l>>> - Got: <<<$queue_l>>>";
    }

    $ratio = $in_queue / $in_collection;

    if (defined($last_time))
    {
        my $time_delta = $time-$last_time;
        my $time_delta_by_in_coll = $time_delta/($in_collection-$last_in_collection);
        my $ratio_log = $ratio ? (log($ratio)*$LOG2_BASER) : 0;

        print_tsv_line_generic(
            sub { my ($hash_ref, $key) = @_; return $hash_ref->{$key} },
            {
                Iterations => $reached,
                InColl => $in_collection,
                InQueue => $in_queue,
                Time => $time,
                TimeDelta => $time_delta,
                TimeDeltaByInColl => $time_delta_by_in_coll,
                LogTimeDelta => (log($time_delta)/log(2)),
                LogTimeDeltaByInColl => (log($time_delta_by_in_coll)/log(2)),
                InQueueVsInCollRatio => $ratio,
                RatioLog => $ratio_log,
                RatioDelta => ($last_ratio - $ratio),
                QueueDelta => ($in_queue - $last_in_queue),
            }
        );
    }
}
continue
{
    $line_idx++;
    if (defined($reached))
    {
        $last_reached = $reached;
    }
    if (defined($time))
    {
        $last_time = $time;
    }
    if (defined($in_collection))
    {
        $last_in_collection = $in_collection;
    }
    if (defined($ratio))
    {
        $last_ratio = $ratio;
    }
    if (defined($in_queue))
    {
        $last_in_queue = $in_queue;
    }
}

=head1 COPYRIGHT & LICENSE

Copyright 2012 by Shlomi Fish

This program is distributed under the MIT (X11) License:
L<http://www.opensource.org/licenses/mit-license.php>

Permission is hereby granted, free of charge, to any person
obtaining a copy of this software and associated documentation
files (the "Software"), to deal in the Software without
restriction, including without limitation the rights to use,
copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the
Software is furnished to do so, subject to the following
conditions:

The above copyright notice and this permission notice shall be
included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
OTHER DEALINGS IN THE SOFTWARE.

=cut
