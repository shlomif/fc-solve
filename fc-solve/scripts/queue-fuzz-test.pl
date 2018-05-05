#!/usr/bin/perl

# Run this script like this:
#
# [ From the git /source/ sub-dir ]
#
# cd ..
# mkdir B
# cd B
# ../source/Tatzer
# make
# export FCS_PATH="$(pwd)" FCS_SRC_PATH="$(cd ../source && pwd)"
# cd t
#
# perl -I ../../source/t/t/lib ../../source/scripts/queue-fuzz-test.pl 10000 2 100

use strict;
use warnings;

use lib './t/t/lib';

package RandGen;

use parent 'Math::RNG::Microsoft';

sub rand30
{
    my $self = shift;

    my $one = $self->rand;
    my $two = $self->rand;

    return ( $one | ( $two << 15 ) );
}

package main;

use Path::Tiny qw( path );
use Data::Dump qw(dd);

use FC_Solve::QueuePrototype;
use FC_Solve::QueueInC;

my ( $items_per_page, $data_seed, $interval_seed ) = @ARGV;

my $input_gen    = RandGen->new( seed => $data_seed );
my $output_gen   = RandGen->new( seed => $data_seed );
my $interval_gen = RandGen->new( seed => $interval_seed );

my $queue_offload_dir_path = path( $ENV{TMPDIR} )->child("queue-offload-dir");
$queue_offload_dir_path->mkpath;

my $class =
    $ENV{'USE_C'}
    ? 'FC_Solve::QueueInC'
    : 'FC_Solve::QueuePrototype';

my $queue = $class->new(
    {
        num_items_per_page => $items_per_page,
        offload_dir_path   => "$queue_offload_dir_path",
    }
);

while ( $queue->get_num_extracted() < 1_000_000 )
{
    # Insert some items.
    {
        my $interval = $interval_gen->rand30() % 1_000;

        foreach my $idx ( 1 .. $interval )
        {
            $queue->insert( $input_gen->rand30() );
        }
    }

    {
        my $interval = $interval_gen->rand30() % 1_000;

        foreach my $idx ( 1 .. $interval )
        {
            if ( !$queue->get_num_items_in_queue() )
            {
                last;
            }
            if ( $queue->extract() != $output_gen->rand30() )
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

__END__

=head1 COPYRIGHT AND LICENSE

This file is part of Freecell Solver. It is subject to the license terms in
the COPYING.txt file found in the top-level directory of this distribution
and at http://fc-solve.shlomifish.org/docs/distro/COPYING.html . No part of
Freecell Solver, including this file, may be copied, modified, propagated,
or distributed except according to the terms contained in the COPYING file.

Copyright (c) 2014 Shlomi Fish

=cut
