#!/usr/bin/perl

use strict;
use warnings;

use Test::More tests => 4;

use File::Spec;

use File::Path qw(mkpath);

use lib './t/lib';
use FC_Solve::DepthMultiQueuePrototype;
# use Games::Solitaire::FC_Solve::QueueInC;

my $queue_offload_dir_path = File::Spec->catdir(
    File::Spec->curdir(), "depth-multi-queue-offload-dir"
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
                first_depth => 100,
                first_item => 1,
            }
        );

        # TEST:$c++;
        ok ($queue, "$blurb_base - Queue was initialized.");

        # TEST:$c++
        is ($queue->get_num_inserted(), 1, "$blurb_base - One item inserted.");
        # TEST:$c++
        is ($queue->get_num_items_in_queue(), 1, "$blurb_base - One item in queue.");

        # TEST:$c++
        is ($queue->get_num_extracted(), 0, "$blurb_base - no items extracted.");
    }

    return;
}
# TEST:$run_queue_tests=$c;

# TEST*$run_queue_tests
run_queue_tests('Perl queue', 'FC_Solve::DepthMultiQueuePrototype');
#====TEST*$run_queue_tests
# run_queue_tests('C queue', 'Games::Solitaire::FC_Solve::QueueInC');
