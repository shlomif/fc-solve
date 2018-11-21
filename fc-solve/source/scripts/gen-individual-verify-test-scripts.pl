#!/usr/bin/perl

use strict;
use warnings;
use autodie;

use FindBin;
use lib "$FindBin::Bin/../t/lib";

use FC_Solve::SplitTests;

my $module = 'FC_Solve::Test::Verify';
FC_Solve::SplitTests->gen(
    {
        prefix      => 'verify',
        module      => $module,
        data_module => 'FC_Solve::Test::Verify::Data',
    },
);

__END__

=head1 COPYRIGHT AND LICENSE

This file is part of Freecell Solver. It is subject to the license terms in
the COPYING.txt file found in the top-level directory of this distribution
and at http://fc-solve.shlomifish.org/docs/distro/COPYING.html . No part of
Freecell Solver, including this file, may be copied, modified, propagated,
or distributed except according to the terms contained in the COPYING file.

Copyright (c) 2009 Shlomi Fish

=cut
