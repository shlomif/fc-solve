/* Copyright (c) 2000 Shlomi Fish
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use,
 * copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following
 * conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 */
/*
 * fc_pro_range_solver.c - the FC-Pro range solver. Solves a range of
 * boards and displays the moves counts and the number of moves in their
 * solution.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "portable_int64.h"

#include "fcs_user.h"
#include "fcs_cl.h"

#include "range_solvers_gen_ms_boards.h"
#include "unused.h"
#include "bool.h"
#include "count.h"
#include "indirect_buffer.h"

struct pack_item_struct
{
    void * instance;
};

typedef struct pack_item_struct pack_item_t;

static void print_help(void)
{
    printf("\n%s",
"freecell-solver-range-parallel-solve start end print_step\n"
"   [--binary-output-to filename] [--total-iterations-limit limit]\n"
"   [fc-solve Arguments...]\n"
"\n"
"Solves a sequence of boards from the Microsoft/Freecell Pro Deals\n"
"\n"
"start - the first board in the sequence\n"
"end - the last board in the sequence (inclusive)\n"
"print_step - at which division to print a status line\n"
"\n"
"--binary-output-to   filename\n"
"     Outputs statistics to binary file 'filename'\n"
"--total-iterations-limit  limit\n"
"     Limits each board for up to 'limit' iterations.\n"
          );
}

static long deals[32000];
static int num_deals = 0;

int main(int argc, char * argv[])
{
    pack_item_t user;
    int ret;
    int len;
    int board_num;
    int start_board, end_board, stop_at;
    const char * variant = "freecell";

    fcs_int64_t total_num_iters = 0;

    char * error_string;
    int parser_ret;
    fcs_bool_t variant_is_freecell;

    int arg = 1;

    DECLARE_IND_BUF_T(indirect_stacks_buffer)

    while (arg < argc && (strcmp(argv[arg], "--")))
    {
        if (num_deals == COUNT(deals))
        {
            fprintf(stderr, "Number of deals exceeded %d!\n", COUNT(deals));
            exit(-1);
        }
        deals[num_deals++] = atol(argv[arg++]);
    }

    if (arg == argc)
    {
        fprintf(stderr, "No double dash (\"--\") after deals indexes!");
        exit(-1);
    }

    arg++;

    for (;arg < argc; arg++)
    {
        if (!strcmp(argv[arg], "--variant"))
        {
            arg++;
            if (arg == argc)
            {
                fprintf(stderr, "--variant came without an argument!\n");
                print_help();
                exit(-1);
            }
            variant = argv[arg];

            if (strlen(variant) > 50)
            {
                fprintf(stderr, "--variant's argument is too long!\n");
                print_help();
                exit(-1);
            }
        }
        else
        {
            break;
        }
    }

    user.instance = freecell_solver_user_alloc();

    parser_ret =
        freecell_solver_user_cmd_line_parse_args(
            user.instance,
            argc,
            (freecell_solver_str_t *)(void *)argv,
            arg,
            NULL,
            NULL,
            &user,
            &error_string,
            &arg
            );

    if (parser_ret == FCS_CMD_LINE_UNRECOGNIZED_OPTION)
    {
        fprintf(stderr, "Unknown option: %s", argv[arg]);
        return (-1);
    }
    else if (parser_ret == FCS_CMD_LINE_PARAM_WITH_NO_ARG)
    {
        fprintf(stderr, "The command line parameter \"%s\" requires an argument"
                " and was not supplied with one.\n", argv[arg]);
        return (-1);
    }
    else if (parser_ret == FCS_CMD_LINE_ERROR_IN_ARG)
    {
        if (error_string != NULL)
        {
            fprintf(stderr, "%s", error_string);
            free(error_string);
        }
        return (-1);
    }

    variant_is_freecell = (!strcmp(variant, "freecell"));
    freecell_solver_user_apply_preset(user.instance, variant);

    ret = 0;

#define BUF_SIZE 2000
    char buffer[BUF_SIZE];

    for (int deal_idx=0;deal_idx < num_deals; deal_idx++)
    {
        board_num = deals[deal_idx];
        if (variant_is_freecell)
        {
            get_board_l(board_num, buffer);
        }
        else
        {
            char command[1000];
            sprintf(command, "make_pysol_freecell_board.py -F -t %d %s",
                    board_num,
                    variant
                   );

            FILE * const from_make_pysol = popen(command, "r");
            fread(buffer, sizeof(buffer[0]), BUF_SIZE-1, from_make_pysol);
            pclose(from_make_pysol);
#undef BUF_SIZE
        }
#if 0
        printf("%s\n", buffer);
#endif

        buffer[COUNT(buffer)-1] = '\0';

        ret =
            freecell_solver_user_solve_board(
                user.instance,
                buffer
                );

        long num_moves;
        const char * verdict;

        if (ret == FCS_STATE_SUSPEND_PROCESS)
        {
            num_moves  = -1;
            verdict = "Intractable";
        }
        else if (ret == FCS_STATE_IS_NOT_SOLVEABLE)
        {
            num_moves = -1;
            verdict = "Unsolved";
        }
        else
        {
            num_moves = freecell_solver_user_get_moves_left(user.instance);

            verdict = "Solved";
        }
        printf("%ld = Verdict: %s ; Iters: %ld ; Length: %ld\n",
            board_num, verdict, freecell_solver_user_get_num_times_long(user.instance), num_moves
        );
        fflush(stdout);

        freecell_solver_user_recycle(user.instance);
    }

    freecell_solver_user_free(user.instance);

    return 0;
}
