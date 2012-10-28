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
 * measure_depth_dep_tests_order_performance.c - measure the relative depth-dependent
 * performance of two scans.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "portable_time.h"
#include "portable_int64.h"
#include "alloc_wrap.h"

#include "fcs_user.h"
#include "fcs_cl.h"
#include "unused.h"
#include "inline.h"
#include "range_solvers_gen_ms_boards.h"

struct pack_item_struct
{
    void * instance;
};

typedef struct pack_item_struct pack_item_t;

#define BINARY_OUTPUT_NUM_INTS 16

typedef struct
{
    FILE * file;
    char * buffer;
    char * buffer_end;
    char * ptr;
} binary_output_t;

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

typedef struct
{
    fcs_portable_time_t start_time, end_time;
    fcs_int_limit_t num_iters;
    int verdict;
} result_t;

static const char * known_parameters[] = {
    NULL
    };

int main(int argc, char * argv[])
{
    pack_item_t user;
    /* char buffer[2048]; */
    int board_num;
    int start_board, end_board;
    result_t * results, * curr_result;
    FILE * output_fh;
    int min_depth_for_scan2;
    fcs_int_limit_t iters_limit = 100000;
    int max_var_depth_to_check = 100;

    fcs_portable_time_t mytime;

    char * error_string;
    char * scan1_to = NULL, * scan2_to = NULL;

    char * output_filename = NULL;
    fcs_state_string_t state_string;

    int arg = 1, start_from_arg = -1, end_args = -1;

    if (argc < 3)
    {
        fprintf(stderr, "Not Enough Arguments!\n");
        print_help();
        exit(-1);
    }
    start_board = atoi(argv[arg++]);
    end_board = atoi(argv[arg++]);

    for (;arg < argc; arg++)
    {
        if (!strcmp(argv[arg], "--args-start"))
        {
            arg++;

            start_from_arg = arg;
            while (arg < argc)
            {
                if (!strcmp(argv[arg], "--args-end"))
                {
                    break;
                }
                arg++;
            }
            end_args = arg;
        }
        else if (!strcmp(argv[arg], "--output-to"))
        {
            arg++;
            if (arg == argc)
            {
                fprintf(stderr, "--output-to came without an argument!\n");
                print_help();
                exit(-1);
            }
            output_filename = argv[arg];
        }
        else if (!strcmp(argv[arg], "--scan1-to"))
        {
            arg++;
            if (arg == argc)
            {
                fprintf(stderr, "--scan1-to came without an argument!\n");
                print_help();
                exit(-1);
            }
            scan1_to = argv[arg];
        }
        else if (!strcmp(argv[arg], "--scan2-to"))
        {
            arg++;
            if (arg == argc)
            {
                fprintf(stderr, "--scan1-to came without an argument!\n");
                print_help();
                exit(-1);
            }
            scan2_to = argv[arg];
        }
        else if (!strcmp(argv[arg], "--iters-limit"))
        {
            arg++;
            if (arg == argc)
            {
                fprintf(stderr, "--iters-limit came without an argument!\n");
                print_help();
                exit(-1);
            }
            iters_limit = (fcs_int_limit_t)atol(argv[arg]);
        }
        else if (!strcmp(argv[arg], "--max-var-depth"))
        {
            arg++;
            if (arg == argc)
            {
                fprintf(stderr, "--max-var-depth came without an argument!\n");
                print_help();
                exit(-1);
            }
            max_var_depth_to_check = atoi(argv[arg]);
        }
        else
        {
            fprintf(stderr, "Unknown argument - '%s'!\n", argv[arg]);
            exit(-1);
        }
    }

    if (!scan1_to)
    {
        fprintf(stderr, "--scan1-to not specified!\n");
        exit(-1);
    }

    if (!scan2_to)
    {
        fprintf(stderr, "--scan2-to not specified!\n");
        exit(-1);
    }

    if (!output_filename)
    {
        fprintf(stderr, "output filename not specified");
        exit(-1);
    }

    FCS_PRINT_STARTED_AT(mytime);
    fflush(stdout);

    results = SMALLOC(results, end_board - start_board + 1);

    output_fh = fopen(output_filename, "wt");

    for (min_depth_for_scan2 = 0; min_depth_for_scan2 < max_var_depth_to_check ;
            min_depth_for_scan2++)
    {
        user.instance = freecell_solver_user_alloc();

        if (start_from_arg >= 0)
        {
            freecell_solver_user_cmd_line_parse_args(
                user.instance,
                end_args,
                (freecell_solver_str_t *)(void *)argv,
                start_from_arg,
                known_parameters,
                NULL,
                &user,
                &error_string,
                &arg
            );
        }

        if (freecell_solver_user_set_depth_tests_order(
            user.instance,
            0,
            scan1_to,
            &error_string
            ))
        {
            fprintf(stderr, "Error! Got '%s'!\n", error_string);
            free(error_string);
            exit(-1);
        }
        if (freecell_solver_user_set_depth_tests_order(
            user.instance,
            min_depth_for_scan2,
            scan2_to,
            &error_string
            ))
        {
            fprintf(stderr, "Error! Got '%s'!\n", error_string);
            free(error_string);
            exit(-1);
        }


        for (board_num=start_board, curr_result = results
                ;
             board_num<=end_board
                ;
             board_num++, curr_result++
        )
        {
            get_board(board_num, state_string);

            freecell_solver_user_limit_iterations_long(
                user.instance,
                iters_limit
            );

            FCS_GET_TIME(curr_result->start_time);

            curr_result->verdict =
                freecell_solver_user_solve_board(
                    user.instance,
                    state_string
                    );

            FCS_GET_TIME(curr_result->end_time);

            curr_result->num_iters
                = freecell_solver_user_get_num_times_long(user.instance);

            freecell_solver_user_recycle(user.instance);
        }

        freecell_solver_user_free(user.instance);

        printf("Reached depth %d\n", min_depth_for_scan2);

        fprintf(output_fh, "Depth == %d\n\n", min_depth_for_scan2);
        for (board_num=start_board, curr_result = results
                ;
             board_num<=end_board
                ;
             board_num++, curr_result++
        )
        {
            fprintf(output_fh, "board[%d].ret == %d\n", board_num, curr_result->verdict);
            fprintf(output_fh, "board[%d].iters == %ld\n", board_num, (long)curr_result->num_iters);

#define FPRINTF_TIME(label, field) \
            fprintf(output_fh, "board[%d].%s = %li.%.6li\n", board_num, label, FCS_TIME_GET_SEC(curr_result->field),FCS_TIME_GET_USEC(curr_result->field));

            FPRINTF_TIME("start", start_time);
            FPRINTF_TIME("end", end_time);

#undef FPRINTF_TIME
        }
        fflush(output_fh);
    }

    fclose(output_fh);

    return 0;
}
