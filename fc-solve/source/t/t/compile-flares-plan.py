#!/usr/bin/env python

import sys
sys.path.append("./t/lib");

from TAP.Simple import *
# TEST:source "$^CURRENT_DIRNAME/lib/FC_Solve/__init__.py"
from FC_Solve import FC_Solve

plan(8)

def main():
    fcs = FC_Solve()

    # TEST*$compile_flares_plan_ok
    fcs.compile_flares_plan_ok("null plan", None)

    # TEST*$compile_flares_plan_ok
    fcs.compile_flares_plan_ok("null plan", "")

#----------------------------------------------------------------------

if __name__ == "__main__":
    main()

