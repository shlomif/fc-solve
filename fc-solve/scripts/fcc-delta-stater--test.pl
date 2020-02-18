#!/usr/bin/perl

use strict;
use warnings;
use autodie;

# use Test::More tests => 56;
# use Test::Differences qw/ eq_or_diff /;

use FC_Solve::DeltaStater                 ();
use FC_Solve::DeltaStater::BitReader      ();
use FC_Solve::DeltaStater::BitWriter      ();
use FC_Solve::DeltaStater::DeBondt        ();
use FC_Solve::DeltaStater::FccFingerPrint ();
use FC_Solve::VarBaseDigitsReader         ();
use FC_Solve::VarBaseDigitsReader::XS     ();
use FC_Solve::VarBaseDigitsWriter         ();
use FC_Solve::VarBaseDigitsWriter::XS     ();

my $RANK_J = 11;
my $RANK_Q = 12;
my $RANK_K = 13;

sub mytest
{
    my $DEAL_IDX = shift;

    # MS Freecell No. 982 Initial state.
    my $delta = FC_Solve::DeltaStater::FccFingerPrint->new(
        {
                  init_state_str => "Foundations: H-0 C-0 D-0 S-0\n"
                . "Freecells:\n"
                . (
                scalar(`pi-make-microsoft-freecell-board -t "$DEAL_IDX"`) =~
                    s/^/: /gmrs
                )
        }
    );

    # TEST
    # ok( $delta, 'Object was initialized correctly.' );

    open my $exe_fh,
qq#pi-make-microsoft-freecell-board -t "$DEAL_IDX" | fc-solve -sam -sel -p -t -l lg|#;
    while ( my $l = <$exe_fh> )
    {
        if ( $l =~ /\AFoundations:/ )
        {
            while ( ( my $m = <$exe_fh> ) =~ /\S/ )
            {
                $l .= $m;
            }
            if ( $l !~ /TD/ )
            {
                # $DB::single = 1;

                # ...;
            }
            $delta->set_derived(
                {
                    state_str => $l,
                }
            );
            if ( $delta->_derived_state->to_string !~ /TD/ )
            {
                # $DB::single = 1;

                # ...;
            }
            my @x =
                map {
                [
                    ( join "|", map { sprintf "%.2X", ord($_) } split //, $_ ),
                    length
                ]
                } @{ $delta->encode_composite() };
            die if @x != 2;

            print( $DEAL_IDX , ":", join( ",", map { $_->[0] } @x ) );
            print "\n";
            if ( $x[1][1] > 10 )
            {
                $DB::single = 1;
                die "exceeded len in deal $DEAL_IDX";
            }
        }
    }
}

foreach my $DEAL_IDX ( 1 .. 300 )
{
    mytest($DEAL_IDX);
}

__END__

=head1 COPYRIGHT AND LICENSE

This file is part of Freecell Solver. It is subject to the license terms in
the COPYING.txt file found in the top-level directory of this distribution
and at http://fc-solve.shlomifish.org/docs/distro/COPYING.html . No part of
Freecell Solver, including this file, may be copied, modified, propagated,
or distributed except according to the terms contained in the COPYING file.

Copyright (c) 2011 Shlomi Fish

=cut
