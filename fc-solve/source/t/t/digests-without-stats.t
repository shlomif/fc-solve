#!/usr/bin/perl

use strict;
use warnings;

=head1 digests-without-stat.t

This test script aims to verify that scripts yield a good result of SHA-256
digests and lengths while stripping away the lines of the statistics. This is
useful for pre-testing desirable solutions while avoiding the issue of the
statistics like the number of states checked and the number of stored states.

=cut

use Test::More tests => 6;
use FC_Solve::Paths qw( data_file );
use FC_Solve::CheckResults ();

my $v = FC_Solve::CheckResults->new(
    {
        data_filename => data_file( ['digests-and-lens-wo-stats-storage.yml'] ),
        trim_stats => 1,
    }
);

sub vtest
{
    local $Test::Builder::Level = $Test::Builder::Level + 1;

    return $v->vtest(@_);
}

# 24 is my lucky number. (Shlomif)
# TEST
vtest( { id => "freecell_default24", deal => 24, theme => [], },
    "Verifying the solution of deal #24" );

# TEST
vtest(
    {
        id    => "freecell_simple_flare_2",
        deal  => 2,
        theme => [
            qw(
                --flare-name dfs
                --next-flare --method a-star --flare-name befs
                --flares-plan), q{Run:500@dfs,Run:1500@befs},
        ],
    }
);

# This should generate the same results as --method dfs.
# TEST
vtest(
    {
        id    => "freecell_flares_cp_1_deal_6",
        deal  => 6,
        theme => [
            qw(
                --flare-name dfs
                --next-flare --method a-star --flare-name befs
                --flares-plan), q{Run:300@dfs,Run:3000@befs,CP:,Run:200@dfs},
        ],
    }
);

# This should generate the same results as --method dfs.
# It checks that the plan is restarted over after it reaches the end
# and yields the end.
# TEST
vtest(
    {
        id    => "freecell_flares_cp_1_circular_deal_6",
        deal  => 6,
        theme => [
            qw(
                --flare-name dfs
                --next-flare --method a-star --flare-name befs
                --flares-plan), q{Run:300@dfs,Run:1000@befs,CP:,Run:100@dfs},
        ],
    }
);

# This should test the run-indefinitely
# TEST
vtest(
    {
        id    => "freecell_flares_run_indef_1_deal_6",
        deal  => 6,
        theme => [
            qw(
                --flare-name dfs
                --next-flare --method a-star --flare-name befs
                --flares-plan), q{Run:500@dfs,RunIndef:befs},
        ],
    }
);

# This checks for an infinite loop when several identically-spaced quotas
# are given to the flares.
# TEST
vtest(
    {
        id    => "freecell_flares_equally_spaced_quotas_deal_1",
        deal  => 1,
        theme => [

            # This is to avoid warnings on commas in qw(...)
            grep { /\S/ } split( /\s+/, <<'EOF')

--method a-star -asw 0.2,0.8,0,0,0 -step 500 --st-name 11 --flare-name 11 -nf
--method a-star -to 0123467 -asw 0.5,0,0.3,0,0 -step 500 --st-name 18 --flare-name 18 -nf
--flares-plan Run:200@18,Run:200@11,Run:200@18

EOF

        ],
    }
);

# Store the changes at the end so they won't get lost.
$v->end();

__END__

=head1 COPYRIGHT AND LICENSE

This file is part of Freecell Solver. It is subject to the license terms in
the COPYING.txt file found in the top-level directory of this distribution
and at http://fc-solve.shlomifish.org/docs/distro/COPYING.html . No part of
Freecell Solver, including this file, may be copied, modified, propagated,
or distributed except according to the terms contained in the COPYING file.

Copyright (c) 2009 Shlomi Fish

=cut
