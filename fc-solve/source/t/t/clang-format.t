#!/usr/bin/perl

use strict;
use warnings;

use File::Copy qw( copy );
use Test::More;
use File::Which qw( which );

if ( $ENV{FCS_TEST_CLANG_FORMAT} )
{
    my $fmt = which('clang-format');
    if ($fmt)
    {
        plan tests => 1;
        my $SRC_PATH = $ENV{FCS_SRC_PATH};
        my @filenames =
            grep { !/\Qcmd_line_inc.h\E|\Qrate_state.c\E/ }
            grep { m#\A\Q$SRC_PATH\E/(?i:[a-z])# }
            sort { $a cmp $b }
            map  { glob "$SRC_PATH/$_ $SRC_PATH/board_gen/$_" }
            qw/*.c *.h *.cpp *.hpp/;
        foreach my $fn (@filenames)
        {
            copy( $fn, "$fn.orig" );
        }
        system( $fmt, '-style=file', '-i', @filenames );
        my $all_ok = 1;
        foreach my $fn (@filenames)
        {
            if ( system( 'cmp', '-s', $fn, "$fn.orig" ) )
            {
                diag("$fn is improperly formatted.");
                $all_ok = 0;
            }
            else
            {
                unlink("$fn.orig");
            }
        }
        ok( $all_ok, "Success - all files are properly formatted." );
    }
    else
    {
        plan skip_all => "No clang-format found.";
    }
}
else
{
    plan skip_all => "Environment variable to check was set.";
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

