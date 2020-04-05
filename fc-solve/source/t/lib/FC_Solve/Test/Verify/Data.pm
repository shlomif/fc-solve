package FC_Solve::Test::Verify::Data;

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
        '1_sp' => {
            args => { deal => 1, theme => [ "--set-pruning", "r:tf", ], },
            msg  => "Solving Deal #1 with set-pruning run-to-founds",
        },
        '1_sp_a_star' => {
            args => {
                deal  => 1,
                theme => [ "--method", "a-star", "--set-pruning", "r:tf" ],
            },
            msg => "Solving Deal #1 with a-star and sp-r-tf",
        },
        'id24' => {
            args => { deal => 24 },
            msg  => "Verifying the solution of deal #24",
        },
        'id1941' => {
            args => { deal => 1941 },
            msg  => "Verifying 1941 (The Hardest Deal})",
        },
        '24_default' => {
            args => { deal => 24, theme => [], },
            msg  => "Solving Deal #24 with the default heuristic",
        },
        '24_sp' => {
            args => { deal => 24, theme => [ "--set-pruning", "r:tf", ], },
            msg  => "Solving Deal #24 with set-pruning run-to-founds",
        },
        '110_dto2' => {
            args => {
                deal  => 110,
                theme => [
                    "--method", "random-dfs",
                    "-dto2",    "9,[012][4678]",
                    "-dto2",    "9,[01234678]=asw(1)",
                    "-seed",    10468
                ]
            },
            msg => "A configuration with which the solver got stuck."
        },
        '617_jgl' => {
            args => { deal => 617, theme => [ "-l", "john-galt-line" ], },
            msg  => "Solving Deal #617 with the john-galt-line",
        },
        '24_bakers_game' => {
            args => { deal => 24, variant => "bakers_game", theme => [], },
            msg  => "Baker's Game Deal #24"
        },
        '17_lg' => {
            args => { deal => 17, theme => [ "-l", "looking-glass", ], },
            msg  => "Freecell Deal No. 17 with the lg preset",
        },
        '1941_as' => {
            args => { deal => 1941, theme => [ "-l", "amateur-star", ], },
            msg  => "Freecell Deal No. 1941 with the amateur-star preset",
        },
        '1099_forecell' => {
            args => { deal => 1099, variant => "forecell", theme => [], },
            msg  => "Forecell Deal #1099",
        },
        '11982_relaxed' => {
            args => { deal => 11982, variant => "relaxed_freecell", },
            msg  => "Relaxed Freecell Deal #11982",
        },
        '1977_seahaven' => {
            args => {
                deal    => 1977,
                variant => "seahaven_towers",
                theme   => [ "-l", "fools-gold", ],
            },
            msg => "Seahaven Towers #1977",
        },
        '200_eight_off' => {
            args => {
                deal    => 200,
                variant => "eight_off",
            },
            msg => "Eight Off #200 with -l gi",
        },
        '200_eight_off_default' => {
            args => { deal => 200, variant => "eight_off", theme => [], },
            msg  => "Eight Off #200 with default heuristic",
        },
        '24_ct' => {
            args => { deal => 24, theme => [ '-l', 'ct', ], },
            msg  => '-l ct should work.',
        },
        '24_opt' => {
            args => { deal => 24, theme => ["-opt"], },
            msg  => "-opt should work.",
        },
        '24_opt_sp_r_tf' => {
            args => { deal => 24, theme => [ "-opt", "-sp", "r:tf", ], },
            msg  => "-opt in conjunction with --set-pruning r:tf should work.",
        },
        '24_simple_simon__default' => {
            args => { deal => 24, variant => "simple_simon", theme => [], },
            msg  => "Simple Simon #24 with default theme",
        },
        '246_eo' => {
            args => { deal => 246, theme => [ "-l", "eo" ], },
            msg  => "Solving Deal #246 with the enlightened-ostrich",
        },

        # This command line theme yields an especially short solution to the
        # previously intractable deal #12 .
        '2fc_12' => {
            args => {
                deal  => 12,
                theme => [
                    qw(--freecells-num 2 -to '[012][347]' --method random-dfs -seed 33)
                ],
                variant        => "custom",
                msdeals        => 1,
                variant_params => {
                    'num_decks'              => 1,
                    'num_columns'            => 8,
                    'num_freecells'          => 2,
                    'sequence_move'          => "limited",
                    'seq_build_by'           => "alt_color",
                    'empty_stacks_filled_by' => "any",
                },
            },
            msg => "Checking the 2-freecells '-seed 33' preset.",
        },

        # This command line theme yields an ever shorter solution to the
        # previously intractable deal #12 .
        '2fc_12_shorter' => {
            args => {
                deal  => 12,
                theme => [
                    qw(--freecells-num 2 -to '[012][347]' --method random-dfs -seed 236)
                ],
                variant        => "custom",
                msdeals        => 1,
                variant_params => {
                    'num_decks'              => 1,
                    'num_columns'            => 8,
                    'num_freecells'          => 2,
                    'sequence_move'          => "limited",
                    'seq_build_by'           => "alt_color",
                    'empty_stacks_filled_by' => "any",
                },
            },
            msg => "Checking the 2-freecells '-seed 236' preset.",
        },
        '19806_simple_simon__default' => {
            args => { deal => 19806, variant => "simple_simon", theme => [], },
            msg  => "Simple Simon #19806 with default theme",
        },
        '1_simple_simon__abcdefghi' => {
            args => {
                deal    => 1,
                variant => "simple_simon",
                theme   => [ "-to", "abcdefghi", ],
            },
            msg => "Simple Simon #1 with abcdefghi (move seq to false parent)",
        },
        '1_simple_simon__ni' => {
            args => {
                deal    => 1,
                variant => "simple_simon",
                theme   => [ "-to", "abcdefgh", "-ni", "-to", "abcdefghi", ],
            },
            msg => "Simple Simon #1 using an -ni",
        },
        'l_by' => {
            args => {
                deal    => 254076,
                msdeals => 1,
                theme   => [ "-l", "by", "--scans-synergy", "dead-end-marks" ],
            },
            msg =>
"There is a solution for 254,076 with -l by and a scans synergy.",
        },
        'freecell25_j_move' => {
            args => {
                deal  => 25,
                theme => [ "-to", "0123456j89" ],
            },
            msg => "Verify the 'j' move - fc2EmptyS",
        },
        'lead_colons' => {
            args => {
                samp_board => "24-mid-with-colons.board",
            },
            msg =>
"Accepting a board with leading colons as directly input from the -p -t solution",
        },
        'nht' => {
            args => {
                deal  => 24,
                theme => [ "-nht", ],
            },
            msg => "Testing a solution with the -nht flag.",
        },
        'pats24' => {
            args => {
                deal          => 24,
                theme         => [ "--method", "patsolve", ],
                uses_patsolve => 1,
            },
            msg => "Solving Deal #24 with patsolve",
        },
        'simple_simon__next_instance' => {
            args => {
                deal    => 1,
                variant => "simple_simon",
                theme   => [
                    "-to",             "abcdefgh",
                    "--next-instance", "-to",
                    "abcdefghi",
                ],
            },
            msg => "Simple Simon #1 using an -ni",
        },
        'simple_simon__tlm' => {
            args => {
                deal    => 1,
                variant => "simple_simon",
                theme   => [ "-l", "tlm" ],
            },
            msg => "Simple Simon #1 using the 'the-last-mohican' theme",
        },
        'unlimited_seq_move' => {
            args => {
                samp_board =>
                    "larrysan-kings-only-0-freecells-unlimited-move.board",
                theme => [
                    qw(--freecells-num 0 --empty-stacks-filled-by kings --sequence-move unlimited)
                ],
                variant        => "custom",
                variant_params => {
                    'num_decks'              => 1,
                    'num_columns'            => 8,
                    'num_freecells'          => 0,
                    'sequence_move'          => "unlimited",
                    'seq_build_by'           => "alt_color",
                    'empty_stacks_filled_by' => "kings",
                },
            },
            msg =>
"sequence move unlimited is indeed unlimited (even if not esf-by-any).",
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
