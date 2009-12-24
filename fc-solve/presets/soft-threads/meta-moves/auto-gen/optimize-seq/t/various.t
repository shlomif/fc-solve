#!/usr/bin/perl

use strict;
use warnings;

use Test::More tests => 6;
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

{
    trap {    
        # TEST
        ok (!system("mono find_opt.exe test_lookup_scan_cmd_line_by_id 3"));
    };

    # TEST
    $trap->stderr_is("", 
        "test_lookup_scan_cmd_line did not return any errors."
    );

    # TEST
    $trap->stdout_is(
        "--method random-dfs -seed 2 -to 0[01][23456789]\n", 
        "test_lookup_scan_cmd_line(3) is OK.",
    );
}
