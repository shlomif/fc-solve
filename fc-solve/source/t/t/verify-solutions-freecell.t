#!/usr/bin/perl

use strict;
use warnings;

use Test::More tests => 2;
use FC_Solve::Test::Verify ();
use FC_Solve::Paths qw( samp_board );

# TEST
FC_Solve::Test::Verify::r(
    {
        board => samp_board("24-mid-with-colons.board"),
    },
"Accepting a board with leading colons as directly input from the -p -t solution",
);

# TEST
FC_Solve::Test::Verify::r(
    {
        board =>
            samp_board( "larrysan-kings-only-0-freecells-unlimited-move.board",
            ),
        theme => [
            qw(--freecells-num 0 --empty-stacks-filled-by kings --sequence-move unlimited)
        ],
        variant        => "custom",
        variant_params => {
            'num_decks'              => 1,
            'num_columns'            => 8,
            'num_freecells'          => 0,
            'sequence_move'          => "unlimited",
            'seq_build_by'           => "alt_color",
            'empty_stacks_filled_by' => "kings",
        },
    },
    "sequence move unlimited is indeed unlimited (even if not esf-by-any)."
);

__END__

=head1 COPYRIGHT AND LICENSE

This file is part of Freecell Solver. It is subject to the license terms in
the COPYING.txt file found in the top-level directory of this distribution
and at http://fc-solve.shlomifish.org/docs/distro/COPYING.html . No part of
Freecell Solver, including this file, may be copied, modified, propagated,
or distributed except according to the terms contained in the COPYING file.

Copyright (c) 2000 Shlomi Fish

=cut
