package FC_Solve::Test::Verify::Data;

use strict;
use warnings;

use parent 'Test::Data::Split::Backend::Hash';

my %verify_tests =
(
    'id24' =>
    {
        args => {deal => 24},
        msg => "Verifying the solution of deal #24",
    },
    'id1941' =>
    {
        args => {deal => 1941},
        msg => "Verifying 1941 (The Hardest Deal})",
    },
    '24_default' =>
    {
        args => {deal => 24, theme => [],},
        msg => "Solving Deal #24 with the default heuristic",
    },
    '617_jgl' =>
    {
        args => {deal => 617, theme => ["-l", "john-galt-line"],},
        msg => "Solving Deal #617 with the john-galt-line",
    },
    '24_bakers_game' =>
    {
        args => {deal => 24, variant => "bakers_game", theme => [],},
        msg => "Baker's Game Deal #24"
    },
    '1941_as' =>
    {
        args => {deal => 1941, theme => ["-l", "amateur-star", ],},
        msg => "Freecell Dal No. 1941 with the amateur-star preset",
    },
    '1099_forecell' =>
    {
        args => {deal => 1099, variant => "forecell", theme => [],},
        msg => "Forecell Deal #1099",
    },
    '11982_relaxed' =>
    {
        args => {deal => 11982, variant => "relaxed_freecell", },
        msg => "Relaxed Freecell Deal #11982",
    },
    '1977_seahaven' =>
    {
        args =>
        {
            deal => 1977,
            variant => "seahaven_towers",
            theme => ["-l", "fools-gold",],
        },
        msg =>
        "Seahaven Towers #1977",
    },
    '200_eight_off' =>
    {
        args =>
        {
            deal => 200,
            variant => "eight_off",
        },
        msg => "Eight Off #200 with -l gi",
    },
    '200_eight_off_default' =>
    {
        args => {deal => 200, variant => "eight_off", theme => [],},
        msg => "Eight Off #200 with default heuristic",
    },
    '24_opt' =>
    {
        args => {deal => 24, theme => ["-opt"],},
        msg => "-opt should work.",
    },
    '24_opt_sp_r_tf' =>
    {
        args => {deal => 24, theme => ["-opt", "-sp", "r:tf",],},
        msg => "-opt in conjunction with --set-pruning r:tf should work.",
    },
    '24_simple_simon__default' =>
    {
        args => { deal => 24, variant => "simple_simon", theme => [], },
        msg => "Simple Simon #24 with default theme",
    },
    '19806_simple_simon__default' =>
    {
        args => { deal => 19806, variant => "simple_simon", theme => [], },
        msg => "Simple Simon #19806 with default theme",
    },
    '1_simple_simon__abcdefghi' =>
    {
        args => {
            deal => 1, variant => "simple_simon",
            theme => ["-to", "abcdefghi",],
        },
        msg => "Simple Simon #1 with abcdefghi (move seq to false parent)",
    },
    '1_simple_simon__ni' =>
    {
        args => {
            deal => 1, variant => "simple_simon",
            theme => ["-to", "abcdefgh", "-ni", "-to", "abcdefghi",],
        },
        msg => "Simple Simon #1 using an -ni",
    },
    'simple_simon__next_instance' =>
    {
        args => {deal => 1, variant => "simple_simon",
            theme => ["-to", "abcdefgh", "--next-instance", "-to", "abcdefghi",],
        },
        msg => "Simple Simon #1 using an -ni",
    },
);

sub get_hash
{
    return \%verify_tests;
}

1;

=head1 COPYRIGHT AND LICENSE

Copyright (c) 2009 Shlomi Fish

Permission is hereby granted, free of charge, to any person
obtaining a copy of this software and associated documentation
files (the "Software"), to deal in the Software without
restriction, including without limitation the rights to use,
copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the
Software is furnished to do so, subject to the following
conditions:

The above copyright notice and this permission notice shall be
included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
OTHER DEALINGS IN THE SOFTWARE.

=cut

