#!/usr/bin/env python

import sys
sys.path.append("./t/lib");

from TAP.Simple import *
# TEST:source "$^CURRENT_DIRNAME/lib/FC_Solve/__init__.py"
from FC_Solve import FC_Solve

plan(7)

def test_1():
    testname = "test_1"

    fcs = FC_Solve()

    # TEST*$input_cmd_line
    fcs.input_cmd_line("dto 1",
        ["-to", "0123456789",
            "-dto", "1,0132456789",
        ]
    )

    # TEST
    fcs.num_by_depth_tests_order_is(testname, 2)

    # TEST
    fcs.by_depth_max_depth_of_depth_idx_is(testname, 0, 1);


def test_2():
    testname = "test_1"

    fcs = FC_Solve()

    # TEST*$input_cmd_line
    fcs.input_cmd_line("dto 1",
        ["-to", "0123456789",
            "-dto", "1,0132456789",
            "-dto", "50,056234",
        ]
    )

    # TEST
    fcs.num_by_depth_tests_order_is(testname, 3)

    # TEST
    fcs.by_depth_max_depth_of_depth_idx_is(testname, 0, 1);

    # TEST
    fcs.by_depth_max_depth_of_depth_idx_is(testname, 1, 50);

def main():

    test_1()

    test_2()

#----------------------------------------------------------------------

if __name__ == "__main__":
    main()

