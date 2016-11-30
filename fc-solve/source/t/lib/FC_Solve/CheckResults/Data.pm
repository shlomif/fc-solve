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
    my ($self, $args) = @_;

    return $args->{data};
}

__PACKAGE__->populate(
    [
        'id24' =>
        {
            args => {id => "freecell24", deal => 24},
            msg => "Verifying the solution of deal #24",
        },
        'bakers_game24default' =>
        {
            args => {id => "bakers_game24default", deal => 24, variant => "bakers_game", theme => [],},
            msg =>     "Baker's Game Deal #24",
        },
        'random_dfs_with_rand_parens' =>
        {
            args => {id => "random_dfs_with_rand_parens", deal => 24,
                theme => ["--method", "random-dfs", "-seed", "1", "-to", "[01]=rand()[23456789]=rand()"],
            },
            msg =>     "Verifying the solution of a deal with random-dfs specified using '=rand()'",
        },
        'freecell1941' =>
        {
            args => {id => "freecell1941", deal => 1941},
            msg =>  "Verifying 1941 (The Hardest Deal)",
        },
        'freecell24empty' =>
        {
            args => {id => "freecell24empty", deal => 24, theme => [],},
            msg =>     "Solving Deal #24 with the default heuristic",
        },
        'freecell617jgl' =>
        {
            args => {id => "freecell617jgl", deal => 617, theme => ["-l", "john-galt-line"],},
            msg =>     "Solving Deal #617 with the john-galt-line",
        },
        'forecell1099default' =>
        {
            args => {id => "forecell1099default", deal => 1099, variant => "forecell", theme => [],},
            msg =>     "Forecell Deal #1099",
        },
        'relaxed_freecell11982' =>
        {
            args => {id => "relaxed_freecell11982",deal => 11982, variant => "relaxed_freecell", },
            msg =>     "Relaxed Freecell Deal #11982",
        },
        'seahaven_towers1977fools' =>
        {
            args => {
                id => "seahaven_towers1977fools-gold",
                deal => 1977,
                variant => "seahaven_towers",
                theme => ["-l", "fools-gold",],
            },
            msg =>     "Seahaven Towers #1977",
        },
        'eight_off200' =>
        {
            args => {
                id => "eight_off200", deal => 200, variant => "eight_off",
            },
            msg =>     "Eight Off #200 with -l gi",
        },
        'eight_off200default' =>
        {
            args => {id =>"eight_off200default", deal => 200, variant => "eight_off",
                theme => [],
            },
            msg =>     "Eight Off #200 with default heuristic",
        },
    ]
);


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

