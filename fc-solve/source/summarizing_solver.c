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

struct fc_solve_display_information_context_struct
{
    fcs_bool_t debug_iter_state_output;
    fcs_bool_t parseable_output;
    fcs_bool_t canonized_order_output;
    fcs_bool_t display_10_as_t;
    fcs_bool_t display_parent_iter_num;
    fcs_bool_t debug_iter_output_on;
    fcs_bool_t display_moves;
    fcs_bool_t display_states;
};

typedef struct fc_solve_display_information_context_struct fc_solve_display_information_context_t;

static void my_iter_handler(
    void * user_instance,
    fcs_int_limit_t iter_num,
    int depth,
    void * ptr_state,
    fcs_int_limit_t parent_iter_num,
    void * lp_context
    )
{
    fc_solve_display_information_context_t * context;
    context = (fc_solve_display_information_context_t*)lp_context;

    fprintf(stdout, "Iteration: %li\n", (long)iter_num);
    fprintf(stdout, "Depth: %i\n", depth);
    if (context->display_parent_iter_num)
    {
        fprintf(stdout, "Parent Iteration: %li\n", (long)parent_iter_num);
    }
    fprintf(stdout, "\n");


    if (context->debug_iter_state_output)
    {
        char * state_string =
            freecell_solver_user_iter_state_as_string(
                user_instance,
                ptr_state,
                context->parseable_output,
                context->canonized_order_output,
                context->display_10_as_t
                );
        printf("%s\n---------------\n\n\n", state_string);
        free((void*)state_string);
    }
}

struct pack_item_struct
{
    fc_solve_display_information_context_t display_context;
    void * instance;
};

typedef struct pack_item_struct pack_item_t;


static int cmd_line_callback(
    void * instance,
    int argc GCC_UNUSED,
    freecell_solver_str_t argv[],
    int arg,
    int * num_to_skip,
    int * ret GCC_UNUSED,
    void * context
    )
{
    pack_item_t * item;
    fc_solve_display_information_context_t * dc;
    item = (pack_item_t * )context;
    dc = &(item->display_context);

    *num_to_skip = 0;

    if ((!strcmp(argv[arg], "-i")) || (!strcmp(argv[arg], "--iter-output")))
    {
        freecell_solver_user_set_iter_handler_long(
            instance,
            my_iter_handler,
            dc
            );
        dc->debug_iter_output_on = TRUE;
    }
    else if ((!strcmp(argv[arg], "-s")) || (!strcmp(argv[arg], "--state-output")))
    {
        dc->debug_iter_state_output = TRUE;
    }
    else if ((!strcmp(argv[arg], "-p")) || (!strcmp(argv[arg], "--parseable-output")))
    {
        dc->parseable_output = TRUE;
    }
    else if ((!strcmp(argv[arg], "-c")) || (!strcmp(argv[arg], "--canonized-order-output")))
    {
        dc->canonized_order_output = TRUE;
    }
    else if ((!strcmp(argv[arg], "-t")) || (!strcmp(argv[arg], "--display-10-as-t")))
    {
        dc->display_10_as_t = TRUE;
    }
    else if ((!strcmp(argv[arg], "-m")) || (!strcmp(argv[arg], "--display-moves")))
    {
        dc->display_moves = TRUE;
        dc->display_states = FALSE;
    }
    else if ((!strcmp(argv[arg], "-sam")) || (!strcmp(argv[arg], "--display-states-and-moves")))
    {
        dc->display_moves = TRUE;
        dc->display_states = TRUE;
    }
    else if ((!strcmp(argv[arg], "-pi")) || (!strcmp(argv[arg], "--display-parent-iter")))
    {
        dc->display_parent_iter_num = TRUE;
    }
    else
    {
        fprintf(stderr, "Unknown option %s!\n", argv[arg]);
        exit(-1);
        return 0;
    }
    *num_to_skip = 1;
    return FCS_CMD_LINE_SKIP;
}

static const char * known_parameters[] = {
    "-i", "--iter-output",
    "-s", "--state-output",
    "-p", "--parseable-output",
    "-t", "--display-10-as-t",
    "-pi", "--display-parent-iter",
    NULL
    };

#define BINARY_OUTPUT_NUM_INTS 16

typedef struct
{
    FILE * file;
    char * buffer;
    char * buffer_end;
    char * ptr;
} binary_output_t;

static void print_int(binary_output_t * bin, int val)
{
    unsigned char * buffer = (unsigned char *)bin->ptr;
    int p;

    for(p=0;p<4;p++)
    {
        buffer[p] = (unsigned char)(val & 0xFF);
        val >>= 8;
    }
    bin->ptr += 4;
    if (bin->ptr == bin->buffer_end)
    {
        fwrite(bin->buffer, 1, bin->ptr - bin->buffer, bin->file);
        fflush(bin->file);
        /* Reset ptr to the beginning */
        bin->ptr = bin->buffer;
    }
}

#define print_int_wrapper(i) { if (binary_output.file) { print_int(&binary_output, (i));  } }

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

static int read_int(FILE * f, int * dest)
{
    unsigned char buffer[4];
    int num_read;

    num_read = fread(buffer, 1, 4, f);
    if (num_read != 4)
    {
        return 1;
    }
    *dest = (buffer[0]+((buffer[1]+((buffer[2]+((buffer[3])<<8))<<8))<<8));

    return 0;
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
    char * buffer;

    fcs_int64_t total_num_iters = 0;

    char * error_string;
    int parser_ret;
    fcs_bool_t variant_is_freecell;


    char * binary_output_filename = NULL;

    binary_output_t binary_output;

    int arg = 1, start_from_arg;

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

    start_from_arg = arg;

    user.instance = freecell_solver_user_alloc();

    arg = start_from_arg;

    parser_ret =
        freecell_solver_user_cmd_line_parse_args(
            user.instance,
            argc,
            (freecell_solver_str_t *)(void *)argv,
            arg,
            known_parameters,
            cmd_line_callback,
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

    for (int deal_idx=0;deal_idx < num_deals; deal_idx++)
    {
        board_num = deals[deal_idx];
#define BUF_SIZE 2000
        if (variant_is_freecell)
        {
            buffer = calloc(BUF_SIZE, sizeof(buffer[0]));
            get_board_l(board_num, buffer);
        }
        else
        {
            char command[1000];
            buffer = calloc(BUF_SIZE, sizeof(buffer[0]));

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

        ret =
            freecell_solver_user_solve_board(
                user.instance,
                buffer
                );

        free(buffer);

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
