// This file is part of Freecell Solver. It is subject to the license terms in
// the COPYING.txt file found in the top-level directory of this distribution
// and at http://fc-solve.shlomifish.org/docs/distro/COPYING.html . No part of
// Freecell Solver, including this file, may be copied, modified, propagated,
// or distributed except according to the terms contained in the COPYING file.
//
// Copyright (c) 2000 Shlomi Fish
// range_solvers.h - common routines for the range solvers.
#pragma once
#include "rinutils/rinutils.h"
#include "print_time.h"
#include "freecell-solver/fcs_conf.h"
#include "freecell-solver/fcs_cl.h"
#include "range_solvers_gen_ms_boards.h"
#include "handle_parsing.h"

#ifndef FCS_WITHOUT_CMD_LINE_HELP
static void print_help(void);
static __attribute__((noreturn)) __attribute__((format(printf, 1, 2))) void
help_err(const char *const msg, ...)
{
    va_list ap;
    va_start(ap, msg);
    vfprintf(stderr, msg, ap);
    va_end(ap);
    print_help();
    exit(-1);
}
#endif

static inline bool range_solvers__solve(char *const state_string,
    void *const instance, const fc_solve_ms_deal_idx_type board_num,
    fcs_iters_int *const total_num_iters_temp)
{
    get_board_l__without_setup(board_num, state_string);

    switch (freecell_solver_user_solve_board(instance, state_string))
    {
    case FCS_STATE_SUSPEND_PROCESS:
        fc_solve_print_intractable(board_num);
        break;

    case FCS_STATE_FLARES_PLAN_ERROR:
        print_flares_plan_error(instance);
        return true;

    case FCS_STATE_IS_NOT_SOLVEABLE:
        fc_solve_print_unsolved(board_num);
        break;
#ifdef FCS_RANGE_SOLVERS_PRINT_SOLVED
    case FCS_STATE_WAS_SOLVED:
        printf("Solved Board No. " RIN_ULL_FMT "\n", board_num);
        break;
#endif
    }

    *total_num_iters_temp += freecell_solver_user_get_num_times_long(instance);
    return false;
}

static inline int range_solvers_main(int argc, char *argv[], int arg,
    fc_solve_ms_deal_idx_type, fc_solve_ms_deal_idx_type,
    fc_solve_ms_deal_idx_type);
int main(int argc, char *argv[])
{
    if (argc < 4)
    {
#ifndef FCS_WITHOUT_CMD_LINE_HELP
        help_err("Not Enough Arguments!\n");
#else
        return -1;
#endif
    }
    int arg = 1;
    const fc_solve_ms_deal_idx_type next_board_num =
        fcs_str2msdeal(argv[arg++]);
    const fc_solve_ms_deal_idx_type end_board = fcs_str2msdeal(argv[arg++]);
    const fc_solve_ms_deal_idx_type stop_at = fcs_str2msdeal(argv[arg++]);

    if (stop_at <= 0)
    {
#ifndef FCS_WITHOUT_CMD_LINE_HELP
        help_err("print_step (the third argument) must be greater than 0.\n");
#else
        return -1;
#endif
    }
    return range_solvers_main(
        argc, argv, arg, next_board_num, end_board, stop_at);
}
