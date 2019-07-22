from freecell_solver import FreecellSolver


class FreecellSolverTestSuite(FreecellSolver):
    def __init__(self, ut):
        FreecellSolver.__init__(self)
        self.ffi.cdef('''
double fc_solve_user_INTERNAL_get_befs_weight(void * user, int idx);
int freecell_solver_user_INTERNAL_compile_all_flares_plans(void * instance,
char * * s);
int fc_solve_user_INTERNAL_get_flares_plan_num_items(void * i);
char * fc_solve_user_INTERNAL_get_flares_plan_item_type(void * i,
int item_idx);
int fc_solve_user_INTERNAL_get_flares_plan_item_flare_idx(void * i,
int item_idx);
int fc_solve_user_INTERNAL_get_flares_plan_item_iters_count(void * i,
int item_idx);
int fc_solve_user_INTERNAL_compile_all_flares_plans(void * i,
char **error_string);
int fc_solve_user_INTERNAL_get_num_by_depth_tests_order(
    void * api_instance);
int fc_solve_user_INTERNAL_get_by_depth_tests_max_depth(
    void * api_instance, int depth_idx);
        ''')
        self.unittest = ut

    def _get_plan_type(self, item_idx):
        return self.ffi.string(
            self.lib.fc_solve_user_INTERNAL_get_flares_plan_item_type(
                self.user, item_idx))

    # TEST:$input_cmd_line=0;
    def input_cmd_line__test(self, name, cmd_line_args):
        ret = self.input_cmd_line(cmd_line_args)
        # TEST:$input_cmd_line++;
        self._eq(
            ret['last_arg'], ret['cmd_line_args_len'],
            name + " - assign weights - processed two arguments")

    def _eq(self, x, y, blurb):
        return self.unittest.assertEqual(x, y, blurb)

    # TEST:$compile_flares_plan_ok=0;
    def compile_flares_plan_ok(self, name, flares_plan_string):
        error_string = self.ffi.new('char * *')

        myplan = self.ffi.NULL
        if flares_plan_string:
            myplan = bytes(flares_plan_string, 'UTF-8')
        ret_code = self.lib.freecell_solver_user_set_flares_plan(
            self.user,
            myplan
        )

        # TEST:$compile_flares_plan_ok++;
        self._eq(ret_code, 0, name + " - set_string returned success")
        ret_code = self.lib.fc_solve_user_INTERNAL_compile_all_flares_plans(
            self.user,
            error_string
        )

        # TEST:$compile_flares_plan_ok++;
        self._eq(ret_code, 0, name + " - returned success.")

        # TEST:$compile_flares_plan_ok++;
        self._eq(error_string[0], self.ffi.NULL,
                 name + " - error_string returned NULL.")

    def flare_plan_num_items_is(self, name, want_num_items):
        got_num_items = \
            self.lib.fc_solve_user_INTERNAL_get_flares_plan_num_items(
                self.user
            )

        self._eq(want_num_items, got_num_items, name + " - got_num_items.")

    # TEST:$flare_plan_item_is_run_indef=0;
    def flare_plan_item_is_run_indef(self, name, item_idx, flare_idx):
        # TEST:$flare_plan_item_is_run_indef++;
        self._eq(self._get_plan_type(item_idx), b"RunIndef",
                 name + " - right type")

        got_flare_idx = \
            self.lib.fc_solve_user_INTERNAL_get_flares_plan_item_flare_idx(
                self.user,
                item_idx
            )

        # TEST:$flare_plan_item_is_run_indef++;
        self._eq(got_flare_idx, flare_idx, name + " - matching flare_idx")

    # TEST:$flare_plan_item_is_run=0;
    def flare_plan_item_is_run(self, name, item_idx, flare_idx, iters_count):
        # TEST:$flare_plan_item_is_run++;
        self._eq(self._get_plan_type(item_idx), b"Run",
                 name + " - right type")

        got_flare_idx = \
            self.lib.fc_solve_user_INTERNAL_get_flares_plan_item_flare_idx(
                self.user,
                item_idx
            )

        # TEST:$flare_plan_item_is_run++;
        self._eq(got_flare_idx, flare_idx, name + " - matching flare_idx")

        got_iters_count = \
            self.lib.fc_solve_user_INTERNAL_get_flares_plan_item_iters_count(
                self.user,
                item_idx
            )

        # TEST:$flare_plan_item_is_run++;
        self._eq(got_iters_count, iters_count,
                 name + " - matching iters_count")

    # TEST:$flare_plan_item_is_checkpoint=0;
    def flare_plan_item_is_checkpoint(self, name, item_idx):
        # TEST:$flare_plan_item_is_checkpoint++;
        self._eq(self._get_plan_type(item_idx), b"CP", name + " - right type")

    def num_by_depth_tests_order_is(self, name, want_num):
        got_num = self.lib.fc_solve_user_INTERNAL_get_num_by_depth_tests_order(
                self.user
        )

        self._eq(want_num, got_num, name + " - by_depth_tests_order.")

    def by_depth_max_depth_of_depth_idx_is(self, name, depth_idx, want_num):
        got_num = self.lib.fc_solve_user_INTERNAL_get_by_depth_tests_max_depth(
            self.user,
            depth_idx
        )

        self._eq(want_num, got_num, "%s - max_depth_of_depth_idx_is for %d." %
                 (name, depth_idx))

    # TEST:$set_befs=0;
    def _set_befs_weights(self, name, weights_s):
        # TEST:$set_befs=$set_befs+$input_cmd_line;
        self.input_cmd_line__test(name, ["-asw", weights_s])

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

            have = self.lib.fc_solve_user_INTERNAL_get_befs_weight(
                    self.user, idx)
            # TEST:$test_befs=$test_befs+$num_befs_weights;
            self.unittest.assertTrue(
                (bottom <= have) and (have <= top),
                name + " - Testing Weight No. " + str(idx) +
                "Should be: [" + str(bottom) + "," + str(top) + "] ; " +
                "Is: " + str(have))
