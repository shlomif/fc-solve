#!/usr/bin/perl

use strict;
use warnings;

use Test::More tests => 6;
use FC_Solve::Paths qw( samp_sol );

use FC_Solve::ShaAndLen ();

{
    my $string = "HelloWorld";

    my $sha = FC_Solve::ShaAndLen->new();

    $sha->add($string);

    # TEST
    is( $sha->len(), 10, "Length is 10" );

    # TEST
    is(
        $sha->hexdigest(),
        "872e4e50ce9990d8b041330c47c9ddd11bec6b503ae9386a99da8584e9bb12c4",
        "Hex digest is OK."
    );
}

{
    my $sha = FC_Solve::ShaAndLen->new();

    $sha->add("Hello");
    $sha->add("World");

    # TEST
    is( $sha->len(), 10, "Length is 10" );

    # TEST
    is(
        $sha->hexdigest(),
        "872e4e50ce9990d8b041330c47c9ddd11bec6b503ae9386a99da8584e9bb12c4",
        "Hex digest is OK."
    );
}

SKIP:
{
    my $sha = FC_Solve::ShaAndLen->new();
    my $FN  = samp_sol('fcs-freecell-24.txt');

    if ( !-e $FN )
    {
        skip( "File does not exist.", 2 );
    }
    open my $in, "<", $FN;
    $sha->add_file($in);
    close($in);

    # TEST
    is( $sha->len(), 31811, "Length for add_file() is 32280" );

    # TEST
    is(
        $sha->hexdigest(),
        "e838102eafd8984dad80528e5bad1abe9f1cc669628495fce2c58e61cf2c3558",
        "Hex digest is OK for add_file()"
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
