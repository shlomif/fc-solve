#!/usr/bin/perl

use strict;
use warnings;

use Test::More tests => 1;

use File::Spec;

use File::Path qw(mkpath);

use lib './t/lib';
use Games::Solitaire::FC_Solve::QueuePrototype;

my $queue_offload_dir_path = File::Spec->catdir(File::Spec->curdir(), "queue-offload-dir");
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
        
    }
}
# TEST:$run_queue_tests=$c;

# TEST*$run_queue_tests
run_queue_tests('Perl queue');
