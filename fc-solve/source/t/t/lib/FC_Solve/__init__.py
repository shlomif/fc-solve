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

    # TEST:$input_cmd_line=0;
    def input_cmd_line(self, name, cmd_line_args):

        last_arg = c_int()
        error_string = c_char_p()
        known_params = c_char_p(None)

        cmd_line_args_tuple = tuple(cmd_line_args)

        self.fcs.freecell_solver_user_cmd_line_parse_args(
            self.user, # instance
            len(cmd_line_args),    # argc
            ((c_char_p * len(cmd_line_args))(*cmd_line_args_tuple)),  # argv
            0,   # start_arg
            byref(known_params), # known_params
            None,   # callback
            None,   # callback_context
            byref(error_string),  # error_string
            byref(last_arg)    # last_arg
        )

        # TEST:$input_cmd_line++;
        is_ok(last_arg.value, len(cmd_line_args),
                name + " - assign weights - processed two arguments")

    # TEST:$set_befs=0;
    def _set_befs_weights(self, name, weights_s):
        # TEST:$set_befs=$set_befs+$input_cmd_line;
        self.input_cmd_line(name, ["-asw", weights_s])

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


    def flare_plan_num_items_is(self, name, want_num_items):
        got_num_items = self.fcs.fc_solve_user_INTERNAL_get_flares_plan_num_items(
                self.user
        )

        ok (want_num_items == got_num_items, 
                name + " - got_num_items.")

    # TEST:$flare_plan_item_is_run_indef=0;
    def flare_plan_item_is_run_indef(self, name, item_idx, flare_idx):
        get_plan_type = self.fcs.fc_solve_user_INTERNAL_get_flares_plan_item_type
        get_plan_type.restype = c_char_p
        
        # TEST:$flare_plan_item_is_run_indef++;
        ok (get_plan_type(self.user, item_idx) == "RunIndef",
                name + " - right type")

        got_flare_idx = self.fcs.fc_solve_user_INTERNAL_get_flares_plan_item_flare_idx(self.user, item_idx);

        # TEST:$flare_plan_item_is_run_indef++;
        ok (got_flare_idx == flare_idx,
                name + " - matching flare_idx")

    # TEST:$flare_plan_item_is_run=0;
    def flare_plan_item_is_run(self, name, item_idx, flare_idx, iters_count):
        get_plan_type = self.fcs.fc_solve_user_INTERNAL_get_flares_plan_item_type
        get_plan_type.restype = c_char_p
        
        # TEST:$flare_plan_item_is_run++;
        ok (get_plan_type(self.user, item_idx) == "Run",
                name + " - right type")

        got_flare_idx = self.fcs.fc_solve_user_INTERNAL_get_flares_plan_item_flare_idx(self.user, item_idx);

        # TEST:$flare_plan_item_is_run++;
        ok (got_flare_idx == flare_idx,
                name + " - matching flare_idx")
        
        got_iters_count = self.fcs.fc_solve_user_INTERNAL_get_flares_plan_item_iters_count(self.user, item_idx);

        # TEST:$flare_plan_item_is_run++;
        ok (got_iters_count == iters_count,
                name + " - matching iters_count")

    # TEST:$flare_plan_item_is_checkpoint=0;
    def flare_plan_item_is_checkpoint(self, name, item_idx):
        get_plan_type = self.fcs.fc_solve_user_INTERNAL_get_flares_plan_item_type
        get_plan_type.restype = c_char_p
        
        # TEST:$flare_plan_item_is_checkpoint++;
        ok (get_plan_type(self.user, item_idx) == "CP",
                name + " - right type")

    def num_by_depth_tests_order_is(self, name, want_num):
        got_num = self.fcs.fc_solve_user_INTERNAL_get_num_by_depth_tests_order(
                self.user
        )

        ok (want_num == got_num, name + " - by_depth_tests_order.")

    def by_depth_max_depth_of_depth_idx_is(self, name, depth_idx, want_num):
        got_num = self.fcs.fc_solve_user_INTERNAL_get_by_depth_tests_max_depth(
            self.user,
            depth_idx
        )

        ok (want_num == got_num,
            name + " - max_depth_of_depth_idx_is for" + str(depth_idx) + ".")

    def solve_board(self, board):
        return self.fcs.freecell_solver_user_solve_board(
                self.user,
                (c_char_p)(board)
        )

    def resume_solution(self):
        return self.fcs.freecell_solver_user_resume_solution(self.user)

    def limit_iterations(self, max_iters):
        self.fcs.freecell_solver_user_limit_iterations(
            self.user, 
            (c_int)(max_iters)
        )
        return

