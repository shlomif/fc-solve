#!/usr/bin/env python

import sys
sys.path.append("./t/lib");

from TAP.Simple import *
from ctypes import *

plan(3)

def main():
    fcs = CDLL("../libfreecell-solver.so")
    libc = CDLL("libc.so.6")

    # TEST
    ok(1, "Dummy assertion")

    user = fcs.freecell_solver_user_alloc()

    last_arg = c_int()
    error_string = c_char_p()
    known_params = c_char_p(None)
    fcs.freecell_solver_user_cmd_line_parse_args(
        user, # instance
        2,    # argc
        ((c_char_p * 2)("-asw", "5,4,3,2,1")),  # argv
        0,   # start_arg
        byref(known_params), # known_params
        None,   # callback
        None,   # callback_context
        byref(error_string),  # error_string
        byref(last_arg)    # last_arg
    )
    
    # TEST
    is_ok(last_arg.value, 2, "Processed two arguments");

    get_weight = fcs.fc_solve_user_INTERNAL_get_befs_weight

    get_weight.restype = c_double

    weight = get_weight(user, 0)

    # TEST
    ok((weight == 5.0), "weight[0] is 5");

    fcs.freecell_solver_user_free(user);

#----------------------------------------------------------------------

if __name__ == "__main__":
    main()

