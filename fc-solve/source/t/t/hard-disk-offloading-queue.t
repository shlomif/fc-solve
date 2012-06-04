#!/usr/bin/perl

use strict;
use warnings;

use Test::More tests => 2262;

use File::Spec;

use File::Path qw(mkpath);

use lib './t/lib';
use Games::Solitaire::FC_Solve::QueuePrototype;
use Games::Solitaire::FC_Solve::QueueInC;

my $queue_offload_dir_path = File::Spec->catdir(
    File::Spec->curdir(), "queue-offload-dir"
);
mkpath($queue_offload_dir_path);

# TEST:$c=0;
sub run_queue_tests
{
    my ($blurb_base, $class_name) = @_;

    {
        my $queue = $class_name->new(
            {
                num_items_per_page => 10,
                offload_dir_path => $queue_offload_dir_path,
            }
        );

        # TEST:$c++;
        ok ($queue, "$blurb_base - Queue was initialized.");

        # TEST:$c++
        is ($queue->get_num_inserted(), 0, "$blurb_base - No items were inserted yet.");
        # TEST:$c++
        is ($queue->get_num_items_in_queue(), 0, "$blurb_base - No items in queue.");

        # TEST:$c++
        is ($queue->get_num_extracted(), 0, "$blurb_base - no items extracted.");

        $queue->insert(1);

        # TEST:$c++
        is ($queue->get_num_inserted(), 1, "$blurb_base - 1 item.");
        # TEST:$c++
        is ($queue->get_num_items_in_queue(), 1, "$blurb_base - 1 items in queue.");

        $queue->insert(200);

        # TEST:$c++
        is ($queue->get_num_inserted(), 2, "$blurb_base - 2 item.");
        # TEST:$c++
        is ($queue->get_num_items_in_queue(), 2, "$blurb_base - 2 items in queue.");

        $queue->insert(33);
        # TEST:$c++
        is ($queue->get_num_inserted(), 3, "$blurb_base - 3 item.");
        # TEST:$c++
        is ($queue->get_num_items_in_queue(), 3, "$blurb_base - 3 items in queue.");
        # TEST:$c++
        is ($queue->get_num_extracted(), 0, "$blurb_base - no items extracted.");


        # TEST:$c++
        is (scalar($queue->extract()), 1, "$blurb_base - Extracted 1 from queue.");

        # TEST:$c++;
        is ($queue->get_num_inserted(), 3, "$blurb_base - 3 Items were inserted so far.");

        # TEST:$c++;
        is ($queue->get_num_items_in_queue(), 2, "$blurb_base - 2 items in queue (after one extracted.");

        # TEST:$c++
        is ($queue->get_num_extracted(), 1, "$blurb_base - 1 item was extracted.");

        # TEST:$c++
        is (scalar($queue->extract()), 200, "$blurb_base - Extracted 1 from queue.");

        # TEST:$c++;
        is ($queue->get_num_inserted(), 3, "$blurb_base - 3 Items were inserted so far.");

        # TEST:$c++;
        is ($queue->get_num_items_in_queue(), 1, "$blurb_base - 1 items in queue (after two extracted.)");

        # TEST:$c++
        is ($queue->get_num_extracted(), 2, "$blurb_base - 2 items were extracted.");


        # Now trying to add an item after a few were extracted and see how
        # the statistics are affected.
        $queue->insert(4);

        # TEST:$c++;
        is ($queue->get_num_inserted(), 4, "$blurb_base - 4 Items were inserted so far.");

        # TEST:$c++;
        is ($queue->get_num_items_in_queue(), 2, "$blurb_base - 2 items in queue (after two extracted and one added.)");

        # TEST:$c++
        is ($queue->get_num_extracted(), 2, "$blurb_base - 2 items were extracted.");
    }

    {
        my $queue = $class_name->new(
            {
                num_items_per_page => 10,
                offload_dir_path => $queue_offload_dir_path,
            }
        );

        my $map_idx_to_item = sub { my ($idx) = @_; return $idx * 3 + 1; };

        # TEST:$num_items=1000;
        foreach my $item_idx (1 .. 1_000)
        {
            $queue->insert($map_idx_to_item->($item_idx));
        }

        foreach my $item_idx (1 .. 1_000)
        {
            # TEST:$c=$c+$num_items;
            is (scalar( $queue->extract() ), $map_idx_to_item->($item_idx),
                "$blurb_base - Testing the extraction of item no. $item_idx"
            );
        }

        # Now let's test the final statistics.

        # TEST:$c++;
        is ($queue->get_num_inserted(), 1_000, "$blurb_base - 1,000 items were inserted");

        # TEST:$c++;
        is ($queue->get_num_items_in_queue(), 0, "$blurb_base - 0 items in queue.");

        # TEST:$c++
        is ($queue->get_num_extracted(), 1_000, "$blurb_base - 1,000 items were extracted in total.");

        # Now let's add more items after the queue is empty.

        # TEST:$num_items=100;
        foreach my $item_idx (1 .. 100)
        {
            $queue->insert($map_idx_to_item->($item_idx));
        }

        # TEST:$c++;
        is ($queue->get_num_inserted(), 1_100, "$blurb_base - 1,100 items were inserted");

        # TEST:$c++;
        is ($queue->get_num_items_in_queue(), 100, "$blurb_base - 100 items in queue.");

        # TEST:$c++
        is ($queue->get_num_extracted(), 1_000, "$blurb_base - 1,000 items were extracted in total.");

        foreach my $item_idx (1 .. 100)
        {
            # TEST:$c=$c+$num_items;
            is (scalar( $queue->extract() ), $map_idx_to_item->($item_idx),
                "$blurb_base - Testing the extraction of item no. $item_idx"
            );
        }

        # TEST:$c++;
        is ($queue->get_num_inserted(), 1_100, "$blurb_base - 1,100 items were inserted");

        # TEST:$c++;
        is ($queue->get_num_items_in_queue(), 0, "$blurb_base - 100 items in queue.");

        # TEST:$c++
        is ($queue->get_num_extracted(), 1_100, "$blurb_base - 1,100 items were extracted in total.");

    }

    return;
}
# TEST:$run_queue_tests=$c;

# TEST*$run_queue_tests
run_queue_tests('Perl queue', 'Games::Solitaire::FC_Solve::QueuePrototype');
# TEST*$run_queue_tests
run_queue_tests('C queue', 'Games::Solitaire::FC_Solve::QueueInC');
