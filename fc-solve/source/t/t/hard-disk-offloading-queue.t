#!/usr/bin/perl

use strict;
use warnings;

use Test::More tests => 12;

use File::Spec;

use File::Path qw(mkpath);

use lib './t/lib';
use Games::Solitaire::FC_Solve::QueuePrototype;

my $queue_offload_dir_path = File::Spec->catdir(
    File::Spec->curdir(), "queue-offload-dir"
);
mkpath($queue_offload_dir_path);

# TEST:$c=0;
sub run_queue_tests
{
    my ($blurb_base) = @_;

    {
        my $queue = Games::Solitaire::FC_Solve::QueuePrototype->new(
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
        is (scalar($queue->extract()), 1, "$blurb_base - Extracted 1 from queue.");

        # TEST:$c++;
        is ($queue->get_num_inserted(), 3, "$blurb_base - 3 Items were inserted so far.");

        # TEST:$c++;
        is ($queue->get_num_items_in_queue(), 2, "$blurb_base - 2 items in queue (after one extracted.");

        # TODO : Add get_num_extracted().
    }
}
# TEST:$run_queue_tests=$c;

# TEST*$run_queue_tests
run_queue_tests('Perl queue');
