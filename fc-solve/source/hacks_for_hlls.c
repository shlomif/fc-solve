// This file is part of Freecell Solver. It is subject to the license terms in
// the COPYING.txt file found in the top-level directory of this distribution
// and at http://fc-solve.shlomifish.org/docs/distro/COPYING.html . No part of
// Freecell Solver, including this file, may be copied, modified, propagated,
// or distributed except according to the terms contained in the COPYING file.
//
// Copyright (c) 2013 Shlomi Fish
// hacks_for_hlls.c - the Freecell Solver utility methods for high-level
// languages.

#include "freecell-solver/fcs_cl.h"
#include "split_cmd_line.h"
#include "rinutils/alloc_wrap.h"

DLLEXPORT void freecell_solver_user_stringify_move_ptr(
    void *const user_instance, char *const output_string,
    fcs_move_t *const move_ptr, const int standard_notation)
{
    freecell_solver_user_stringify_move_w_state(
        user_instance, output_string, *move_ptr, standard_notation);
}

DLLEXPORT void *freecell_solver_user_args_man_chop(const char *buffer)
{
    fcs_args_man *ret = SMALLOC1(ret);
    *ret = fc_solve_args_man_chop(buffer);
    return ret;
}

DLLEXPORT int freecell_solver_user_args_man_argc(void *a)
{
    return (((fcs_args_man *)a)->argc);
}

DLLEXPORT char **freecell_solver_user_args_man_argv(void *a)
{
    return (((fcs_args_man *)a)->argv);
}

DLLEXPORT void freecell_solver_user_args_man_free(void *v)
{
    fcs_args_man *a = v;
    fc_solve_args_man_free(a);
    free(a);
}
