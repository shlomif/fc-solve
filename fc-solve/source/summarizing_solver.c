/*
 * This file is part of Freecell Solver. It is subject to the license terms in
 * the COPYING.txt file found in the top-level directory of this distribution
 * and at http://fc-solve.shlomifish.org/docs/distro/COPYING.html . No part of
 * Freecell Solver, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the COPYING file.
 *
 * Copyright (c) 2000 Shlomi Fish
 */
/*
 * summarizing_solver.c - solves several indices of deals and prints a summary
 * of the solutions of each one.
 * */
#include "fcs_cl.h"
#include "range_solvers_gen_ms_boards.h"
#include "handle_parsing.h"

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

static long deals[32000];
static size_t num_deals = 0;

static inline void append(long idx)
{
    if (num_deals == COUNT(deals))
    {
        fc_solve_err("Number of deals exceeded %ld!\n", (long)(COUNT(deals)));
    }
    deals[num_deals++] = idx;
}

int main(int argc, char *argv[])
{
    const char *variant = "freecell";
    int arg = 1;

    while (arg < argc && (strcmp(argv[arg], "--")))
    {
        if (!strcmp(argv[arg], "seq"))
        {
            arg++;
            if (arg < argc)
            {
                const long start = atol(argv[arg++]);
                if (arg < argc)
                {
                    const long end = atol(argv[arg++]);
                    for (long deal = start; deal <= end; deal++)
                    {
                        append(deal);
                    }
                }
                else
                {
                    fc_solve_err("seq without args!\n");
                }
            }
            else
            {
                fc_solve_err("seq without args!\n");
            }
        }
        else
        {
            append(atol(argv[arg++]));
        }
    }

    if (arg == argc)
    {
        fc_solve_err("No double dash (\"--\") after deals indexes!\n");
    }

    arg++;

    for (; arg < argc; arg++)
    {
        if (!strcmp(argv[arg], "--variant"))
        {
            arg++;
            if (arg == argc)
            {
                fprintf(stderr, "--variant came without an argument!\n");
                print_help();
            }
            variant = argv[arg];

            if (strlen(variant) > 50)
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

    void *const instance = simple_alloc_and_parse(argc, argv, &arg);

    const fcs_bool_t variant_is_freecell = (!strcmp(variant, "freecell"));
#ifndef FCS_FREECELL_ONLY
    freecell_solver_user_apply_preset(instance, variant);
#endif
    char buffer[2000];

    for (size_t deal_idx = 0; deal_idx < num_deals; deal_idx++)
    {
        const_AUTO(board_num, deals[deal_idx]);
        if (variant_is_freecell)
        {
            get_board_l(board_num, buffer);
        }
        else
        {
            char command[1000];
            sprintf(command, "make_pysol_freecell_board.py -F -t %ld %s",
                board_num, variant);

            FILE *const from_make_pysol = popen(command, "r");
            fread(
                buffer, sizeof(buffer[0]), COUNT(buffer) - 1, from_make_pysol);
            pclose(from_make_pysol);
        }
        buffer[COUNT(buffer) - 1] = '\0';

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
        printf("%ld = Verdict: %s ; Iters: %ld ; Length: %ld\n", board_num,
            verdict, (long)freecell_solver_user_get_num_times_long(instance),
            num_moves);
        fflush(stdout);

        freecell_solver_user_recycle(instance);
    }

    freecell_solver_user_free(instance);

    return 0;
}
