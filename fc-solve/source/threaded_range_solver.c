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
 *  threaded_range_solver.c - a range solver that solves different boards in 
 *  several POSIX threads.
 *
 *  See also:
 *      - fc_pro_range_solver.c
 *      - test_multi_parallel.c
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifndef WIN32
#include <sys/time.h>
#else
#include <sys/types.h>
#include <sys/timeb.h>
#endif
#include <pthread.h>
#ifndef WIN32
#include <unistd.h>
#endif

#include "fcs_user.h"
#include "fcs_cl.h"
#include "unused.h"
#include "inline.h"
#include "range_solvers_gen_ms_boards.h"

struct fc_solve_display_information_context_struct
{
    int debug_iter_state_output;
    int freecells_num;
    int stacks_num;
    int decks_num;
    int parseable_output;
    int canonized_order_output;
    int display_10_as_t;
    int display_parent_iter_num;
    int debug_iter_output_on;
    int display_moves;
    int display_states;
    int standard_notation;
};

typedef struct fc_solve_display_information_context_struct fc_solve_display_information_context_t;

static void my_iter_handler(
    void * user_instance,
    int iter_num,
    int depth,
    void * ptr_state,
    int parent_iter_num,
    void * lp_context
    )
{
    fc_solve_display_information_context_t * context;
    context = (fc_solve_display_information_context_t*)lp_context;

    fprintf(stdout, "Iteration: %i\n", iter_num);
    fprintf(stdout, "Depth: %i\n", depth);
    if (context->display_parent_iter_num)
    {
        fprintf(stdout, "Parent Iteration: %i\n", parent_iter_num);
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

#ifdef MYDEBUG
    {
        fcs_card_t card;
        int ret;
        char card_str[10];

        ret = fcs_check_state_validity(
            ptr_state_with_locations,
            context->freecells_num,
            context->stacks_num,
            context->decks_num,
            &card
            );

        if (ret != 0)
        {

            fcs_card_perl2user(card, card_str, context->display_10_as_t);
            if (ret == 3)
            {
                fprintf(stdout, "%s\n",
                    "There's an empty slot in one of the stacks."
                    );
            }
            else
            {
                fprintf(stdout, "%s%s.\n",
                    ((ret == 2)? "There's an extra card: " : "There's a missing card: "),
                    card_str
                );
            }
            exit(-1);
        }
    }
#endif
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
    char * argv[],
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
        freecell_solver_user_set_iter_handler(
            instance,
            my_iter_handler,
            dc
            );
        dc->debug_iter_output_on = 1;
    }
    else if ((!strcmp(argv[arg], "-s")) || (!strcmp(argv[arg], "--state-output")))
    {
        dc->debug_iter_state_output = 1;
    }
    else if ((!strcmp(argv[arg], "-p")) || (!strcmp(argv[arg], "--parseable-output")))
    {
        dc->parseable_output = 1;
    }
    else if ((!strcmp(argv[arg], "-c")) || (!strcmp(argv[arg], "--canonized-order-output")))
    {
        dc->canonized_order_output = 1;
    }
    else if ((!strcmp(argv[arg], "-t")) || (!strcmp(argv[arg], "--display-10-as-t")))
    {
        dc->display_10_as_t = 1;
    }
    else if ((!strcmp(argv[arg], "-m")) || (!strcmp(argv[arg], "--display-moves")))
    {
        dc->display_moves = 1;
        dc->display_states = 0;
    }
    else if ((!strcmp(argv[arg], "-sn")) || (!strcmp(argv[arg], "--standard-notation")))
    {
        dc->standard_notation = 1;
    }
    else if ((!strcmp(argv[arg], "-sam")) || (!strcmp(argv[arg], "--display-states-and-moves")))
    {
        dc->display_moves = 1;
        dc->display_states = 1;
    }
    else if ((!strcmp(argv[arg], "-pi")) || (!strcmp(argv[arg], "--display-parent-iter")))
    {
        dc->display_parent_iter_num = 1;
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

#define LIMIT_STEP 500

static char * known_parameters[] = {
    "-i", "--iter-output",
    "-s", "--state-output",
    "-p", "--parseable-output",
    "-t", "--display-10-as-t",
    "-pi", "--display-parent-iter",
    NULL
    };

#define BINARY_OUTPUT_NUM_INTS 16

#define print_int_wrapper(i) { }

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
"--total-iterations-limit  limit\n"
"     Limits each board for up to 'limit' iterations.\n"
          );
}

static const pthread_mutex_t initial_mutex_constant =
    PTHREAD_MUTEX_INITIALIZER
    ;

static int next_board_num;
static int end_board;
static pthread_mutex_t next_board_num_lock;
static int board_num_step = 1;
static int update_total_num_iters_threshold = 1000000;

typedef struct {
    int argc;
    char * * argv;
    int arg;
    int stop_at;
} context_t;

int total_iterations_limit_per_board = -1;

#ifndef WIN32
typedef long long very_long_int_t;
#else
typedef __int64 very_long_int_t;
#endif

very_long_int_t total_num_iters = 0;
static pthread_mutex_t total_num_iters_lock;

static void * worker_thread(void * void_context)
{
    context_t * context;
    pack_item_t user;
    char * error_string;
    int arg;
    int parser_ret;
    int ret;
    char * * argv;
    int board_num;
    int quota_end;
    int stop_at;
#ifndef WIN32
    struct timeval tv;
    struct timezone tz;
#else
    struct _timeb tb;
#endif    
    long total_num_iters_temp = 0;
    /* 52 cards of 3 chars (suit+rank+whitespace) each, 
     * plus 8 newlines, plus one '\0' terminator*/
    fcs_state_string_t state_string;

    context = (context_t *)void_context;
    arg = context->arg;
    argv = context->argv;
    stop_at = context->stop_at;

    user.instance = freecell_solver_user_alloc();

    parser_ret =
        freecell_solver_user_cmd_line_parse_args(
            user.instance,
            context->argc,
            argv,
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
        goto ret_label;
    }
    else if (
        (parser_ret == FCS_CMD_LINE_PARAM_WITH_NO_ARG)
            )
    {
        fprintf(stderr, "The command line parameter \"%s\" requires an argument"
                " and was not supplied with one.\n", argv[arg]);
        goto ret_label;
    }
    else if (
        (parser_ret == FCS_CMD_LINE_ERROR_IN_ARG)
        )
    {
        if (error_string != NULL)
        {
            fprintf(stderr, "%s", error_string);
            free(error_string);
        }
        goto ret_label;
    }

    ret = 0;

    do
    {
        pthread_mutex_lock(&next_board_num_lock);
        board_num = next_board_num;
        quota_end = (next_board_num += board_num_step);
        pthread_mutex_unlock(&next_board_num_lock);

        if (quota_end > end_board+1)
        {
            quota_end = end_board+1;
        }
        for(;board_num<quota_end;board_num++)
        {
            get_board(board_num, state_string);

            freecell_solver_user_limit_iterations(user.instance, total_iterations_limit_per_board);

            ret =
                freecell_solver_user_solve_board(
                    user.instance,
                    state_string
                    );

            if (ret == FCS_STATE_SUSPEND_PROCESS)
            {
#ifndef WIN32
                gettimeofday(&tv,&tz);
                printf("Intractable Board No. %i at %li.%.6li\n",
                    board_num,
                    tv.tv_sec,
                    tv.tv_usec
                    );
#else
                _ftime(&tb);
                printf("Intractable Board No. %i at %li.%.6i\n",
                    board_num,
                    tb.time,
                    tb.millitm*1000
                );
#endif
                fflush(stdout);
                print_int_wrapper(-1);
            }
            else if (ret == FCS_STATE_IS_NOT_SOLVEABLE)
            {
#ifndef WIN32
                gettimeofday(&tv,&tz);
                printf("Unsolved Board No. %i at %li.%.6li\n",
                    board_num,
                    tv.tv_sec,
                    tv.tv_usec
                    );
#else
                _ftime(&tb);
                printf("Unsolved Board No. %i at %li.%.6i\n",
                    board_num,
                    tb.time,
                    tb.millitm*1000
                );
#endif
                print_int_wrapper(-2);
            }
            else
            {
                print_int_wrapper(freecell_solver_user_get_num_times(user.instance));
            }

            total_num_iters_temp += freecell_solver_user_get_num_times(user.instance);
            if (total_num_iters_temp >= update_total_num_iters_threshold)
            {
                pthread_mutex_lock(&total_num_iters_lock);
                total_num_iters += total_num_iters_temp;
                pthread_mutex_unlock(&total_num_iters_lock);
                total_num_iters_temp = 0;
            }

            if (board_num % stop_at == 0)
            {
                very_long_int_t total_num_iters_copy;

                pthread_mutex_lock(&total_num_iters_lock);
                total_num_iters_copy = (total_num_iters += total_num_iters_temp);
                pthread_mutex_unlock(&total_num_iters_lock);
                total_num_iters_temp = 0;

#ifndef WIN32
                gettimeofday(&tv,&tz);
                printf("Reached Board No. %i at %li.%.6li (total_num_iters=%lli)\n",
                    board_num,
                    tv.tv_sec,
                    tv.tv_usec,
                    total_num_iters_copy
                    );
#else
                _ftime(&tb);
                printf(
#ifdef __GNUC__
                        "Reached Board No. %i at %li.%.6i (total_num_iters=%lli)\n",
#else
                        "Reached Board No. %i at %li.%.6i (total_num_iters=%I64i)\n",
#endif
                    board_num,
                    tb.time,
                    tb.millitm*1000,
                    total_num_iters_copy
                );
#endif
                fflush(stdout);
            }

            freecell_solver_user_recycle(user.instance);
        }
    } while (board_num <= end_board);

    pthread_mutex_lock(&total_num_iters_lock);
    total_num_iters += total_num_iters_temp;
    pthread_mutex_unlock(&total_num_iters_lock);

    freecell_solver_user_free(user.instance);

ret_label:
    return NULL;
}


int main(int argc, char * argv[])
{
    int stop_at;
#ifndef WIN32
    struct timeval tv;
    struct timezone tz;
#else
    struct _timeb tb;
#endif


    int num_workers = 3;
    pthread_t * workers;

    int arg = 1, start_from_arg, idx, check;

    context_t context;

    next_board_num_lock = initial_mutex_constant;
    total_num_iters_lock = initial_mutex_constant;
    if (argc < 4)
    {
        fprintf(stderr, "Not Enough Arguments!\n");
        print_help();
        exit(-1);
    }
    next_board_num = atoi(argv[arg++]);
    end_board = atoi(argv[arg++]);
    stop_at = atoi(argv[arg++]);

    for (;arg < argc; arg++)
    {
        if (!strcmp(argv[arg], "--total-iterations-limit"))
        {
            arg++;
            if (arg == argc)
            {
                fprintf(stderr, "--total-iterations-limit came without an argument!\n");
                print_help();
                exit(-1);
            }
            total_iterations_limit_per_board = atoi(argv[arg]);
        }
        else if (!strcmp(argv[arg], "--num-workers"))
        {
            arg++;
            if (arg == argc)
            {
                fprintf(stderr, "--num-workers came without an argument!\n");
                print_help();
                exit(-1);
            }
            num_workers = atoi(argv[arg]);
        }
        else if (!strcmp(argv[arg], "--worker-step"))
        {
            arg++;
            if (arg == argc)
            {
                fprintf(stderr, "--worker-step came without an argument!\n");
                print_help();
                exit(-1);
            }
            board_num_step = atoi(argv[arg]);
        }
        else if (!strcmp(argv[arg], "--iters-update-on"))
        {
            arg++;
            if (arg == argc)
            {
                fprintf(stderr, "--iters-update-on came without an argument!\n");
                print_help();
                exit(-1);
            }
            update_total_num_iters_threshold = atoi(argv[arg]);
        }
        else
        {
            break;
        }
    }

    start_from_arg = arg;

    /* for(board_num=1;board_num<100000;board_num++) */
#ifndef WIN32
    gettimeofday(&tv,&tz);
    printf("Started at %li.%.6li\n",
        tv.tv_sec,
        tv.tv_usec
        );
#else
    _ftime(&tb);
    printf("Started at %li.%.6i\n",
        tb.time,
        tb.millitm*1000
        );
#endif
    fflush(stdout);

    context.argc = argc;
    context.argv = argv;
    context.arg = start_from_arg;
    context.stop_at = stop_at;

    workers = malloc(sizeof(workers[0])*num_workers);

    for ( idx = 0 ; idx < num_workers ; idx++)
    {
        check = pthread_create(
            &workers[idx],
            NULL,
            worker_thread,
            &context
        );
        if (check)
        {
            fprintf(stderr, 
                "Worker Thread No. %d Initialization failed!\n", 
                idx
            );
            exit(-1);
        }
    }

    /* Wait for all threads to finish. */
    for( idx = 0 ; idx < num_workers ; idx++)
    {
        pthread_join(workers[idx], NULL);
    }

#ifndef WIN32
            gettimeofday(&tv,&tz);
            printf("Finished at %li.%.6li (total_num_iters=%lli)\n",
                tv.tv_sec,
                tv.tv_usec,
                total_num_iters
                );
#else
            _ftime(&tb);
            printf(
#ifdef __GNUC__
                    "Finished at %li.%.6i (total_num_iters=%lli)\n",
#else
                    "Finshed at %li.%.6i (total_num_iters=%I64i)\n",
#endif
                tb.time,
                tb.millitm*1000,
                total_num_iters
            );
#endif

    free(workers);

    return 0;
}

