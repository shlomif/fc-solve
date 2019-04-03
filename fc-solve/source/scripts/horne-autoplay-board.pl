#!/usr/bin/perl

# This program converts the solution output of dbm_fc_solver
# (and depth_dbm_fc_solver) to something that can be used as input for
# verify-solitaire-solution. It accepts the same command line arguments
# as verify-solitaire-solution so you should use it something like that:
#
# perl scripts/convert-dbm-fc-solver-solution-to-fc-solve-solution.pl \
#  --freecells-num 2 -
#
use strict;
use warnings;

package Games::Solitaire::Verify::App::From_DBM_FC_Solver;

use parent 'Games::Solitaire::Verify::App::CmdLine';

use Games::Solitaire::Verify::State ();
use FC_Solve::HorneAutomovePrune    ();

sub _out_running_state
{
    my $running_state = shift;
    print $running_state->to_string();
    return;
}

sub run
{
    my $self = shift;

    my $filename       = $self->_filename();
    my $variant_params = $self->_variant_params();

    my $fh;

    if ( $filename eq "-" )
    {
        $fh = *STDIN;
    }
    else
    {
        open $fh, "<", $filename
            or die "Cannot open '$filename' - $!";
    }

    my $initial_state = Games::Solitaire::Verify::State->new(
        {
            variant        => "custom",
            variant_params => $self->_variant_params(),
            string         => do
            {
                local $/;
                my $s = <$fh>;
                $s =~ s/^(\w)/: $1/gms;
                $s = "Freecells:\n$s" if ( $s !~ /^Freecells/ms );
                $s = "Foundations: H-0 C-0 D-0 S-0\n$s" if ( $s !~ /^Found/ms );
                $s;
            },
        },
    );

    my $running_state = $initial_state->clone();

    FC_Solve::HorneAutomovePrune::_prune_all( $running_state,
        sub { }, sub { }, );
    _out_running_state($running_state);

    close($fh);
    return;
}

package main;

Games::Solitaire::Verify::App::From_DBM_FC_Solver->new( { argv => [@ARGV] } )
    ->run();

__END__

=head1 COPYRIGHT AND LICENSE

This file is part of Freecell Solver. It is subject to the license terms in
the COPYING.txt file found in the top-level directory of this distribution
and at http://fc-solve.shlomifish.org/docs/distro/COPYING.html . No part of
Freecell Solver, including this file, may be copied, modified, propagated,
or distributed except according to the terms contained in the COPYING file.

Copyright (c) 2012 Shlomi Fish

=cut
