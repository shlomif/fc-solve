#!/usr/bin/perl

use strict;
use warnings;

use Test::More;
use Test::Differences qw( eq_or_diff );

use File::Which qw( which );
use String::ShellQuote qw/ shell_quote /;

my $flake8 = which('flake8');

if ( !$flake8 )
{
    plan skip_all => "No flake8 found.";
}
plan tests => 2;
my $cmd = shell_quote( $flake8, $ENV{FCS_SRC_PATH} );

# TEST
eq_or_diff( scalar(`$cmd`), '', "flake8 is happy with the code." );

# TEST
is( $?, 0, 'flake8 exited succesfully' );

__END__

=head1 COPYRIGHT AND LICENSE

This file is part of Freecell Solver. It is subject to the license terms in
the COPYING.txt file found in the top-level directory of this distribution
and at http://fc-solve.shlomifish.org/docs/distro/COPYING.html . No part of
Freecell Solver, including this file, may be copied, modified, propagated,
or distributed except according to the terms contained in the COPYING file.

Copyright (c) 2016 Shlomi Fish

=cut
