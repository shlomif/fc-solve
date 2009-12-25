#!/usr/bin/perl

use strict;
use warnings;

use Test::More tests => 27;
use Test::Trap qw( trap $trap :flow:stderr(systemsafe):stdout(systemsafe):warn );

# TEST:$cnt=0;
sub test_find_opt_command_output
{
    my ($cmd, $cmd_args, $output) = @_;

    my $test_id = "$cmd(" . join(",", @$cmd_args) . ")";

    trap {    
        # TEST:$cnt++;
        ok (!system(
                "mono", "find_opt.exe", $cmd, @$cmd_args,
            ),
            "$test_id ran successfully."
        );
    };

    # TEST:$cnt++;
    $trap->stderr_is("",
        "$test_id did not return any errors."
    );

    # TEST:$cnt++;
    $trap->stdout_is(
        "$output\n",
        "$test_id output is OK.",
    );

    return;
}
# TEST:$test_find_opt_command_output=$cnt;

{

    # TEST*$test_find_opt_command_output
    test_find_opt_command_output(
        "test_blacklist",
        [],
        "7\n8",
    );
}

sub test_scan_cmd_line
{
    my ($id, $cmd_line) = @_;

    return test_find_opt_command_output(
        "test_lookup_scan_cmd_line_by_id",
        [ $id, ],
        $cmd_line,
    );
}

# TEST:$tests_for_scan_cmd_line=$test_find_opt_command_output;


# TEST*$tests_for_scan_cmd_line
test_scan_cmd_line(3, "--method random-dfs -seed 2 -to 0[01][23456789]");

# TEST*$tests_for_scan_cmd_line
test_scan_cmd_line(16, "--method a-star -asw 0.2,0.3,0.5,0,0");

# TEST*$tests_for_scan_cmd_line
test_scan_cmd_line(1, "--method soft-dfs -to 0123456789",);

sub test_lookup_iters
{
    my ($scan_id, $board_idx, $iters_num) = @_;
    
    return test_find_opt_command_output(
        "test_lookup_iters",
        [ $scan_id, $board_idx, ],
        $iters_num,
    );
}
# TEST:$lookup_iters=$test_find_opt_command_output;

# TEST*$lookup_iters
test_lookup_iters(1, 24 => 136);

# TEST*$lookup_iters
test_lookup_iters(1, 100 => 57282);

# TEST*$lookup_iters
test_lookup_iters(16, 97 => 14139);

# TEST*$lookup_iters
test_lookup_iters(5, 24070 => 83);

sub test_process_sample_run
{
    my ($output) = @_;
    return test_find_opt_command_output(
        "test_process_sample_run",
        [],
        $output,
    );
}
# TEST:$test_process_sample_run=$test_find_opt_command_output;

sub strip_newlines
{
    my $s = shift;

    $s =~ s/\n//g;

    return $s;
}

# TEST*$test_process_sample_run
test_process_sample_run(
    strip_newlines(<<'EOF'),
350@2,350@5,350@9,350@20,350@2,350@12,350@3,350@5,350@4,350@9,350@15,
350@2,350@18,350@12,350@5,350@11,700@10,350@4,350@5,350@3,350@15,350@18,
350@15,700@5,350@18,350@3,350@9,350@18,1400@5,350@15,350@16,1050@10,
350@11,1050@16,1050@17,350@18,1050@12,1400@4,1050@18,1050@17,
1400@10,2100@11,3500@17,4900@12,6300@1,6300@20
EOF
);

