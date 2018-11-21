#!/usr/bin/perl

use strict;
use warnings;

use Test::More tests => 3;
use FC_Solve::Paths qw( bin_board data_file samp_board samp_sol );
use FC_Solve::CheckResults ();

my $v = FC_Solve::CheckResults->new(
    {
        data_filename => data_file( ['digests-and-lens-storage.yml'] ),
    }
);

sub vtest
{
    local $Test::Builder::Level = $Test::Builder::Level + 1;

    return $v->vtest(@_);
}

# TEST
vtest(
    {
        id   => "375783-dbm-sol",
        deal => 375_783,
        complete_command =>
"$^X $ENV{FCS_SRC_PATH}/scripts/convert-dbm-fc-solver-solution-to-fc-solve-solution.pl --freecells-num=2 @{[samp_sol('375783.dbm-sol')]}"
    },
"Verifying the output of scripts/convert-dbm-fc-solver-solution-to-fc-solve-solution.pl",
);

#
# We discovered that Freecell Solver did not properly handle the foundations
# with 0's like "H-0" , "S-0", etc. This is as opposed to what exists in the
# README.txt file.
#
# This test aims to fix it.

# TEST
vtest(
    {
        id    => "freecell24_board_with_founds_0",
        deal  => 24,
        board => samp_board('ms24-with-founds-0.board'),
    },
    "Properly handle foundations like H-0 S-0 etc."
);

# TEST
vtest(
    {
        id    => "freecell24",
        deal  => 24,
        board => bin_board('24.no-newline.board'),
    },
    "Handle initial layouts without a trailing newline."
);

# Store the changes at the end so they won't get lost.
$v->end();

__END__

=head1 COPYRIGHT AND LICENSE

This file is part of Freecell Solver. It is subject to the license terms in
the COPYING.txt file found in the top-level directory of this distribution
and at http://fc-solve.shlomifish.org/docs/distro/COPYING.html . No part of
Freecell Solver, including this file, may be copied, modified, propagated,
or distributed except according to the terms contained in the COPYING file.

Copyright (c) 2008 Shlomi Fish

=cut
