package FC_Solve::CheckResults::Data;

use strict;
use warnings;

use parent 'Test::Data::Split::Backend::ValidateHash';

my %verify_tests;

sub get_hash
{
    return \%verify_tests;
}

sub validate_and_transform
{
    my ( $self, $args ) = @_;

    return $args->{data};
}

__PACKAGE__->populate(
    [
        'id24' => {
            args => { id => "freecell24", deal => 24 },
            msg  => "Verifying the solution of deal #24",
        },
        'bakers_game24default' => {
            args => {
                id      => "bakers_game24default",
                deal    => 24,
                variant => "bakers_game",
                theme   => [],
            },
            msg => "Baker's Game Deal #24",
        },
        'random_dfs_with_rand_parens' => {
            args => {
                id    => "random_dfs_with_rand_parens",
                deal  => 24,
                theme => [
                    "--method", "random-dfs",
                    "-seed",    "1",
                    "-to",      "[01]=rand()[23456789]=rand()"
                ],
            },
            msg =>
"Verifying the solution of a deal with random-dfs specified using '=rand()'",
        },
        'freecell1941' => {
            args => { id => "freecell1941", deal => 1941 },
            msg  => "Verifying 1941 (The Hardest Deal)",
        },
        'freecell24empty' => {
            args => { id => "freecell24empty", deal => 24, theme => [], },
            msg  => "Solving Deal #24 with the default heuristic",
        },
        'freecell617jgl' => {
            args => {
                id    => "freecell617jgl",
                deal  => 617,
                theme => [ "-l", "john-galt-line" ],
            },
            msg => "Solving Deal #617 with the john-galt-line",
        },
        'forecell1099default' => {
            args => {
                id      => "forecell1099default",
                deal    => 1099,
                variant => "forecell",
                theme   => [],
            },
            msg => "Forecell Deal #1099",
        },
        'relaxed_freecell11982' => {
            args => {
                id      => "relaxed_freecell11982",
                deal    => 11982,
                variant => "relaxed_freecell",
            },
            msg => "Relaxed Freecell Deal #11982",
        },
        'seahaven_towers1977fools' => {
            args => {
                id      => "seahaven_towers1977fools-gold",
                deal    => 1977,
                variant => "seahaven_towers",
                theme   => [ "-l", "fools-gold", ],
            },
            msg => "Seahaven Towers #1977",
        },
        'eight_off200' => {
            args => {
                id      => "eight_off200",
                deal    => 200,
                variant => "eight_off",
            },
            msg => "Eight Off #200 with -l gi",
        },
        'eight_off200default' => {
            args => {
                id      => "eight_off200default",
                deal    => 200,
                variant => "eight_off",
                theme   => [],
            },
            msg => "Eight Off #200 with default heuristic",
        },
        'simple_simon24default' => {
            args => {
                id      => "simple_simon24default",
                deal    => 24,
                variant => "simple_simon",
                theme   => [],
            },
            msg => "Simple Simon #24 with default theme",
        },
        'simple_simon19806default' => {
            args => {
                id      => "simple_simon19806default",
                deal    => 19806,
                variant => "simple_simon",
                theme   => [],
            },
            msg => "Simple Simon #19806 with default theme",
        },
        'simple_simon1with_i' => {
            args => {
                id      => "simple_simon1with_i",
                deal    => 1,
                variant => "simple_simon",
                theme   => [ "-to", "abcdefghi" ],
            },
            msg => "Simple Simon #1 with seq-to-false-parent",
        },
        'simple_simon1with_next_instance' => {
            args => {
                id      => "simple_simon1with_next_instance",
                deal    => 1,
                variant => "simple_simon",
                theme   => [
                    "-to",             "abcdefgh",
                    "--next-instance", "-to",
                    "abcdefghi",
                ],
            },
            msg => "Simple Simon #1 using an --next-instance",
        },
        'freecell254076_l_by' => {
            args => {
                id      => "freecell254076_l_by",
                deal    => 254076,
                msdeals => 1,
                theme   => [ "-l", "by", "--scans-synergy", "dead-end-marks" ],
            },
            msg => "Freecell MS 254,076 while using -l by with dead-end-marks",
        },
        'freecell24_out_file' => {
            args => {
                id          => "freecell24",
                deal        => 24,
                output_file => "24.solution.txt",
            },
            msg => "Verifying the solution of deal No. 24 with -o",
        },
        'freecell24_children_playing_ball' => {
            args => {
                id          => "freecell24_children_playing_ball",
                deal        => 24,
                theme       => [ "-l", "children-playing-ball" ],
                with_flares => 1
            },
            msg => "Verifying the solution of deal No. 24 with -l cpb",
        },
        'freecell24_sentient_pearls' => {
            args => {
                id          => "freecell24_sentient_pearls",
                deal        => 24,
                theme       => [ "-l", "sentient-pearls" ],
                with_flares => 1
            },
            msg => "Verifying the solution of deal No. 24 with -l sp",
        },
        'freecell24with_empty_soft_thread_name' => {
            args => {
                id    => "freecell24with_empty_soft_thread_name",
                deal  => 1,
                theme => [ "-to", "013[2456789]", "-nst", "-l", "by", ],
            },
            msg => "Solving Deal #24 with an empty soft thread name",
        },
        'freecell1941__cache_limit' => {
            args => {
                id    => "freecell1941",
                deal  => 1941,
                theme => [ "-l", "gi", "--cache-limit", "2000" ],
            },
            msg => "Verifying 1941 with --cache-limit set to a different value",
        },
        'qualified_seed_for_6240' => {
            args => {
                id   => "qualified_seed_for_6240",
                deal => 6240,
                theme =>
                    [ "-l", "qs", "-fif", "5", "--flares-choice", "fcpro", ],
                with_flares => 1,
            },
            msg => "Qualified seed test with -fif and --flares-choice fcpro",
        },
    ]
);

1;

__END__

=head1 COPYRIGHT AND LICENSE

This file is part of Freecell Solver. It is subject to the license terms in
the COPYING.txt file found in the top-level directory of this distribution
and at http://fc-solve.shlomifish.org/docs/distro/COPYING.html . No part of
Freecell Solver, including this file, may be copied, modified, propagated,
or distributed except according to the terms contained in the COPYING file.

Copyright (c) 2009 Shlomi Fish

=cut
