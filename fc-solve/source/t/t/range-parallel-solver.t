#!/usr/bin/perl

use strict;
use warnings;

use Test::More tests => 9;
use FC_Solve::Paths qw/ bin_exe_raw /;

my $RANGE_SOLVER = bin_exe_raw( ['freecell-solver-range-parallel-solve'] );

# TEST
ok( !system( $RANGE_SOLVER, "1", "20", "10", "-l", "gi" ),
    "Range solver was successful" );

# TEST
ok(
    !system( $RANGE_SOLVER, "1", "2", "1", "-opt" ),
    "Range solver with -opt was successful"
);

# Apparently, the excessive creation of new soft_threads for the optimization
# thread caused next_soft_thread_id to overflow.
# This checks against it.

# TEST
ok(
    !system( $RANGE_SOLVER, "1", "64", "1", "-l", "cool-jives", "-opt" ),
    "Range solver with -opt and a large number of boards was successful."
);

{
    my $output = `$RANGE_SOLVER 1 3 1 -mi 1`;

    foreach my $deal ( 1 .. 3 )
    {
        # TEST*3
        like(
            $output,
            qr{^Intractable Board No. \Q$deal\E}ms,
            "Boards are marked as intractable given --max-iters"
        );
    }
}

{
    my $output = `$RANGE_SOLVER 1 3 1 -mss 1`;

    foreach my $deal ( 1 .. 3 )
    {
        # TEST*3
        like(
            $output,
            qr{^Intractable Board No. \Q$deal\E}ms,
            "Boards are marked as intractable given --max-stored-states"
        );
    }
}
__END__

=head1 COPYRIGHT AND LICENSE

This file is part of Freecell Solver. It is subject to the license terms in
the COPYING.txt file found in the top-level directory of this distribution
and at http://fc-solve.shlomifish.org/docs/distro/COPYING.html . No part of
Freecell Solver, including this file, may be copied, modified, propagated,
or distributed except according to the terms contained in the COPYING file.

Copyright (c) 2008 Shlomi Fish

=cut
