#!/usr/bin/env python

import sys
sys.path.append("./t/lib");

from TAP.Simple import *
# TEST:source "$^CURRENT_DIRNAME/lib/FC_Solve/__init__.py"
from FC_Solve import FC_Solve

plan(77)

def test_null_plan():
    fcs = FC_Solve()

    name = "null plan"

    # TEST*$compile_flares_plan_ok
    fcs.compile_flares_plan_ok(name, None)

    # TEST
    fcs.flare_plan_num_items_is(name, 1);

    # TEST*$flare_plan_item_is_run_indef
    fcs.flare_plan_item_is_run_indef(name, 0, 0);

def test_empty_plan():

    fcs = FC_Solve()

    name = "empty string plan"

    # TEST*$compile_flares_plan_ok
    fcs.compile_flares_plan_ok(name, "")

    # TEST
    fcs.flare_plan_num_items_is(name, 1);

    # TEST*$flare_plan_item_is_run_indef
    fcs.flare_plan_item_is_run_indef(name, 0, 0);

def test_two_runs():
    # Create a fresh instance
    fcs = FC_Solve()

    name = "Two Run's"
    # TEST*$input_cmd_line
    fcs.input_cmd_line("Input Flares", 
        ["--flare-name", "dfs", "-nf",
         "--flare-name", "befs", "--method", "a-star"])

    # TEST*$compile_flares_plan_ok
    fcs.compile_flares_plan_ok(name, "Run:500@befs,Run:1500@dfs")

    # 2 runs and then the implicit checkpoint.
    # TEST
    fcs.flare_plan_num_items_is(name, 3);

    # TEST*$flare_plan_item_is_run
    fcs.flare_plan_item_is_run(name + " No. 0", 0, 1, 500);

    # TEST*$flare_plan_item_is_run
    fcs.flare_plan_item_is_run(name + " No. 1", 1, 0, 1500);

    # TEST*$flare_plan_item_is_checkpoint
    fcs.flare_plan_item_is_checkpoint(name + " No. 2", 2);

def test_with_checkpoints():    
    testname = "With checkpoints"

    fcs = FC_Solve()

    # TEST*$input_cmd_line
    fcs.input_cmd_line("Input Flares", 
        ["--flare-name", "dfs", "-nf",
         "--flare-name", "befs", "--method", "a-star", "-nf",
         "--flare-name", "foo", "--method", "a-star",
            "-asw", "0.2,0.3,0.5,0,0", "-nf",
         "--flare-name", "bar", "-to", "[01][23467]",
         ])

    # TEST*$compile_flares_plan_ok
    fcs.compile_flares_plan_ok(testname, "Run:500@dfs,Run:300@bar,CP:,Run:1000@befs")

    # 4 items and then the implicit checkpoint.
    # TEST
    fcs.flare_plan_num_items_is(testname, 5);

    # TEST*$flare_plan_item_is_run
    fcs.flare_plan_item_is_run(("%s No. 0" % (testname)), 0, 0, 500);

    # TEST*$flare_plan_item_is_run
    fcs.flare_plan_item_is_run(("%s No. 1" % (testname)), 1, 3, 300);

    # TEST*$flare_plan_item_is_checkpoint
    fcs.flare_plan_item_is_checkpoint(("%s No. 2" % (testname)), 2);
    
    # TEST*$flare_plan_item_is_run
    fcs.flare_plan_item_is_run(("%s No. 3" % (testname)), 3, 1, 1000);
    
    # TEST*$flare_plan_item_is_checkpoint
    fcs.flare_plan_item_is_checkpoint(("%s No. 4" % (testname)), 4);



def test_with_checkpoints_and_explicit_checkpoint():
    testname = "With checkpoints with explicit checkpoint at end."

    fcs = FC_Solve()

    # TEST*$input_cmd_line
    fcs.input_cmd_line("Input Flares", 
        ["--flare-name", "dfs", "-nf",
         "--flare-name", "befs", "--method", "a-star", "-nf",
         "--flare-name", "foo", "--method", "a-star",
            "-asw", "0.2,0.3,0.5,0,0", "-nf",
         "--flare-name", "bar", "-to", "[01][23467]",
         ])


    # TEST*$compile_flares_plan_ok
    fcs.compile_flares_plan_ok(testname, "Run:500@dfs,Run:300@bar,CP:,Run:1000@befs,CP:")

    # 5 items (without an extra and redundant explicit checkpoint at the end.)
    # TEST
    fcs.flare_plan_num_items_is(testname, 5);

    # TEST*$flare_plan_item_is_run
    fcs.flare_plan_item_is_run(("%s No. 0" % (testname)), 0, 0, 500);

    # TEST*$flare_plan_item_is_run
    fcs.flare_plan_item_is_run(("%s No. 1" % (testname)), 1, 3, 300);

    # TEST*$flare_plan_item_is_checkpoint
    fcs.flare_plan_item_is_checkpoint(("%s No. 2" % (testname)), 2);
    
    # TEST*$flare_plan_item_is_run
    fcs.flare_plan_item_is_run(("%s No. 3" % (testname)), 3, 1, 1000);
    
    # TEST*$flare_plan_item_is_checkpoint
    fcs.flare_plan_item_is_checkpoint(("%s No. 4" % (testname)), 4);




def test_with_run_indef():
    testname = "With RunIndef"

    fcs = FC_Solve()

    # TEST*$input_cmd_line
    fcs.input_cmd_line("Input Flares", 
        ["--flare-name", "dfs", "-nf",
         "--flare-name", "befs", "--method", "a-star", "-nf",
         "--flare-name", "foo", "--method", "a-star",
            "-asw", "0.2,0.3,0.5,0,0", "-nf",
         "--flare-name", "bar", "-to", "[01][23467]",
         ])

    # TEST*$compile_flares_plan_ok
    fcs.compile_flares_plan_ok(testname, "Run:500@dfs,Run:300@bar,CP:,RunIndef:befs")

    # 5 items (with an extra implicit chechpoint at the end).
    # TEST
    fcs.flare_plan_num_items_is(testname, 5);

    # TEST*$flare_plan_item_is_run
    fcs.flare_plan_item_is_run(("%s No. 0" % (testname)), 0, 0, 500);

    # TEST*$flare_plan_item_is_run
    fcs.flare_plan_item_is_run(("%s No. 1" % (testname)), 1, 3, 300);

    # TEST*$flare_plan_item_is_checkpoint
    fcs.flare_plan_item_is_checkpoint(("%s No. 2" % (testname)), 2);
    
    # TEST*$flare_plan_item_is_run_indef
    fcs.flare_plan_item_is_run_indef(("%s No. 3" % (testname)), 3, 1);
    
    # TEST*$flare_plan_item_is_checkpoint
    fcs.flare_plan_item_is_checkpoint(("%s No. 4" % (testname)), 4);


def main():

    test_null_plan()

    test_empty_plan()

    test_two_runs()

    test_with_checkpoints()

    test_with_checkpoints_and_explicit_checkpoint()

    test_with_run_indef()

#----------------------------------------------------------------------

if __name__ == "__main__":
    main()

