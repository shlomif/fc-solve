from TAP.Simple import diag, is_ok, ok
from ctypes import byref, c_char_p, c_int, c_long, c_void_p, \
        create_string_buffer, CDLL
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

''')
        self.fcs = CDLL("../libfreecell-solver." +
                        ("dll" if (platform.system() == 'Windows') else "so"))

        self.user_alloc = self.fcs.freecell_solver_user_alloc
        self.user_alloc.restype = c_void_p
        self.u_get_num_times = self.fcs.freecell_solver_user_get_num_times_long
        self.u_get_num_times.restype = c_long
        prefix = 'freecell_solver_user'
        func = 'get_num_states_in_collection_long'
        self.get_num_states = self.fcs[prefix + '_' + func]
        self.get_num_states.restype = c_long
        self.user = c_void_p(self.user_alloc())
        self.lib_user = self.lib.freecell_solver_user_alloc()

    # TEST:$input_cmd_line=0;
    def lib__input_cmd_line(self, name, cmd_line_args):

        last_arg = self.ffi.new('int *')
        error_string = self.ffi.new('char * *')
        known_params = self.ffi.new('char * *')
        opened_files_dir = self.ffi.new('char [32001]')

        prefix = 'freecell_solver_user_cmd_line'
        func = 'parse_args_with_file_nesting_count'

        getattr(self.lib, prefix + '_' + func)(
            self.lib_user,  # instance
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

        # TEST:$input_cmd_line++;
        is_ok(last_arg[0], len(cmd_line_args),
              name + " - assign weights - processed two arguments")

    def input_cmd_line(self, name, cmd_line_args):

        last_arg = c_int()
        error_string = c_char_p()
        known_params = c_char_p(None)
        opened_files_dir = create_string_buffer(32001)
        diag("opened_files_dir = <%s>" % opened_files_dir)

        if False:  # (platform.system() == 'Windows'):
            import sys
            sys.exit(1)

        cmd_line_args_tuple = tuple(cmd_line_args)

        prefix = 'freecell_solver_user_cmd_line'
        func = 'parse_args_with_file_nesting_count'

        self.fcs[prefix + '_' + func](
            self.user,  # instance
            len(cmd_line_args),    # argc
            ((c_char_p * len(cmd_line_args))(
                *tuple(bytes(s, 'UTF-8') for s in cmd_line_args_tuple)
            )),  # argv
            0,   # start_arg
            byref(known_params),  # known_params
            None,   # callback
            None,   # callback_context
            byref(error_string),  # error_string
            byref(last_arg),    # last_arg
            c_int(-1),  # file_nesting_count
            opened_files_dir
        )

        is_ok(last_arg.value, len(cmd_line_args),
              name + " - assign weights - processed two arguments")

    # TEST:$set_befs=0;
    def _set_befs_weights(self, name, weights_s):
        # TEST:$set_befs=$set_befs+$input_cmd_line;
        self.lib__input_cmd_line(name, ["-asw", weights_s])

    def __destroy__(self):
        self.fcs.freecell_solver_user_free(self.user)

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
                    self.lib_user, idx)
            # TEST:$test_befs=$test_befs+$num_befs_weights;
            if (not ok((bottom <= have) and (have <= top),
                       name + " - Testing Weight No. " + str(idx))):
                diag("Should be: [" + str(bottom) + "," + str(top) + "] ; " +
                     "Is: " + str(have))

    # TEST:$compile_flares_plan_ok=0;
    def compile_flares_plan_ok(self, name, flares_plan_string):
        error_string = c_char_p()

        myplan = None
        if flares_plan_string:
            myplan = bytes(flares_plan_string, 'UTF-8')
        ret_code = self.fcs.freecell_solver_user_set_flares_plan(
            self.user,
            c_char_p(myplan)
        )

        # TEST:$compile_flares_plan_ok++;
        ok(ret_code == 0,
           name + " - set_string returned success")
        ret_code = self.fcs.fc_solve_user_INTERNAL_compile_all_flares_plans(
            self.user,
            byref(error_string)
        )

        # TEST:$compile_flares_plan_ok++;
        ok(ret_code == 0,
           name + " - returned success.")

        # TEST:$compile_flares_plan_ok++;
        ok(error_string.value is None,
           name + " - error_string returned NULL.")

    def flare_plan_num_items_is(self, name, want_num_items):
        got_num_items = \
            self.fcs.fc_solve_user_INTERNAL_get_flares_plan_num_items(
                self.user
            )

        ok(want_num_items == got_num_items, name + " - got_num_items.")

    def _get_plan_type(self, item_idx):
        f = self.fcs.fc_solve_user_INTERNAL_get_flares_plan_item_type
        f.restype = c_char_p

        return f(self.user, item_idx)

    # TEST:$flare_plan_item_is_run_indef=0;
    def flare_plan_item_is_run_indef(self, name, item_idx, flare_idx):
        # TEST:$flare_plan_item_is_run_indef++;
        ok(self._get_plan_type(item_idx) == b"RunIndef",
           name + " - right type")

        got_flare_idx = \
            self.fcs.fc_solve_user_INTERNAL_get_flares_plan_item_flare_idx(
                self.user,
                item_idx
            )

        # TEST:$flare_plan_item_is_run_indef++;
        ok(got_flare_idx == flare_idx,
           name + " - matching flare_idx")

    # TEST:$flare_plan_item_is_run=0;
    def flare_plan_item_is_run(self, name, item_idx, flare_idx, iters_count):
        # TEST:$flare_plan_item_is_run++;
        ok(self._get_plan_type(item_idx) == b"Run",
           name + " - right type")

        got_flare_idx = \
            self.fcs.fc_solve_user_INTERNAL_get_flares_plan_item_flare_idx(
                self.user,
                item_idx
            )

        # TEST:$flare_plan_item_is_run++;
        ok(got_flare_idx == flare_idx, name + " - matching flare_idx")

        got_iters_count = \
            self.fcs.fc_solve_user_INTERNAL_get_flares_plan_item_iters_count(
                self.user,
                item_idx
            )

        # TEST:$flare_plan_item_is_run++;
        ok(got_iters_count == iters_count, name + " - matching iters_count")

    # TEST:$flare_plan_item_is_checkpoint=0;
    def flare_plan_item_is_checkpoint(self, name, item_idx):
        # TEST:$flare_plan_item_is_checkpoint++;
        ok(self._get_plan_type(item_idx) == b"CP",
           name + " - right type")

    def num_by_depth_tests_order_is(self, name, want_num):
        got_num = self.fcs.fc_solve_user_INTERNAL_get_num_by_depth_tests_order(
                self.user
        )

        ok(want_num == got_num, name + " - by_depth_tests_order.")

    def by_depth_max_depth_of_depth_idx_is(self, name, depth_idx, want_num):
        got_num = self.fcs.fc_solve_user_INTERNAL_get_by_depth_tests_max_depth(
            self.user,
            depth_idx
        )

        ok(want_num == got_num,
            name + " - max_depth_of_depth_idx_is for" + str(depth_idx) + ".")

    def solve_board(self, board):
        return self.fcs.freecell_solver_user_solve_board(
                self.user,
                (c_char_p)(bytes(board, 'UTF-8'))
        )

    def resume_solution(self):
        return self.fcs.freecell_solver_user_resume_solution(self.user)

    def limit_iterations(self, max_iters):
        self.fcs.freecell_solver_user_limit_iterations_long(
            self.user,
            (c_long)(max_iters)
        )
        return

    def get_num_times(self):
        return self.u_get_num_times(self.user)

    def get_num_states_in_col(self):
        return self.get_num_states(self.user)

    def recycle(self):
        self.fcs.freecell_solver_user_recycle(self.user)
        return
