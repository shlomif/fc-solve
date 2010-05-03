from TAP.Simple import *
from ctypes import *

class FC_Solve:
    # TEST:$num_befs_weights=5;
    NUM_BEFS_WEIGHTS = 5

    def __init__(self):
        self.fcs = CDLL("../libfreecell-solver.so")

        self.user = self.fcs.freecell_solver_user_alloc()

        self.get_befs_weight = self.fcs.fc_solve_user_INTERNAL_get_befs_weight

        self.get_befs_weight.restype = c_double

    # TEST:$set_befs=0;
    def _set_befs_weights(self, name, weights_s):

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
        
        # TEST:$set_befs++;
        is_ok(last_arg.value, 2,
                name + " - assign weights - processed two arguments");

    def __destroy__(self):
        self.fcs.freecell_solver_user_free(self.user);

    # TEST:$test_befs=0;
    def test_befs_weights(self, name, string, weights):

        # TEST:$test_befs=$test_befs+$set_befs;
        self._set_befs_weights(name, string)

        for idx in range(0, self.NUM_BEFS_WEIGHTS):
            top = bottom = weights[idx]
            # floating-point values.
            if (top != int(top) + 0.0):
                top = top + 1e-6
                bottom = bottom - 1e-6
                
            have = self.get_befs_weight(self.user, idx)
            # TEST:$test_befs=$test_befs+$num_befs_weights;
            if (not ok((bottom <= have) and (have <= top), \
                    name + " - Testing Weight No. " + str(idx))):
                diag("Should be: [" + str(bottom) + "," + str(top) + "] ; " +
                        "Is: " + str(have))

    # TEST:$compile_flares_plan_ok=0;
    def compile_flares_plan_ok(self, name, flares_plan_string):
        instance_list_index = c_int()
        error_string = c_char_p()

        ret_code = self.fcs.freecell_solver_user_set_flares_plan(
                self.user,
                c_char_p(flares_plan_string)
                )

        # TEST:$compile_flares_plan_ok++;
        ok(ret_code == 0,
                name + " - set_string returned success")
        ret_code = self.fcs.fc_solve_user_INTERNAL_compile_all_flares_plans(
            self.user,
            byref(instance_list_index),
            byref(error_string)
        )
        
        # TEST:$compile_flares_plan_ok++;
        ok(ret_code == 0,
                name + " - returned success.")

        # TEST:$compile_flares_plan_ok++;
        ok(instance_list_index.value == -1,
                name + " - instance_list_index returned -1.")

        # TEST:$compile_flares_plan_ok++;
        ok(error_string.value == None,
                name + " - error_string returned NULL.")
