#!/usr/bin/perl

use strict;
use warnings;
use autodie;

use Test::More;
use String::ShellQuote qw/ shell_quote /;
use FC_Solve::Paths qw( $FC_SOLVE_EXE is_break );
if ( is_break() )
{
    plan skip_all => "Does not work with BREAK_BACK_COMPAT";
}
plan tests => 2;
{
    open my $good_ver_fh, "<", "$ENV{FCS_SRC_PATH}/ver.txt";
    my $good_ver = <$good_ver_fh>;
    close($good_ver_fh);
    chomp($good_ver);

    my $got_output = `$FC_SOLVE_EXE --version`;

    my $got_status = $?;

    # TEST
    ok( !$got_status, "fc-solve --version ran fine" );

    # TEST
    like(
        $got_output,
        qr{^libfreecell-solver version \Q$good_ver\E}ms,
        "Version is contained in the --version display",
    );
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
