#!/usr/bin/perl

# Autopmove / autoplay / prune a deal. It accepts the same command line
# arguments as verify-solitaire-solution so you should use it something like that:
#
# perl scripts/horne-autoplay-board.pl --freecells-num 2 -
#
use strict;
use warnings;
use autodie;

use Test::More tests => 1;

use Games::Solitaire::Verify::State              ();
use Games::Solitaire::Verify::HorneAutomovePrune ();

sub run
{
    my $s             = shift;
    my $running_state = Games::Solitaire::Verify::State->new(
        {
            variant => "freecell",
            string  => $s,
        },
    );

    Games::Solitaire::Verify::HorneAutomovePrune::do_prune(
        {
            state        => $running_state,
            output_state => sub { },
            output_move  => sub { },
        }
    );
    return $running_state->to_string();
}

my $ms24 = <<'EOF';
Foundations: H-0 C-0 D-0 S-0
Freecells:
: 4C 2C 9C 8C QS 4S 2H
: 5H QH 3C AC 3H 4H QD
: QC 9S 6H 9H 3S KS 3D
: 5D 2S JC 5C JH 6D AS
: 2D KD TH TC TD 8D
: 7H JS KH TS KC 7C
: AH 5S 6S AD 8H JD
: 7S 6C 7D 4D 8S 9D
EOF

my $ms24_pruned = <<'EOF';
Foundations: H-0 C-0 D-0 S-A
Freecells:
: 4C 2C 9C 8C QS 4S 2H
: 5H QH 3C AC 3H 4H QD
: QC 9S 6H 9H 3S KS 3D
: 5D 2S JC 5C JH 6D
: 2D KD TH TC TD 8D
: 7H JS KH TS KC 7C
: AH 5S 6S AD 8H JD
: 7S 6C 7D 4D 8S 9D
EOF

# TEST
is_deeply( [ run($ms24) ], [$ms24_pruned], "pruned correctly" );

__END__

=head1 COPYRIGHT AND LICENSE

This file is part of Freecell Solver. It is subject to the license terms in
the COPYING.txt file found in the top-level directory of this distribution
and at http://fc-solve.shlomifish.org/docs/distro/COPYING.html . No part of
Freecell Solver, including this file, may be copied, modified, propagated,
or distributed except according to the terms contained in the COPYING file.

Copyright (c) 2012 Shlomi Fish

=cut
