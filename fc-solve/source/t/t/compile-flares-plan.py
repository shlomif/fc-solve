#!/usr/bin/env python

import sys
sys.path.append("./t/lib");

from TAP.Simple import *
# TEST:source "$^CURRENT_DIRNAME/lib/FC_Solve/__init__.py"
from FC_Solve import FC_Solve

plan(43)

def main():
    fcs = FC_Solve()

    # TEST*$compile_flares_plan_ok
    fcs.compile_flares_plan_ok("null plan", None)

    # TEST
    fcs.flare_plan_num_items_is("null plan", 1);

    # TEST*$flare_plan_item_is_run_indef
    fcs.flare_plan_item_is_run_indef("null plan", 0, 0);

    # TEST*$compile_flares_plan_ok
    fcs.compile_flares_plan_ok("empty string plan", "")

    # TEST*$flare_plan_item_is_run_indef
    fcs.flare_plan_item_is_run_indef("empty string plan", 0, 0);

    # Create a fresh instance
    fcs = FC_Solve()

    # TEST*$input_cmd_line
    fcs.input_cmd_line("Input Flares", 
        ["--flare-name", "dfs", "-nf",
         "--flare-name", "befs", "--method", "a-star"])

    # TEST*$compile_flares_plan_ok
    fcs.compile_flares_plan_ok("two Run's", "Run:500@befs,Run:1500@dfs")

    # 2 runs and then the implicit checkpoint.
    # TEST
    fcs.flare_plan_num_items_is("two Run's", 3);

    # TEST*$flare_plan_item_is_run
    fcs.flare_plan_item_is_run("two Run's No. 0", 0, 1, 500);

    # TEST*$flare_plan_item_is_run
    fcs.flare_plan_item_is_run("two Run's No. 1", 1, 0, 1500);

    # TEST*$flare_plan_item_is_checkpoint
    fcs.flare_plan_item_is_checkpoint("two Run's No. 2", 2);
    
    fcs = FC_Solve()

    # TEST*$input_cmd_line
    fcs.input_cmd_line("Input Flares", 
        ["--flare-name", "dfs", "-nf",
         "--flare-name", "befs", "--method", "a-star", "-nf",
         "--flare-name", "foo", "--method", "a-star",
            "-asw", "0.2,0.3,0.5,0,0", "-nf",
         "--flare-name", "bar", "-to", "[01][23467]",
         ])

    testname = "With checkpoints"
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

#----------------------------------------------------------------------

if __name__ == "__main__":
    main()

