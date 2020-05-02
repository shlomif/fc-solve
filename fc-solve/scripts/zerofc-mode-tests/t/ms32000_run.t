#!/usr/bin/env perl

use strict;
use warnings;

use Test::More tests => 1;

use Path::Tiny qw/ path tempdir tempfile cwd /;

{
    my $out =
`$ENV{FCS_PATH}/summary-fc-solve seq 1 32000 -- --freecells-num 0 --tests-order 0AB -sp r:tf -mi 10000 | sha256sum -`;

    # TEST
    is(
        scalar( $out =~ s/\s.*//mrs ),
        scalar(
            path(
"$ENV{FCS_SRC_PATH}/../scripts/ms32000_zerofc_mode_expected.txt.sha256"
            )->slurp_raw =~ s/\r?\n\z//r
        ),
        "good zero fc mode results"
    );
}
