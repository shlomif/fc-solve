#!/usr/bin/perl

# Autopmove / autoplay / prune a deal. It accepts the same command line
# arguments as verify-solitaire-solution so you should use it something like that:
#
# perl scripts/horne-autoplay-board.pl --freecells-num 2 -
#
use strict;
use warnings;
use autodie;

package Games::Solitaire::Verify::App::HorneAutomove;

use parent 'Games::Solitaire::Verify::App::CmdLine';

use Games::Solitaire::Verify::State              ();
use Games::Solitaire::Verify::HorneAutomovePrune ();

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
        open $fh, "<", $filename;
    }
    my $s = do { local $/; <$fh> };
    close($fh);
    $s =~ s/^(\w)/: $1/gms;
    $s = "Freecells:\n$s" if ( $s !~ /^Freecells/ms );
    $s = "Foundations: H-0 C-0 D-0 S-0\n$s" if ( $s !~ /^Found/ms );

    my $running_state = Games::Solitaire::Verify::State->new(
        {
            variant        => "custom",
            variant_params => $self->_variant_params(),
            string         => $s,
        },
    );

    Games::Solitaire::Verify::HorneAutomovePrune::do_prune(
        {
            state        => $running_state,
            output_state => sub { },
            output_move  => sub { },
        }
    );
    print $running_state->to_string();

    return;
}

package main;

Games::Solitaire::Verify::App::HorneAutomove->new( { argv => [@ARGV] } )->run();

__END__

=head1 COPYRIGHT AND LICENSE

This file is part of Freecell Solver. It is subject to the license terms in
the COPYING.txt file found in the top-level directory of this distribution
and at http://fc-solve.shlomifish.org/docs/distro/COPYING.html . No part of
Freecell Solver, including this file, may be copied, modified, propagated,
or distributed except according to the terms contained in the COPYING file.

Copyright (c) 2012 Shlomi Fish

=cut
