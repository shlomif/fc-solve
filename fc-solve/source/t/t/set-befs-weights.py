#!/usr/bin/env python

import sys
sys.path.append("./t/lib");

from TAP.Simple import *
# TEST:source "$^CURRENT_DIRNAME/lib/FC_Solve/__init__.py"
from FC_Solve import FC_Solve

# Should be 24.
plan(24)

def main():
    fcs = FC_Solve()

    # TEST*$test_befs
    fcs.test_befs_weights("Simple - all integers",
            "5,4,3,0,2",
            [5.0, 4.0, 3.0, 0.0, 2.0])

    # TEST*$test_befs
    fcs.test_befs_weights("Simple - fractions",
            "0.2,0.3,0.4,0.5,0.6",
            [0.2,0.3,0.4,0.5,0.6])

    # TEST*$test_befs
    fcs.test_befs_weights("Testing end after three numbers",
            "0.2,0.35,0.4\x00500\x00600.7\x00",
            [0.2,0.35,0.4,0,0])

    # TEST*$test_befs
    fcs.test_befs_weights("One number with a trailing comma",
            "100,\x002,3,4,5,6,7,8,9,100\x00",
            [100,0,0,0,0])

#----------------------------------------------------------------------

if __name__ == "__main__":
    main()

