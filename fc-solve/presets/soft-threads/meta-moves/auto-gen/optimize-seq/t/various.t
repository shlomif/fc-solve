#!/usr/bin/perl

use strict;
use warnings;

use Test::More tests => 24;
use Test::Trap qw( trap $trap :flow:stderr(systemsafe):stdout(systemsafe):warn );

{
    trap {    
        # TEST
        ok (!system("mono find_opt.exe test_blacklist"));
    };

    my $stderr = $trap->stderr();

    # TEST
    is ($stderr, "", "process.pl did not return any errors on stderr");

    # TEST
    is ($trap->stdout(), "7\n8\n", "Black list is OK.");
}

# TEST:$cnt=0;
sub test_scan_cmd_line
{
    my ($id, $cmd_line) = @_;

    trap {    
        # TEST:$cnt++;
        ok (!system(
                "mono", "find_opt.exe", "test_lookup_scan_cmd_line_by_id", $id
            ),
            "test_lookup_scan_cmd_line_by_id($id) ran successfully."
        );
    };

    # TEST:$cnt++;
    $trap->stderr_is("",
        "test_lookup_scan_cmd_line_by_id($id) did not return any errors."
    );

    # TEST:$cnt++;
    $trap->stdout_is(
        "$cmd_line\n",
        "test_lookup_scan_cmd_line($id) is OK.",
    );

    return;
}
# TEST:$tests_for_scan_cmd_line=$cnt;


# TEST*$tests_for_scan_cmd_line
test_scan_cmd_line(3, "--method random-dfs -seed 2 -to 0[01][23456789]");

# TEST*$tests_for_scan_cmd_line
test_scan_cmd_line(16, "--method a-star -asw 0.2,0.3,0.5,0,0");

# TEST*$tests_for_scan_cmd_line
test_scan_cmd_line(1, "--method soft-dfs -to 0123456789",);

# TEST:$cnt=0;
sub test_lookup_iters
{
    my ($scan_id, $board_idx, $iters_num) = @_;

    my $test_id = "test_lookup_iters($scan_id, $board_idx)";

    trap {
        # TEST:$cnt++;
        ok (!system(
                "mono", "find_opt.exe", "test_lookup_iters", 
                $scan_id, $board_idx,
            ),
             "$test_id ran successfully.",
        );
    };

    # TEST:$cnt++;
    $trap->stderr_is("",
        "$test_id did not return any errors."
    );

    # TEST:$cnt++;
    $trap->stdout_is(
        "$iters_num\n",
        "$test_id returns the right number of iterations.",
    );

    return;
}
# TEST:$lookup_iters=$cnt;

# TEST*$lookup_iters
test_lookup_iters(1, 24 => 136);

# TEST*$lookup_iters
test_lookup_iters(1, 100 => 57282);

# TEST*$lookup_iters
test_lookup_iters(16, 97 => 14139);

# TEST*$lookup_iters
test_lookup_iters(5, 24070 => 83);

