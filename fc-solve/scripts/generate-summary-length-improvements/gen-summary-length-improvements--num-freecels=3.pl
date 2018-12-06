#!/usr/bin/env perl

use strict;
use warnings;

use IO::All qw/io/;
use Parallel::ForkManager;

my $MAX_PROCESSES = 4;

my $pm = Parallel::ForkManager->new($MAX_PROCESSES);

my $MAX_ITERS = 1000000;

my $THEME = q#-l ve -fif 10 --freecells-num 3#;

my $OUTPUT_PATH = "$ENV{HOME}/Backup/Arcs/fcs-summary-len-fc=3";

my $BASE = 'theme1';

io->dir($OUTPUT_PATH)->mkpath;

foreach my $seed (1)
{
    my $out_fn = qq#$OUTPUT_PATH/lens-$BASE--seed=$seed.txt#;
    if ( !-e $out_fn )
    {
        if ( not my $pid = $pm->start )
        {
            my $cmd =
qq#./summary-fc-solve seq 1 32000 -- $THEME -mi $MAX_ITERS | tee -a "$out_fn" | grep Unsolved#;
            print "$cmd\n";
            system($cmd);
            $pm->finish;
        }
    }
}

__END__

=head1 COPYRIGHT AND LICENSE

This file is part of Freecell Solver. It is subject to the license terms in
the COPYING.txt file found in the top-level directory of this distribution
and at http://fc-solve.shlomifish.org/docs/distro/COPYING.html . No part of
Freecell Solver, including this file, may be copied, modified, propagated,
or distributed except according to the terms contained in the COPYING file.

Copyright (c) 2016 Shlomi Fish

=cut
