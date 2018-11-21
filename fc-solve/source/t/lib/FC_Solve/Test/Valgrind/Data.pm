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
    my ( $self, $args ) = @_;

    my $id   = $args->{id};
    my $data = $args->{data};
    if ( !exists( $data->{msg} ) )
    {
        die "msg does not exist.";
    }
    if ( exists( $data->{blurb} ) )
    {
        die "blurb exists.";
    }

    my %new_data = %$data;
    my $msg      = delete $new_data{msg};
    $new_data{log_fn} = "valgrind--$id.log";

    return +{ msg => $msg, args => ( \%new_data ) };
}

my $arr = [
    'dbm_fc_solver_1' => {
        dbm  => 1,
        prog => "dbm_fc_solver",
        argv => [
            '--offload-dir-path',
            { type => 'tempdir', },
            {
                type => 'sample_board',
                arg  => '2freecells-24-mid-with-colons.board'
            }
        ],
        msg => qq{dbm_fc_solver from 24-mid-with-colons.},
    },
    'empty_board' => {
        prog => "fc-solve",
        argv => [ { type => 'bin_board', arg => 'empty.board', }, ],
        msg  => "Sanity of running on an empty board input.",
    },
    'fc-solve-crashy-preset-1' => {
        prog => "fc-solve",
        argv => [
            '--read-from-file',
            {
                type   => 'catfile',
                prefix => "4,",
                args   => [
                    {
                        type => 'sample_preset',
                        arg  => 'crashy-preset-1.preset'
                    }
                ]
            },
            qw(-s -i -p -t -sam),
            { type => 'bin_board', arg => '24.board', }
        ],
        msg =>
"Check the sanity of crashy-preset-1 which over-rides the soft-thread several times.",
    },
    'fc-solve-hoi' => {
        prog => "fc-solve",
        argv => [
            qw(-s -i -p -t -sam -sel -hoi -mi 100),
            { type => 'bin_board', arg => '11982.board', }
        ],
        msg => "--hint-on-intractable sanity",
    },
    'fc-solve-crashy-preset-2' => {
        prog => "fc-solve",
        argv => [
            '--read-from-file',
            {
                type   => 'catfile',
                prefix => "0,",
                args   => [
                    {
                        type => 'sample_preset',
                        arg  => 'crashy-preset-2.preset'
                    }
                ]
            },
            qw(-s -i -p -t -sam),
            { type => 'bin_board', arg => '24.board', }
        ],
        msg =>
"Check the sanity of crashy-preset-2 which contains a long double-quoted string.",
    },
    'fc-solve-non-existent-game-preset' => {
        prog => "fc-solve",
        argv =>
            [ '-g', 'notexist', { type => 'bin_board', arg => '24.board', } ],
        msg =>
            "Make sure a non-existent game preset does not crash the solver.",
    },
    'fc-solve-not-enough-input' => {
        prog => "fc-solve",
        argv => [ { type => 'sample_board', arg => '24-with-7-cols.board' } ],
        msg  => "Check the sanity of not enough input.",
    },
    'fc-solve-trim-max-stored-states' => {
        prog => "fc-solve",
        argv => [
            qw(--method soft-dfs --st-name dfs -nst --method a-star --st-name befs --trim-max-stored-states 100 --prelude),
            '200@befs,100@dfs,1000@befs,500000@dfs',
            qw(-s -i -p -t -sam -mi 3000),
            { type => 'bin_board', arg => '1941.board', },
        ],
        msg => "Check the sanity of --trim-max-stored-states.",
    },
    'range_parallel_solve_befs' => {
        prog => "freecell-solver-range-parallel-solve",
        argv => [qw(1 2 1 --method a-star)],
        msg =>
            qq{range-parallel-solve for board #2 using the BeFS method valgrind}
    },
    'range_parallel_solve_l_as_3fc' => {
        prog => "freecell-solver-range-parallel-solve",
        argv => [qw(285 285 1 --freecells-num 3 -l as)],
        msg =>
qq{"range-parallel-solve --freecells-num 3 -l as" returned no errors}
    },
    'range_parallel_solve_l_gi' => {
        prog => "freecell-solver-range-parallel-solve",
        argv => [ "1", "2", "1", "-l", "gi" ],
        msg  => qq{"range-parallel-solve 1 2 1 -l gi" returned no errors}
    },
    'range_parallel_solve__dash_opt' => {
        prog => "freecell-solver-range-parallel-solve",
        argv => [ "1", "2", "1", "-opt" ],
        msg  => qq{"range-parallel-solve 1 2 1 -opt" returned no errors}
    },
    'range_parallel_solve__missing_arg' => {
        prog => "freecell-solver-range-parallel-solve",
        argv => [
            '1', '1', '1',
            '--read-from-file',
            {
                type   => 'catfile',
                prefix => "0,",
                args   => [
                    {
                        type => 'sample_preset',
                        arg  => 'crashy-preset-3.preset'
                    }
                ]
            },
        ],
        msg =>
qq{range-parallel-solve with a preset that ends with an option with a missing argument},
    },
    'range_parallel_solve__mixed_simple_simon' => {
        prog => "freecell-solver-range-parallel-solve",
        argv => [ "1", "1", "1", "-to", "01abc", ],
        msg =>
            qq{range-parallel-solve with mixed Freecell and Simple Simon Tests}
    },
    'range_parallel_solve__next-flare' => {
        prog => "freecell-solver-range-parallel-solve",
        argv => [ "1", "2", "1", "--next-flare" ],
        msg  => qq{"range-parallel-solve 1 2 1 --next-flare" returned no errors}
    },
    'range_parallel_solve__11982_opt' => {
        prog => "freecell-solver-range-parallel-solve",
        argv => [qw(11981 11983 1 -opt)],
        msg  => qq{"range-parallel-solve 11981 11983 1 -opt" returned no errors}
    },
    'range_parallel_solve__1_3_1_flares' => {
        prog => "freecell-solver-range-parallel-solve",
        argv => [
            qw(1 3 1 --flare-name dfs --next-flare --method a-star --flare-name befs --flares-plan),
            q(Run:300@dfs,Run:500@befs,CP:,Run:200@dfs),
        ],
        msg => qq{"range-parallel-solve 1 3 1 flares" returned no errors}
    },
    'range_parallel_solve__1__2__invalid_flares_plan_1' => {
        prog => "freecell-solver-range-parallel-solve",
        argv => [ qw(1 2 1 --flares-plan), q(Run:500@foo) ],
        msg =>
qq{"range-parallel-solve --flares-plan Run:500\@foo" does not crash.}
    },
    'range_parallel_solve__sp_r_tf__not_leak' => {
        prog => "freecell-solver-range-parallel-solve",
        argv => [
            qw(1 2 1 --total-iterations-limit 1000 --method soft-dfs
                -to 0123456789 -sp r:tf)
        ],
        msg => qq{"range-parallel-solve -sp r:tf" does not leak.}
    },
    'range_parallel__invalid_print_step' => {
        prog => "freecell-solver-range-parallel-solve",
        argv => [qw(1 1 0 -l as)],
        msg =>
            qq{Make sure that the program does not crash on a zero print_step},
    },
    'fc_pro_range_solve__invalid_print_step' => {
        prog => "freecell-solver-fc-pro-range-solve",
        argv => [qw(1 2 0 -l as)],
        msg =>
qq{Make sure that the fc-pro program does not crash on a zero print_step},
    },
    'board_gen__pi_make_ms__t_only' => {
        prog => "board_gen/pi-make-microsoft-freecell-board",
        argv => [qw(-t)],
        msg =>
qq{Board generation should not crash with only -t flag (pi-make-ms)},
    },
];

# use YAML::XS qw / DumpFile /;
# DumpFile( 'valg.yaml', $arr );
__PACKAGE__->populate($arr);

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
