#!/usr/bin/perl

use strict;
use warnings;

my $last_reached = 0;
my $reached;
my $in_collection;

sub print_tsv_line_generic
{
    my $callback = shift;
    my $hash_ref = shift;

    my @vals;
    foreach my $key ( 'Iterations', 'InQueue', )
    {
        my $val = $callback->( $hash_ref, $key );
        if ( !defined($val) )
        {
            die "Hash reference does not contain '$key'";
        }
        push @vals, $val;
    }
    print join( "\t", @vals ), "\n";
}

print_tsv_line_generic( sub { my ( $hash_ref, $key ) = @_; return $key }, {} );

my $is_finished = 0;

my $LOG2_BASER = 1 / log(2);

LINES:
while ( my $l = <> )
{
    chomp($l);
    if ( my ( $func, $stage ) =
        $l =~
m{\A(instance_run_all_threads|instance_run_solver_thread|handle_and_destroy_instance_solution) (start|end)\z}
        )
    {
        if (    $func eq 'handle_and_destroy_instance_solution'
            and $stage eq 'start' )
        {
            $is_finished = 1;
        }
        next LINES;
    }
    elsif ( $l eq 'Could not solve successfully.' )
    {
        next LINES;
    }
    elsif ( $l =~
m{\A(?:Running threads for|Finished running threads for|Start mark-and-sweep cleanup for|Finish mark-and-sweep cleanup for) curr_depth=[0-9]+\z}
        )
    {
        next LINES;
    }
    elsif ( $l =~ m{\AMark\+Sweep Progress - [0-9]+/[0-9]+\z} )
    {
        next LINES;
    }
    elsif ( $l =~ m{\A[0-9]+\.[0-9]+user\s+[0-9]+\.[0-9]+system} )
    {
        next LINES;
    }
    elsif ( $l =~ m{inputs.*?outputs.*?major.*?minor} )
    {
        next LINES;
    }

    if (
        not( ( $reached, $in_collection ) =
            $l =~
m{\AReached ([0-9]+) ; States-in-collection: ([0-9]+) ; Time: [0-9]+\.[0-9]+\z}
        )
        )
    {
        die "Wrong syntax in line <<$l>>.";
    }

    if ( ( $last_reached + 100_000 != $reached ) && ( !$is_finished ) )
    {
        die "Reached on line <<$l>> is wrong.";
    }

    # Skip a line
    scalar <>;

    if ($reached)
    {
        my $in_queue = $in_collection - $reached;
        print "$reached\t$in_queue\n";
    }
}
continue
{
    if ( defined($reached) )
    {
        $last_reached = $reached;
    }
}

__END__

=head1 COPYRIGHT AND LICENSE

This file is part of Freecell Solver. It is subject to the license terms in
the COPYING.txt file found in the top-level directory of this distribution
and at http://fc-solve.shlomifish.org/docs/distro/COPYING.html . No part of
Freecell Solver, including this file, may be copied, modified, propagated,
or distributed except according to the terms contained in the COPYING file.

Copyright (c) 2012 Shlomi Fish

=cut
