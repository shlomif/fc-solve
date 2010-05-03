#!/usr/bin/env python

import sys
sys.path.append("./t/lib");

from TAP.Simple import *
# TEST:source "$^CURRENT_DIRNAME/lib/FC_Solve/__init__.py"
from FC_Solve import FC_Solve

plan(12)

def main():
    fcs = FC_Solve()

    # TEST*$compile_flares_plan_ok
    fcs.compile_flares_plan_ok("null plan", None)

    # TEST*$flare_plan_item_is_run_indef
    fcs.flare_plan_item_is_run_indef("null plan", 0, 0);

    # TEST*$compile_flares_plan_ok
    fcs.compile_flares_plan_ok("empty string plan", "")

    # TEST*$flare_plan_item_is_run_indef
    fcs.flare_plan_item_is_run_indef("empty string plan", 0, 0);

#----------------------------------------------------------------------

if __name__ == "__main__":
    main()

