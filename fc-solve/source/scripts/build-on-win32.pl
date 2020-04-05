#!/usr/bin/perl

use strict;
use warnings;

use Carp (qw/ confess /);

sub run
{
    my $cmd = shift;
    if ( system(@$cmd) )
    {
        confess("[@{$cmd}] failed! $! $?");
    }
}

my $build_dir = 'B';
mkdir($build_dir);
chdir($build_dir);

run(
    [
        qq{C:/Program Files/CMake 3.4/bin/CMake},
        '-G', 'MinGW Makefiles',

        # These variables require libgmp which isn't provided by default.
        '-DFCS_WITH_TEST_SUITE=', '-DFCS_ENABLE_DBM_SOLVER=',
        @ARGV,
        '..'
    ]
);

my $make_path = 'C:/strawberry/c/bin/mingw32-make';

run( [ $make_path, ] );

run( [ $make_path, 'package', ] );

__END__

=head1 COPYRIGHT AND LICENSE

This file is part of Freecell Solver. It is subject to the license terms in
the COPYING.txt file found in the top-level directory of this distribution
and at http://fc-solve.shlomifish.org/docs/distro/COPYING.html . No part of
Freecell Solver, including this file, may be copied, modified, propagated,
or distributed except according to the terms contained in the COPYING file.

Copyright (c) 2000 Shlomi Fish

=cut
