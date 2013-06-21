#!/usr/bin/perl

use strict;
use warnings;

use v5.016;

use Test::More tests => 11;
use Carp;
use Data::Dumper;
use String::ShellQuote;
use File::Spec;
use File::Temp qw( tempdir );


use Games::Solitaire::Verify::Solution;

sub test_using_valgrind
{
    my $args = shift;

    my $log_fn = "valgrind.log";

    if (ref($args) eq "ARRAY")
    {
        die "ArrayRef is no longer supported.";
    }

    # $args = { argv => $args, prog => "freecell-solver-range-parallel-solve", };

    my $id = $args->{id};

    if (!defined($id))
    {
        Carp::confess("Missing id");
    }
    state %ids;

    if ($ids{$id}++)
    {
        die "Duplicate ID '$id'";
    }

    my $cmd_line_args = $args->{argv};
    my $prog = $args->{prog};

    my $blurb = $args->{blurb};
    if ((!defined($blurb)) or ref($blurb) ne '' or length($blurb) == 0)
    {
        Carp::confess("Blurb is wrong.");
    }

    system(
        "valgrind",
        "--track-origins=yes",
        "--leak-check=yes",
        "--log-file=$log_fn",
        $ENV{'FCS_PATH'} . "/$prog",
        @$cmd_line_args,
    );

    open my $read_from_valgrind, "<", $log_fn
        or die "Cannot open valgrind.log for reading";
    my $found_error_summary = 0;
    my $found_malloc_free = 0;
    LINES_LOOP:
    while (my $l = <$read_from_valgrind>)
    {
        if (index($l, q{ERROR SUMMARY: 0 errors from 0 contexts}) >= 0)
        {
            $found_error_summary = 1;
        }
        elsif (index($l, q{in use at exit: 0 bytes}) >= 0)
        {
            $found_malloc_free = 1;
        }
    }
    close ($read_from_valgrind);

    if (ok (($found_error_summary && $found_malloc_free), $blurb))
    {
        unlink($log_fn);
    }
    else
    {
        die "Valgrind failed";
    }
}

{
    my $temp_dir = tempdir (CLEANUP => 1);
    # TEST
    test_using_valgrind(
        {
            id => "dbm_fc_solver_1",
            prog => "dbm_fc_solver",
            argv =>
            [
                '--offload-dir-path', $temp_dir,
                File::Spec->catfile(
                    $ENV{FCS_SRC_PATH}, 't', 't', 'data',
                    'sample-boards', '2freecells-24-mid-with-colons.board'
                ),
            ],
            blurb => qq{dbm_fc_solver from 24-mid-with-colons.},
        },
    );
}

# TEST
test_using_valgrind(
    {
        id => "fc-solve-crashy-preset-1",
        prog => "fc-solve",
        argv => [
            '--read-from-file', "4,$ENV{FCS_SRC_PATH}/t/t/data/presets/crashy-preset-1.preset",
            qw(-s -i -p -t -sam),
            "$ENV{FCS_PATH}/24.board",
        ],
        blurb => "Check the sanity of crashy-preset-1 which over-rides the soft-thread several times.",

    },
);

# TEST
test_using_valgrind(
    {
        id => "fc-solve-trim-max-stored-states",
        prog=>"fc-solve",
        argv => [
            qw(--method soft-dfs --st-name dfs -nst --method a-star --st-name befs --trim-max-stored-states 100 --prelude) ,
            '200@befs,100@dfs,1000@befs,500000@dfs',
            qw(-s -i -p -t -sam -mi 3000), "$ENV{FCS_PATH}/1941.board",
        ],
        blurb => "Check the sanity of --trim-max-stored-states.",
    },
);

# TEST
test_using_valgrind(
    {
        id => "range_parallel_solve_befs",
        prog => "freecell-solver-range-parallel-solve",
        argv => [qw(1 2 1 --method a-star)],
        blurb => qq{range-parallel-solve for board #2 using the BeFS method valgrind}
    },
);

# TEST
test_using_valgrind(
    {
        id => "range_parallel_solve_l_gi",
        prog => "freecell-solver-range-parallel-solve",
        argv => ["1", "2", "1", "-l", "gi"],
        blurb => qq{"range-parallel-solve 1 2 1 -l gi" returned no errors}
    },
);

# TEST
test_using_valgrind(
    {
        id => "range_parallel_solve__dash_opt",
        prog => "freecell-solver-range-parallel-solve",
        argv => ["1", "2", "1", "-opt"],
        blurb => qq{"range-parallel-solve 1 2 1 -opt" returned no errors}
    },
);

# TEST
test_using_valgrind(
    {
        id => "range_parallel_solve__next-flare",
        prog => "freecell-solver-range-parallel-solve",
        argv => ["1", "2", "1", "--next-flare"],
        blurb => qq{"range-parallel-solve 1 2 1 --next-flare" returned no errors}
    },
);

# TEST
test_using_valgrind(
    {
        id => "range_parallel_solve__11982_opt",
        prog => "freecell-solver-range-parallel-solve",
        argv => [qw(11981 11983 1 -opt)],
        blurb => qq{"range-parallel-solve 11981 11983 1 -opt" returned no errors}
    },
);

# TEST
test_using_valgrind(
    {
        id => "range_parallel_solve__1_3_1_flares",
        prog => "freecell-solver-range-parallel-solve",
        argv => [qw(1 3 1 --flare-name dfs --next-flare --method a-star --flare-name befs --flares-plan), q(Run:300@dfs,Run:500@befs,CP:,Run:200@dfs),],
        blurb => qq{"range-parallel-solve 1 3 1 flares" returned no errors}
    },
);

# TEST
test_using_valgrind(
    {
        id => "range_parallel_solve__1__2__invalid_flares_plan_1",
        prog => "freecell-solver-range-parallel-solve",
        argv => [qw(1 2 1 --flares-plan), q(Run:500@foo)],
        blurb => qq{"range-parallel-solve --flares-plan Run:500\@foo" does not crash.}
    },
);

# TEST
test_using_valgrind(
    {
        id => "range_parallel_solve__sp_r_tf__not_leak",
        prog => "freecell-solver-range-parallel-solve",
        argv => [qw(1 2 1 --total-iterations-limit 1000 --method soft-dfs
            -to 0123456789 -sp r:tf)
        ],
        blurb => qq{"range-parallel-solve -sp r:tf" does not leak.}
    },
);

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

