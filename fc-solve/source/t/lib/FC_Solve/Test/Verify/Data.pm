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
            msg => "Solving Deal #24 with the default heuristic",
        },
        '617_jgl' => {
            args => { deal => 617, theme => [ "-l", "john-galt-line" ], },
            msg => "Solving Deal #617 with the john-galt-line",
        },
        '24_bakers_game' => {
            args => { deal => 24, variant => "bakers_game", theme => [], },
            msg => "Baker's Game Deal #24"
        },
        '1941_as' => {
            args => { deal => 1941, theme => [ "-l", "amateur-star", ], },
            msg => "Freecell Dal No. 1941 with the amateur-star preset",
        },
        '1099_forecell' => {
            args => { deal => 1099, variant => "forecell", theme => [], },
            msg => "Forecell Deal #1099",
        },
        '11982_relaxed' => {
            args => { deal => 11982, variant => "relaxed_freecell", },
            msg => "Relaxed Freecell Deal #11982",
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
            msg => "Eight Off #200 with default heuristic",
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
            msg => "-opt in conjunction with --set-pruning r:tf should work.",
        },
        '24_simple_simon__default' => {
            args => { deal => 24, variant => "simple_simon", theme => [], },
            msg => "Simple Simon #24 with default theme",
        },
        '246_eo' => {
            args => { deal => 246, theme => [ "-l", "eo" ], },
            msg => "Solving Deal #246 with the enlightened-ostrich",
        },

        '19806_simple_simon__default' => {
            args => { deal => 19806, variant => "simple_simon", theme => [], },
            msg => "Simple Simon #19806 with default theme",
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

