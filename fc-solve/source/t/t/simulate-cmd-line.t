#!/usr/bin/perl

use strict;
use warnings;

use FC_Solve::CmdLine::Expand   ();
use FC_Solve::CmdLine::Simulate ();
use Test::More tests => 6;

use Test::Differences qw/ eq_or_diff /;

sub check
{
    local $Test::Builder::Level = $Test::Builder::Level + 1;

    my $input_argv = shift;
    my $want_argv  = shift;
    my $msg        = shift;

    my $obj = FC_Solve::CmdLine::Expand->new(
        {
            input_argv => $input_argv,
        }
    );

    eq_or_diff( scalar( $obj->argv() ), $want_argv, $msg, );
}

# TEST
check( [ '-s', '-i' ], [ '-s', '-i' ], "Basic test", );

# TEST
check(
    [ '-l', 'cpb' ],
    [
        split( /\s+/, <<'EOF' ),
--flare-name 1 --method soft-dfs -to 0123456789 -opt
-nf --flare-name 2 --method soft-dfs -to 0123467 -opt
-nf --flare-name 3 --method random-dfs -seed 2 -to 0[01][23456789] -opt
-nf --flare-name 4 --method random-dfs -seed 1 -to 0[0123456789] -opt
-nf --flare-name 5 --method random-dfs -seed 3 -to 0[01][23467] -opt
-nf --flare-name 9 --method random-dfs -seed 4 -to 0[0123467] -opt
-nf --flare-name 10 --method random-dfs -to [01][23456789] -seed 8 -opt
-nf --flare-name 11 --method a-star -asw 0.2,0.8,0,0,0 -opt
-nf --flare-name 12 --method random-dfs -to [01][23456789] -seed 268 -opt
-nf --flare-name 15 --method random-dfs -to [0123456789] -seed 142 -opt
-nf --flare-name 16 --method a-star -asw 0.2,0.3,0.5,0,0 -opt
-nf --flare-name 17 --method random-dfs -to [01][23456789] -seed 5 -opt
-nf --flare-name 18 --method a-star -to 0123467 -asw 0.5,0,0.3,0,0 -opt
-nf --flare-name 19 --method soft-dfs -to 0126394875 -opt
-nf --flare-name 20 --method random-dfs -seed 105 -opt
EOF
        '--flares-plan',
'Run:6246@1,Run:2663@2,Run:6799@3,Run:7161@4,Run:3466@5,Run:3594@9,Run:6896@10,Run:7269@11,Run:7194@12,Run:6462@15,Run:7742@16,Run:7029@17,Run:3769@18,Run:5244@19,Run:7149@20',
    ],
    'With -l',
);

{
    my $obj = FC_Solve::CmdLine::Simulate->new(
        {
            input_argv => [ '-l', 'cpb', ],
        },
    );

    # TEST
    ok( $obj, "FC_Solve::CmdLine::Simulate was initialized." );

    # TEST
    is( $obj->get_flares_num(), 15, "There are 15 flares." );

    # TEST
    is(
        $obj->get_flare_by_idx(14)->name,
        20, "The 14th flare's name is '20'.",
    );

    # TEST
    eq_or_diff(
        $obj->get_flare_by_idx(2)->argv,
        [ qw(--method random-dfs -seed 2 -to 0[01][23456789] -opt), ],
        "The 2nd flare's argv is fine.",
    );
}

__END__

=head1 COPYRIGHT AND LICENSE

This file is part of Freecell Solver. It is subject to the license terms in
the COPYING.txt file found in the top-level directory of this distribution
and at http://fc-solve.shlomifish.org/docs/distro/COPYING.html . No part of
Freecell Solver, including this file, may be copied, modified, propagated,
or distributed except according to the terms contained in the COPYING file.

Copyright (c) 2009 Shlomi Fish

=cut
