#!/usr/bin/env perl

use strict;
use warnings;

use Test::More tests => 1;

use Path::Tiny qw/ path tempdir tempfile cwd /;

{
    my $out =
`$ENV{FCS_PATH}/board_gen/pi-make-microsoft-freecell-board -t 164 | $ENV{FCS_PATH}/fc-solve  --freecells-num 0 --tests-order '[0AB]=all()' -sp r:tf -mi 10000 -sam -sel -p -t | sha256sum -`;

    # TEST
    is(
        scalar( $out =~ s/\s.*//mrs ),
        scalar(
            path(
"$ENV{FCS_SRC_PATH}/../scripts/ms164_zerofc_mode_expected-sample-sol.txt.sha256"
            )->slurp_raw =~ s/\r?\n\z//r
        ),
        "good solution zero fc mode results"
    );
}
