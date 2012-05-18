#!/usr/bin/perl

use strict;
use warnings;

use lib './t/t/lib';

package RandGen;

use base 'Games::ABC_Path::MicrosoftRand';

sub rand30
{
    my $self = shift;

    my $one = $self->rand;
    my $two = $self->rand;

    return ($one | ($two << 15));
}

package main;


use File::Spec;
use File::Path qw( mkpath );

use Data::Dump qw(dd);

use Games::Solitaire::FC_Solve::QueuePrototype;
use Games::Solitaire::FC_Solve::QueueInC;

my ($items_per_page, $data_seed, $interval_seed) = @ARGV;

my $input_gen =  RandGen->new(seed => $data_seed);
my $output_gen = RandGen->new(seed => $data_seed);
my $interval_gen = RandGen->new(seed => $interval_seed);

my $queue_offload_dir_path = File::Spec->catdir(
    File::Spec->curdir(), "queue-offload-dir"
);
mkpath($queue_offload_dir_path);

my $class = $ENV{'USE_C'} ? 'Games::Solitaire::FC_Solve::QueueInC' :
    'Games::Solitaire::FC_Solve::QueuePrototype';

my $queue = $class->new(
    {
        num_items_per_page => $items_per_page,
        offload_dir_path => $queue_offload_dir_path,
    }
);

while ($queue->get_num_extracted() < 10_000_000)
{
    # Insert some items.
    {
        my $interval = $interval_gen->rand30() % 1_000;

        foreach my $idx (1 .. $interval)
        {
            $queue->insert($input_gen->rand30());
        }
    }

    {
        my $interval = $interval_gen->rand30() % 1_000;

        foreach my $idx (1 .. $interval)
        {
            if (! $queue->get_num_items_in_queue())
            {
                last;
            }
            if ($queue->extract() != $output_gen->rand30())
            {
                print "Problem occured.";
                dd($queue);
                die "Error";
            }
        }
    }
    print "Checkpoint." . $queue->get_num_extracted() . "\n";
}

exit(0);
