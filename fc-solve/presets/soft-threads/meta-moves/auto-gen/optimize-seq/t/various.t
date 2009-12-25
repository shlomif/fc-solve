#!/usr/bin/perl

use strict;
use warnings;

use Test::More tests => 12;
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
}
# TEST:$tests_for_scan_cmd_line=$cnt;


# TEST*$tests_for_scan_cmd_line
test_scan_cmd_line(3, "--method random-dfs -seed 2 -to 0[01][23456789]");

# TEST*$tests_for_scan_cmd_line
test_scan_cmd_line(16, "--method a-star -asw 0.2,0.3,0.5,0,0");

# TEST*$tests_for_scan_cmd_line
test_scan_cmd_line(1, "--method soft-dfs -to 0123456789",);

