#!/usr/bin/perl

use strict;
use warnings;

use Test::More tests => 25;
use File::Spec;
use lib './t/lib';
use File::Basename qw( dirname );

use Games::Solitaire::FC_Solve::CheckResults;

my $data_dir = File::Spec->catdir( dirname( __FILE__), 'data');

my $digests_storage_fn = File::Spec->catfile(
    $data_dir, 'digests-and-lens-storage.yml'
);

my $verifier = Games::Solitaire::FC_Solve::CheckResults->new(
    {
        data_filename => $digests_storage_fn,
    }
);

sub verify_solution_test
{
    local $Test::Builder::Level = $Test::Builder::Level + 1;

    return $verifier->verify_solution_test(@_);
}

# 24 is my lucky number. (Shlomif)
# TEST
verify_solution_test({id => "freecell24", deal => 24}, "Verifying the solution of deal #24");

# TEST
verify_solution_test({id => "random_dfs_with_rand_parens", deal => 24,
        # theme => ["--method", "random-dfs", "-seed", "1", "-to", "[01][23456789]=rand()"],
        theme => ["--method", "random-dfs", "-seed", "1", "-to", "[01]=rand()[23456789]=rand()"],
    },
    "Verifying the solution of a deal with random-dfs specified using '=rand()'");

# TEST
verify_solution_test(
    {
        id => "375783-dbm-sol",
        deal => 375_783,
        complete_command =>
        "$^X $ENV{FCS_SRC_PATH}/scripts/convert-dbm-fc-solver-solution-to-fc-solve-solution.pl --freecells-num=2 $ENV{FCS_SRC_PATH}/t/t/data/sample-solutions/375783.dbm-sol"
    },
    "Verifying the output of scripts/convert-dbm-fc-solver-solution-to-fc-solve-solution.pl",
);

# TEST
verify_solution_test({id => "freecell1941", deal => 1941}, "Verifying 1941 (The Hardest Deal)");

# TEST
verify_solution_test({id => "freecell24empty", deal => 24, theme => [],},
    "Solving Deal #24 with the default heuristic"
);

# TEST
verify_solution_test({id => "freecell617jgl", deal => 617, theme => ["-l", "john-galt-line"],},
    "Solving Deal #617 with the john-galt-line"
);

# TEST
verify_solution_test({id => "bakers_game24default", deal => 24, variant => "bakers_game", theme => [],},
    "Baker's Game Deal #24"
);

# TEST
verify_solution_test({id => "forecell1099default", deal => 1099, variant => "forecell", theme => [],},
    "Forecell Deal #1099"
);

# TEST
verify_solution_test({id => "relaxed_freecell11982",deal => 11982, variant => "relaxed_freecell", },
    "Relaxed Freecell Deal #11982"
);


# TEST
verify_solution_test(
    {
        id => "seahaven_towers1977fools-gold",
        deal => 1977,
        variant => "seahaven_towers",
        theme => ["-l", "fools-gold",],
    },
    "Seahaven Towers #1977"
);

# TEST
verify_solution_test({
        id => "eight_off200", deal => 200, variant => "eight_off",
    },
    "Eight Off #200 with -l gi"
);

# TEST
verify_solution_test(
    {id =>"eight_off200default", deal => 200, variant => "eight_off",
        theme => [],
    },
    "Eight Off #200 with default heuristic"
);

# TEST
verify_solution_test(
    {id => "simple_simon24default", deal => 24, variant => "simple_simon",
        theme => [],
    },
    "Simple Simon #24 with default theme",
);

# TEST
verify_solution_test(
    {id => "simple_simon19806default", deal => 19806, variant => "simple_simon",
        theme => [],
    },
    "Simple Simon #19806 with default theme",
);

# TEST
verify_solution_test(
    {id => "simple_simon1with_i", deal => 1, variant => "simple_simon",
        theme => ["-to", "abcdefghi"],
    },
    "Simple Simon #1 with seq-to-false-parent",
);

# TEST
verify_solution_test(
    {
        id => "simple_simon1with_next_instance",
        deal => 1,
        variant => "simple_simon",
        theme => ["-to", "abcdefgh", "--next-instance", "-to", "abcdefghi",],
    },
    "Simple Simon #1 using an --next-instance",
);

# TEST
verify_solution_test(
    {
        id => "freecell254076_l_by",
        deal => 254076,
        msdeals => 1,
        theme => ["-l", "by", "--scans-synergy", "dead-end-marks"],
    },
    "Freecell MS 254,076 while using -l by with dead-end-marks"
);

# TEST
verify_solution_test({id => "freecell24", deal => 24,
        output_file => "24.solution.txt",},
    "Verifying the solution of deal No. 24 with -o");

# TEST
verify_solution_test({id => "freecell24_children_playing_ball", deal => 24,
        theme => ["-l", "children-playing-ball"],},
    "Verifying the solution of deal No. 24 with -l cpb");

# TEST
verify_solution_test({id => "freecell24_sentient_pearls", deal => 24,
        theme => ["-l", "sentient-pearls"],},
    "Verifying the solution of deal No. 24 with -l sp");


# TEST
verify_solution_test({id => "freecell24with_empty_soft_thread_name", deal => 1,
        # theme => ["-l", "by"],
        theme => ["-to", "013[2456789]", "-nst", "-l" ,"by",],
    },
    "Solving Deal #24 with an empty soft thread name"
);

# TEST
verify_solution_test(
    {
        id => "freecell1941",
        deal => 1941,
        theme => ["-l", "gi", "--cache-limit", "2000"],
    },
    "Verifying 1941 with --cache-limit set to a different value",
);

#
# We discovered that Freecell Solver did not properly handle the foundations
# with 0's like "H-0" , "S-0", etc. This is as opposed to what exists in the
# README.txt file.
#
# This test aims to fix it.

# TEST
verify_solution_test({id => "freecell24_board_with_founds_0", deal => 24,
        board => File::Spec->catfile(
            $data_dir, 'sample-boards',
            'ms24-with-founds-0.txt'
        )
    }, "Properly handle foundations like H-0 S-0 etc.");

# TEST
verify_solution_test({
    id => "qualified_seed_for_6240",
    deal => 6240,
    theme => ["-l", "qs", "-fif", "5", "--flares-choice", "fcpro",],
    },
    "Qualified seed test with -fif and --flares-choice fcpro",
);

# TEST
verify_solution_test(
    {
        id => "freecell24",
        deal => 24,
        board => File::Spec->catfile(
            $ENV{FCS_PATH}, '24.no-newline.board',
        ),
    },
    "Handle initial layouts without a trailing newline."
);

# Store the changes at the end so they won't get lost.
$verifier->end();


=head1 COPYRIGHT AND LICENSE

Copyright (c) 2008 Shlomi Fish

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

