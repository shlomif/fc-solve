#!/usr/bin/perl

use strict;
use warnings;

use Test::More tests => 9;

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
        is ($queue->get_num_inserted(), 0, "No items were inserted yet.");
        # TEST:$c++
        is ($queue->get_num_items_in_queue(), 0, "No items in queue.");

        $queue->insert(1);
        
        # TEST:$c++
        is ($queue->get_num_inserted(), 1, "1 item.");
        # TEST:$c++
        is ($queue->get_num_items_in_queue(), 1, "1 items in queue.");

        $queue->insert(200);
        
        # TEST:$c++
        is ($queue->get_num_inserted(), 2, "2 item.");
        # TEST:$c++
        is ($queue->get_num_items_in_queue(), 2, "2 items in queue.");

        $queue->insert(33);
        # TEST:$c++
        is ($queue->get_num_inserted(), 3, "3 item.");
        # TEST:$c++
        is ($queue->get_num_items_in_queue(), 3, "3 items in queue.");
    }
}
# TEST:$run_queue_tests=$c;

# TEST*$run_queue_tests
run_queue_tests('Perl queue');
