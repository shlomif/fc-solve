#!/usr/bin/perl

use strict;
use warnings;

use Test::TrailingSpace ();
use Test::More tests => 2;

foreach my $path ( @ENV{qw/FCS_SRC_PATH FCS_PATH/} )
{
    my $finder = Test::TrailingSpace->new(
        {
            root              => $path,
            filename_regex    => qr/./,
            abs_path_prune_re => qr#CMakeFiles|CTestTestfile\.cmake|_Inline|
(?:\.(?:a|dll|exe|gz|o|patch|pyc|xcf)\z) |
(?:(?:\A|/|\\)valgrind\S*\.log\z) |
(?:(?:\A|/|\\)(?:dbm_fc_solver|depth_dbm_fc_solver|fc-solve|fc-solve-multi|fc-solve-pruner|freecell-solver-fc-pro-range-solve|freecell-solver-fork-solve|freecell-solver-multi-thread-solve|freecell-solver-range-parallel-solve|libfcs_dbm_calc_derived_test\.so|libfcs_debondt_delta_states_test\.so|libfcs_delta_states_test\.so|libfcs_fcc_brfs_test\.so|libfcs_find_deal\.so|libfcs_gen_ms_freecell_boards\.so|libfreecell-solver\.so(?:[\.0-9]*)|measure-depth-dep-tests-order-perf|pi-make-microsoft-freecell-board|pseudo_dfs_fc_solver|split_fcc_fc_solver|summary-fc-solve|lextab\.py|yacctab\.py)\z)
#msx,
        }
    );

    # TEST*2
    $finder->no_trailing_space("No trailing whitespace was found.");
}
