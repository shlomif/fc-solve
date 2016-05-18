package FC_Solve::Test::Valgrind::Data;

use strict;
use warnings;

use parent 'Test::Data::Split::Backend::ValidateHash';

my %valgrind_tests;

sub get_hash
{
    return \%valgrind_tests;
}

sub validate_and_transform
{
    my ($self, $args) = @_;

    my $id = $args->{id};
    my $data = $args->{data};
    if (!exists($data->{msg}))
    {
        die "msg does not exist.";
    }
    if (exists($data->{blurb}))
    {
        die "blurb exists.";
    }

    my %new_data = %$data;
    my $msg = delete $new_data{msg};
    $new_data{log_fn} = "valgrind--$id.log";

    return +{ msg => $msg, args => (\%new_data) };
}

__PACKAGE__->populate
(
    [
        'dbm_fc_solver_1' =>
        {
            prog => "dbm_fc_solver",
            argv =>
            [
                '--offload-dir-path', {type => 'tempdir', },
                { type => 'sample_board', arg => '2freecells-24-mid-with-colons.board' }
            ],
            msg => qq{dbm_fc_solver from 24-mid-with-colons.},
        },
        'empty_board' =>
        {
            prog => "fc-solve",
            argv => [
                {
                    type => 'catfile',
                    args => [{ type => 'ENV', arg => 'FCS_PATH'}, 'empty.board'],
                },
            ],
            msg => "Sanity of running on an empty board input.",
        },
        'fc-solve-crashy-preset-1' =>
        {
            prog => "fc-solve",
            argv => [
                '--read-from-file',
                {
                    type => 'catfile',
                    prefix => "4,",
                    args => [{ type => 'ENV', arg => 'FCS_SRC_PATH',},
                        qw(t data presets crashy-preset-1.preset)
                    ],
                },
                qw(-s -i -p -t -sam),
                {
                    type => 'catfile',
                    args => [{ type => 'ENV', arg => 'FCS_PATH'}, '24.board'],
                },
            ],
            msg => "Check the sanity of crashy-preset-1 which over-rides the soft-thread several times.",
        },
        'fc-solve-not-enough-input' =>
        {
            prog => "fc-solve",
            argv => [ { type => 'sample_board', arg => '24-with-7-cols.board' } ],
            msg => "Check the sanity of not enough input.",
        },
        'fc-solve-trim-max-stored-states' =>
        {
            prog=>"fc-solve",
            argv => [
                qw(--method soft-dfs --st-name dfs -nst --method a-star --st-name befs --trim-max-stored-states 100 --prelude) ,
                '200@befs,100@dfs,1000@befs,500000@dfs',
                qw(-s -i -p -t -sam -mi 3000),
                {
                    type => 'catfile',
                    args => [{type => 'ENV', arg => 'FCS_PATH'}, '1941.board',]
                },
            ],
            msg => "Check the sanity of --trim-max-stored-states.",
        },
        'range_parallel_solve_befs' =>
        {
            prog => "freecell-solver-range-parallel-solve",
            argv => [qw(1 2 1 --method a-star)],
            msg => qq{range-parallel-solve for board #2 using the BeFS method valgrind}
        },
        'range_parallel_solve_l_gi' =>
        {
            prog => "freecell-solver-range-parallel-solve",
            argv => ["1", "2", "1", "-l", "gi"],
            msg => qq{"range-parallel-solve 1 2 1 -l gi" returned no errors}
        },
        'range_parallel_solve__dash_opt' =>
        {
            prog => "freecell-solver-range-parallel-solve",
            argv => ["1", "2", "1", "-opt"],
            msg => qq{"range-parallel-solve 1 2 1 -opt" returned no errors}
        },
        'range_parallel_solve__next-flare' =>
        {
            prog => "freecell-solver-range-parallel-solve",
            argv => ["1", "2", "1", "--next-flare"],
            msg => qq{"range-parallel-solve 1 2 1 --next-flare" returned no errors}
        },
        'range_parallel_solve__11982_opt' =>
        {
            prog => "freecell-solver-range-parallel-solve",
            argv => [qw(11981 11983 1 -opt)],
            msg => qq{"range-parallel-solve 11981 11983 1 -opt" returned no errors}
        },
        'range_parallel_solve__1_3_1_flares' =>
        {
            prog => "freecell-solver-range-parallel-solve",
            argv => [qw(1 3 1 --flare-name dfs --next-flare --method a-star --flare-name befs --flares-plan), q(Run:300@dfs,Run:500@befs,CP:,Run:200@dfs),],
            msg => qq{"range-parallel-solve 1 3 1 flares" returned no errors}
        },
        'range_parallel_solve__1__2__invalid_flares_plan_1' =>
        {
            prog => "freecell-solver-range-parallel-solve",
            argv => [qw(1 2 1 --flares-plan), q(Run:500@foo)],
            msg => qq{"range-parallel-solve --flares-plan Run:500\@foo" does not crash.}
        },
        'range_parallel_solve__sp_r_tf__not_leak' =>
        {
            prog => "freecell-solver-range-parallel-solve",
            argv => [qw(1 2 1 --total-iterations-limit 1000 --method soft-dfs
                -to 0123456789 -sp r:tf)
            ],
            msg => qq{"range-parallel-solve -sp r:tf" does not leak.}
        },
        'range_parallel__invalid_print_step' =>
        {
            prog => "freecell-solver-range-parallel-solve",
            argv => [qw(1 1 0 -l as)
            ],
            msg => qq{Make sure that the program does not crash on a zero print_step},
        },
        'fc_pro_range_solve__invalid_print_step' =>
        {
            prog => "freecell-solver-fc-pro-range-solve",
            argv => [qw(1 2 0 -l as)
            ],
            msg => qq{Make sure that the fc-pro program does not crash on a zero print_step},
        },
        'board_gen__pi_make_ms__t_only' =>
        {
            prog => "board_gen/pi-make-microsoft-freecell-board",
            argv => [qw(-t)
            ],
            msg => qq{Board generation should not crash with only -t flag (pi-make-ms)},
        },
        'board_gen__aisleriot__t_only' =>
        {
            prog => "board_gen/make-aisleriot-freecell-board",
            argv => [qw(-t)
            ],
            msg => qq{Board generation should not crash with only -t flag (aisleriot)},
        },
        'board_gen__gnome__t_only' =>
        {
            prog => "board_gen/make-gnome-freecell-board",
            argv => [qw(-t)
            ],
            msg => qq{Board generation should not crash with only -t flag (gnome)},
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

