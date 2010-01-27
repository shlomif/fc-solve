#!/usr/bin/env python

import sys
sys.path.append("./t/lib");

from TAP.Simple import *
from ctypes import *

plan(7)

class FC_Solve:
    # TEST:$num_befs_weights=5;
    NUM_BEFS_WEIGHTS = 5

    def __init__(self):
        self.fcs = CDLL("../libfreecell-solver.so")

        self.user = self.fcs.freecell_solver_user_alloc()

        self.get_befs_weight = self.fcs.fc_solve_user_INTERNAL_get_befs_weight

        self.get_befs_weight.restype = c_double

    # TEST:$set_befs_w_cnt=0;
    def _set_befs_weights(self, weights_s):

        last_arg = c_int()
        error_string = c_char_p()
        known_params = c_char_p(None)
        self.fcs.freecell_solver_user_cmd_line_parse_args(
            self.user, # instance
            2,    # argc
            ((c_char_p * 2)("-asw", weights_s)),  # argv
            0,   # start_arg
            byref(known_params), # known_params
            None,   # callback
            None,   # callback_context
            byref(error_string),  # error_string
            byref(last_arg)    # last_arg
        )
        
        # TEST:$set_befs_w_cnt++;
        is_ok(last_arg.value, 2, "Processed two arguments");

    def __destroy__(self):
        self.fcs.freecell_solver_user_free(self.user);

    # TEST:$test_befs=0;
    def _test_befs_weights(self, name, weights):
        for idx in range(0, self.NUM_BEFS_WEIGHTS):
            top = weights[idx]
            bottom = top
            have = self.get_befs_weight(self.user, idx)
            # TEST:$test_befs+=$num_befs_weights;
            if (not ok((have == top), \
                    name + " - Testing Weight No. " + str(idx))):
                diag("Should be: [" + str(bottom) + "," + str(top) + "] ; " +
                        "Is: " + str(have))

def main():
    # TEST
    ok(1, "Dummy assertion")

    fcs = FC_Solve()

    # TEST*$set_befs_w_cnt
    fcs._set_befs_weights("5,4,3,0,2")

    # TEST*$test_befs
    fcs._test_befs_weights("Simple - all integers", [5.0, 4.0, 3.0, 0.0, 2.0])


#----------------------------------------------------------------------

if __name__ == "__main__":
    main()

