from cffi import FFI
import platform


class FC_Solve:
    # TEST:$num_befs_weights=5;
    NUM_BEFS_WEIGHTS = 5

    def __init__(self):
        self.ffi = FFI()
        self.lib = self.ffi.dlopen(
            "../libfreecell-solver." +
            ("dll" if (platform.system() == 'Windows') else "so"))
        self.ffi.cdef('''
void * freecell_solver_user_alloc();
void freecell_solver_user_free(void * instance);
double fc_solve_user_INTERNAL_get_befs_weight(void * user, int idx);
typedef char * freecell_solver_str_t;
typedef int (*freecell_solver_user_cmd_line_known_commands_callback_t)(
    void *instance, int argc, freecell_solver_str_t argv[], int arg_index,
    int *num_to_skip, int *ret, void *context);
int freecell_solver_user_cmd_line_parse_args_with_file_nesting_count(
    void *instance,
    int argc, freecell_solver_str_t argv[], int start_arg,
    freecell_solver_str_t *known_parameters,
    freecell_solver_user_cmd_line_known_commands_callback_t callback,
    void *callback_context, char **error_string,
    int *last_arg, int file_nesting_count,
    freecell_solver_str_t opened_files_dir);
int freecell_solver_user_set_flares_plan(void * instance, char * s);
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
long freecell_solver_user_get_num_times_long(void * user);
long freecell_solver_user_get_num_states_in_collection_long(void * user);
void freecell_solver_user_limit_iterations_long(
    void * api_instance, const long max_iters);
int freecell_solver_user_solve_board(void *api_instance,
const char *const state_as_string);
int freecell_solver_user_resume_solution(void * user);
void freecell_solver_user_recycle(void *api_instance);
''')
        self.user = self.lib.freecell_solver_user_alloc()

    def input_cmd_line__generic(self, cmd_line_args):
        last_arg = self.ffi.new('int *')
        error_string = self.ffi.new('char * *')
        known_params = self.ffi.new('char * *')
        opened_files_dir = self.ffi.new('char [32001]')

        prefix = 'freecell_solver_user_cmd_line'
        func = 'parse_args_with_file_nesting_count'

        getattr(self.lib, prefix + '_' + func)(
            self.user,  # instance
            len(cmd_line_args),    # argc
            [self.ffi.new('char[]', bytes(s, 'UTF-8')) \
             for s in cmd_line_args],  # argv
            0,   # start_arg
            known_params,  # known_params
            self.ffi.NULL,   # callback
            self.ffi.NULL,   # callback_context
            error_string,  # error_string
            last_arg,   # last_arg
            -1,  # file_nesting_count
            opened_files_dir
        )

        return (last_arg[0], len(cmd_line_args))

    # TEST:$set_befs=0;
    def _set_befs_weights(self, name, weights_s):
        # TEST:$set_befs=$set_befs+$input_cmd_line;
        self.input_cmd_line(name, ["-asw", weights_s])

    def __destroy__(self):
        self.ffi.freecell_solver_user_free(self.user)

    def _get_plan_type(self, item_idx):
        return self.ffi.string(
            self.lib.fc_solve_user_INTERNAL_get_flares_plan_item_type(
                self.user, item_idx))

    def solve_board(self, board):
        return self.lib.freecell_solver_user_solve_board(
                self.user,
                bytes(board, 'UTF-8')
        )

    def resume_solution(self):
        return self.lib.freecell_solver_user_resume_solution(self.user)

    def limit_iterations(self, max_iters):
        self.lib.freecell_solver_user_limit_iterations_long(
            self.user,
            max_iters
        )

    def get_num_times(self):
        return self.lib.freecell_solver_user_get_num_times_long(
                self.user)

    def get_num_states_in_col(self):
        return self.lib.freecell_solver_user_get_num_states_in_collection_long(
                self.user)

    def recycle(self):
        self.lib.freecell_solver_user_recycle(self.user)


class FC_Solve_Suite(FC_Solve):
    def __init__(self, ut):
        FC_Solve.__init__(self)
        self.unittest = ut

    # TEST:$input_cmd_line=0;
    def input_cmd_line(self, name, cmd_line_args):
        ret = self.input_cmd_line__generic(cmd_line_args)
        # TEST:$input_cmd_line++;
        self._eq(
            ret[0], ret[1],
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
