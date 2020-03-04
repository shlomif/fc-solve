// This file is part of Freecell Solver. It is subject to the license terms in
// the COPYING.txt file found in the top-level directory of this distribution
// and at http://fc-solve.shlomifish.org/docs/distro/COPYING.html . No part of
// Freecell Solver, including this file, may be copied, modified, propagated,
// or distributed except according to the terms contained in the COPYING file.
//
// Copyright (c) 2000 Shlomi Fish
// summarizing_solver.c - solves several indices of deals and prints a summary
// of the solutions of each one.
#include "freecell-solver/fcs_conf.h"
#include "freecell-solver/fcs_cl.h"
#include "range_solvers_gen_ms_boards.h"
#include "handle_parsing.h"
#include "try_param.h"

static void __attribute__((noreturn)) print_help(void)
{
    printf("\n%s", "summary-fc-solve [deal1_idx] [deal2_idx] .. -- \n"
                   "   [--variant variant_str] [fc-solve theme args]\n"
                   "\n"
                   "Attempts to solve several arbitrary deal indexes from the\n"
                   "Microsoft/Freecell Pro deals using the fc-solve's theme "
                   "and reports a\n"
                   "summary of their results to STDOUT\n");
    exit(-1);
}

#include "deals_populator.h"

int main(int argc, char *argv[])
{
    const char *variant = "freecell";

    int arg = populate_deals_from_argv(argc, argv, 1);
    if (arg == argc)
    {
        exit_error("No double dash (\"--\") after deals indexes!\n");
    }

    for (++arg; arg < argc; ++arg)
    {
        const char *param;
        if ((param = TRY_P("--variant")))
        {
            if (strlen(variant = param) > 50)
            {
                fprintf(stderr, "--variant's argument is too long!\n");
                print_help();
            }
        }
        else
        {
            break;
        }
    }

    void *const instance = simple_alloc_and_parse(argc, argv, arg);

    const bool variant_is_freecell = (!strcmp(variant, "freecell"));
    char buffer[2000];
    if (variant_is_freecell)
    {
        get_board__setup_string(buffer);
    }

    DEALS_ITERATE__START(board_num)
    if (variant_is_freecell)
    {
        get_board_l__without_setup(board_num, buffer);
    }
    else
    {
        char command[1000];
        sprintf(command, "make_pysol_freecell_board.py -F -t %lu %s", board_num,
            variant);

        FILE *const from_make_pysol = popen(command, "r");
        if (fread(buffer, sizeof(buffer[0]), COUNT(buffer) - 1,
                from_make_pysol) <= 0)
        {
            abort();
        }
        pclose(from_make_pysol);
    }
    LAST(buffer) = '\0';

    long num_moves;
    const char *verdict;
    switch (freecell_solver_user_solve_board(instance, buffer))
    {
    case FCS_STATE_SUSPEND_PROCESS:
        num_moves = -1;
        verdict = "Intractable";
        break;

    case FCS_STATE_IS_NOT_SOLVEABLE:
        num_moves = -1;
        verdict = "Unsolved";
        break;

    default:
#ifdef FCS_WITH_MOVES
        num_moves = freecell_solver_user_get_moves_left(instance);
#else
        num_moves = -1;
#endif
        verdict = "Solved";
        break;
    }
    printf("%lu = Verdict: %s ; Iters: %ld ; Length: %ld\n", board_num, verdict,
        (long)freecell_solver_user_get_num_times_long(instance), num_moves);
    fflush(stdout);

    freecell_solver_user_recycle(instance);
    DEALS_ITERATE__END()

    freecell_solver_user_free(instance);
    deals_ranges__free();
    return 0;
}
