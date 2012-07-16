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
 * pruner-main.c : pruning solver.
 */
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>

#include "fcs_cl.h"
#include "unused.h"
#include "bool.h"

#ifdef FCS_TRACE_MEM
#include "portable_time.h"
#include <unistd.h>
#include <sys/types.h>
#endif

#include "output_to_file.h"

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
    fprintf(stdout, "Stored-States: %li\n",
        (long)freecell_solver_user_get_num_states_in_collection_long(user_instance)
        );
    fprintf(stdout, "Scan: %s\n",
        freecell_solver_user_get_current_soft_thread_name(user_instance)
        );
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
                1,
                0,
                1
                );
        printf("%s\n---------------\n\n\n", state_string);

        /* Process the string in-place to make it available as input
         * to fc-solve again.
         * */

        {
            char * s, * d;
            char c;
            s = d = state_string;

            while ((c = *(d++) = *(s++)))
            {
                if ((c == '\n') && (s[0] == ':') && (s[1] = ' '))
                {
                    s += 2;
                }
            }
        }

        /* Now pass it to a secondary user_instance prune it. */
        {
            void * pruner;
            char * error_string;
            int ret;

            pruner = freecell_solver_user_alloc();

            freecell_solver_user_set_num_freecells(pruner, 2);
            freecell_solver_user_set_tests_order(pruner, "01ABCDE", &error_string);
            freecell_solver_user_limit_iterations_long(pruner, 128*1024);

            ret = freecell_solver_user_solve_board(pruner, state_string);
            if (ret == FCS_STATE_SUSPEND_PROCESS)
            {
                printf ("\n\nVerdict: INDETERMINATE\n\n");
            }
            else if (ret == FCS_STATE_WAS_SOLVED)
            {
                printf("\n\nVerdict: SOLVED\n\nYay! We found a solution from this one.");
                exit(0);
            }
            else if (ret == FCS_STATE_IS_NOT_SOLVEABLE)
            {
                printf("\n\nVerdict: PRUNED\n\n");
            }
            else
            {
                printf("\n\nVerdict: unknown ret code: %d\n\n", ret);
                exit(-1);
            }

            freecell_solver_user_free(pruner);
        }

        free((void*)state_string);
    }
}

typedef struct
{
    const char * key;
    const char * screen;
} help_screen_t;

help_screen_t help_screens[] = {
{
    "configs",
"These configurations are usually faster than the unmodified run:\n"
"\n"
"    fc-solve -l foss-nessy\n"
"    fc-solve -l the-iglu-cabal\n"
"\n"
"Or if you want an accurate verdict:\n"
"\n"
"    fc-solve -l toons-for-twenty-somethings\n"
"\n"
"If you want to try constructing your own configurations refer to the\n"
"USAGE file in the Freecell Solver distribution\n"
},
{
    "options",
"fc-solve [options] board_file\n"
"\n"
"If board_file is - or unspecified reads standard input\n"
"\n"
"Available Options:\n"
"-h     --help\n"
"     display the default help screen\n"
"--help-summary\n"
"     display the summary help screen\n"
"--version\n"
"     display the version number of the components\n"
"-o [Filename] --output [Filename]\n"
"     Redirect output to a file.\n"
"-i     --iter-output\n"
"     display the iteration number and depth in every state that is checked\n"
"-s     --state-output\n"
"     also output the state in every state that is checked\n"
"-p     --parseable-output\n"
"     Output the states in a format that is friendly to perl, grep and\n"
"     friends.\n"
"-c     --canonized-order-output\n"
"     Output the stacks and freecells according to their canonic order.\n"
"     (That means that stacks and freecells won't retain their place.)\n"
"-t     --display-10-as-t\n"
"     Display the card 10 as a capital T instead of \"10\".\n"
"-m     --display-moves\n"
"     Display the moves instead of the intermediate states.\n"
"-sam   --display-states-and-moves \n"
"     Display both intermediate states and moves.\n"
"-sn    --standard-notation\n"
"     Display the moves in standard (non-verbose) notation.\n"
"     (Applicable only if -m was specified)\n"
"-snx   --standard-notation-extended\n"
"     Display the moves in extended standard notation while specifying the\n"
"     number of cards moved if applicable\n"
"-pi    --display-parent-iter \n"
"     Display the index of the parent iteration of each state in the\n"
"     run-time dump.\n"
"\n"
"--freecells-num [Freecells\' Number]\n"
"     The number of freecells present in the board.\n"
"--stacks-num [Stacks\' Number]\n"
"     The number of stacks present in the board.\n"
"--decks-num [Decks\' Number]\n"
"     The number of decks in the board.\n"
"\n"
"--sequences-are-built-by {suit|alternate_color|rank}\n"
"     Specifies the type of sequence\n"
"--sequence-move {limited|unlimited}\n"
"     Specifies whether the sequence move is limited by the number of\n"
"     freecells or vacant stacks or not.\n"
"--empty-stacks-filled-by {kings|none|all}\n"
"     Specifies which cards can fill empty stacks.\n"
"\n"
"--game [game]   --preset [game]  -g [game]\n"
"     Specifies the type of game. (Implies several of the game settings\n"
"     options above.). Available presets:\n"
"     bakers_dozen       - Baker\'s Dozen\n"
"     bakers_game        - Baker\'s Game\n"
"     beleaguered_castle - Beleaguered Castle\n"
"     citadel            - Citadel\n"
"     cruel              - Cruel\n"
"     der_katz           - Der Katzenschwanz\n"
"     die_schlange       - Die Schlange\n"
"     eight_off          - Eight Off\n"
"     fan                - Fan\n"
"     forecell           - Forecell\n"
"     freecell           - Freecell\n"
"     good_measure       - Good Measure\n"
"     ko_bakers_game     - Kings\' Only Baker\'s Game\n"
"     relaxed_freecell   - Relaxed Freecell\n"
"     relaxed_seahaven   - Relaxed Seahaven Towers\n"
"     seahaven           - Seahaven Towers\n"
"     simple_simon       - Simple Simon\n"
"     streets_and_alleys - Streets and Alleys\n"
"\n"
"-md [depth]       --max-depth [depth] \n"
"     Specify a maximal search depth for the solution process.\n"
"-mi [iter_num]    --max-iters [iter_num] \n"
"     Specify a maximal number of iterations number.\n"
"-mss [states_num] --max-stored-states [states_num] \n"
"     Specify the maximal number of states stored in memory.\n"
"\n"
"-to [tests_order]   --tests-order  [tests_order] \n"
"     Specify a test order string. Each test is represented by one character.\n"
"     Valid tests:\n"
"        Freecell Tests:\n"
"\n"
"        '0' - put top stack cards in the foundations.\n"
"        '1' - put freecell cards in the foundations.\n"
"        '2' - put freecell cards on top of stacks.\n"
"        '3' - put non-top stack cards in the foundations.\n"
"        '4' - move stack cards to different stacks.\n"
"        '5' - move stack cards to a parent card on the same stack.\n"
"        '6' - move sequences of cards onto free stacks.\n"
"        '7' - put freecell cards on empty stacks.\n"
"        '8' - move cards to a different parent.\n"
"        '9' - empty an entire stack into the freecells.\n"
"\n"
"        Atomic Freecell Tests:\n"
"\n"
"        'A' - move a stack card to an empty stack.\n"
"        'B' - move a stack card to a parent on a different stack.\n"
"        'C' - move a stack card to a freecell.\n"
"        'D' - move a freecell card to a parent.\n"
"        'E' - move a freecell card to an empty stack.\n"
"\n"
"        Simple Simon Tests:\n"
"\n"
"        'a' - move a full sequence to the foundations.\n"
"        'b' - move a sequence to a true parent of his.\n"
"        'c' - move a whole stack sequence to a false parent (in order to\n"
"              clear the stack)\n"
"        'd' - move a sequence to a true parent that has some cards above it.\n"
"        'e' - move a sequence with some cards above it to a true parent.\n"
"        'f' - move a sequence with a junk sequence above it to a true parent\n"
"              that has some cards above it.\n"
"        'g' - move a whole stack sequence to a false parent which has some\n"
"              cards above it.\n"
"        'h' - move a sequence to a parent on the same stack.\n"
"        'i' - move a sequence to a false parent.\n"
"\n"
"        Tests are grouped with parenthesis or square brackets. Each group\n"
"        will be randomized as a whole by the random-dfs scan.\n"
"\n"
"-dto [min_d],[tests_order]   --depth-tests-order [min_d],[tests_order]\n"
"     Like --to / --tests-order only assigns the tests starting from the\n"
"     minimal depth.\n"
"\n"
"\n"
"-me [solving_method]   --method [solving_method]\n"
"     Specify a solving method. Available methods are:\n"
"        \"a-star\" - Best-First-Search\n"
"        \"bfs\" - Breadth-First Search\n"
"        \"dfs\" - Depth-First Search (default)\n"
"        \"random-dfs\" - A randomized DFS\n"
"        \"soft-dfs\" - \"Soft\" DFS\n"
"\n"
"-asw [BeFS Weights]   --a-star-weight [BeFS Weights]\n"
"     Specify weights for the Best-first-search scan, assuming it is used.\n"
"     The parameter should be a comma-separated list of numbers, each one is\n"
"     proportional to the weight of its corresponding test.\n"
"\n"
"     The numbers are, in order:\n"
"     1. The number of cards out.\n"
"     2. The maximal sequence move.\n"
"     3. The number of cards under sequences.\n"
"     4. The length of the sequences which are found over renegade cards.\n"
"     5. The depth of the board in the solution.\n"
"\n"
"-seed [seed_number]\n"
"     Set the seed for the random number generator used by the\n"
"     \"random-dfs\" scan.\n"
"\n"
"-nst         --next-soft-thread\n"
"     Move to the next Soft-Thread. I.e: input another scan to run in\n"
"     parallel.\n"
"-step [step iterations]     --soft-thread-step [step iterations]\n"
"     Set the number of iterations in the step of the current soft-thread.\n"
"-nht        --next-hard-thread\n"
"     Move to the next Hard-Thread. This is a new group of scans to run\n"
"     in their own system thread (assuming the executable was compiled with\n"
"     support for them.)\n"
"--st-name\n"
"     Set the name of the soft-thread.\n"
"\n"
"--prelude [prelude_string]\n"
"     Set the prelude string of the hard thread. A prelude is a static\n"
"     sequence of iterations quotas that are executed at the beginning of\n"
"     the search. The format is a list of [Limit]@[Soft-Thread Name]\n"
"     delimited by commas.\n"
"\n"
"-ni         --next-instance\n"
"     Move to the next distinct solver instance. This is a separate scan\n"
"     which would run only if the previous ones returned an unsolvable\n"
"     verdict.\n"
"\n"
"-opt         --optimize-solution\n"
"     Try and optimize the solution for a small number of moves.\n"
"-opt-to      --optimization-tests-order\n"
"     The test order of the optimization scan.\n"
"\n"
"\n"
"--reparent-states\n"
"     Reparent states that have a larger depth than that of the state\n"
"     from which they were reached a posteriori.\n"
"--calc-real-depth\n"
"     If --reparent-states is enabled, then explictly calculate the real\n"
"     depth of a state by tracing its path to the initial state\n"
"--scans-synergy {none|dead-end-marks}\n"
"     Specifies the cooperation between the scans.\n"
"\n"
"\n"
"--reset\n"
"     Reset the program to its initial, unconfigured state.\n"
"--read-from-file [{num_skip},]filename\n"
"     Reads configuration parameter with the file while skipping num_skip\n"
"     arguments from the beginning.\n"
"-l [configuration]      --load-config [configuration]\n"
"     Reads the configuration [configruration] and configures the solver\n"
"     accordingly.\n"
"\n"
"\n"
"Signals:\n"
"SIGUSR1 - Prints the number of states that were checked so far to stderr.\n"
"SIGUSR2 SIGUSR1 - Turns iteration output on/off.\n"
"SIGUSR2 SIGUSR2 SIGUSR1 - Turns iteration's state output on/off.\n"
"\n"
"\n"
"Freecell Solver was written by Shlomi Fish.\n"
"Homepage: http://fc-solve.shlomifish.org/\n"
"Send comments and suggestions to http://www.shlomifish.org/me/contact-me/\n"
},
{
    "real-help",
"The environment variable FREECELL_SOLVER_DEFAULT_HELP sets the default help\n"
"screen. The name of the help screen is the same name as its \"--help-\" flag\n"
"but without the preceding \"--help-\". Type:\n"
"\n"
"    fc-solve --help-summary\n"
"\n"
"for the available help screens.\n"
"\n"
"Refer to your system's documentation for information on how to set environment\n"
"variables.\n"
},
{
    "problems",
"To be discussed.\n"
},
{
    "short-sol",
"The following configurations may produce shorter solutions:\n"
"\n"
"    fc-solve -l children-playing-ball\n"
"    fc-solve -l gooey-unknown-thing\n"
"    fc-solve -l slick-rock\n"
"\n"
"You may also try adding the \"-opt\" and/or \"--reparent-states\" options\n"
"which may make things a little better.\n"
"\n"
"Refer to the file 'USAGE' for more information.\n"
},
{
    "summary",
"fc-solve [flags] [board_file|-]\n"
"\n"
"Reads board from standard input by default or if a \"-\" is specified.\n"
"\n"
"- If it takes too long to finish, type \"fc-solve --help-configs\"\n"
"- If it erroneously reports a board as unsolvable, try adding the\n"
"  \"-to 01ABCDE\" flag\n"
"- If the solution is too long type \"fc-solve --help-short-sol\"\n"
"- To present the moves only try adding \"-m\" or \"-m -snx\"\n"
"- For a description of all options type \"fc-solve --help-options\"\n"
"- To deal with other problems type \"fc-solve --help-problems\"\n"
"- To turn --help into something more useful, type\n"
"  \"fc-solve --help-real-help\"\n"
"\n"
"Contact Shlomi Fish, http://www.shlomifish.org/ for more information.\n"
},
{
    NULL,
    NULL
}
}
;

enum
{
    EXIT_AND_RETURN_0 = FCS_CMD_LINE_USER
};

static void print_help_string(const char * key)
{
    int i;
    for(i=0;help_screens[i].key != NULL ; i++)
    {
        if (!strcmp(key, help_screens[i].key))
        {
            printf("%s", help_screens[i].screen);
        }
    }
}

static int cmd_line_callback(
    void * instance,
    int argc,
    const char * argv[],
    int arg,
    int * num_to_skip,
    int * ret,
    void * context
    )
{
    fc_solve_display_information_context_t * dc;
    *num_to_skip = 0;

    dc = (fc_solve_display_information_context_t * )context;

    if (!strcmp(argv[arg], "--version"))
    {
        printf(
            "fc-solve\nlibfreecell-solver version %s\n",
            freecell_solver_user_get_lib_version(instance)
        );
        *ret = EXIT_AND_RETURN_0;
        return FCS_CMD_LINE_STOP;
    }
    else if ((!strcmp(argv[arg], "-h")) || (!strcmp(argv[arg], "--help")))
    {
        const char * help_key;

        help_key = getenv("FREECELL_SOLVER_DEFAULT_HELP");
        if (help_key == NULL)
        {
            help_key = "summary";
        }
        print_help_string(help_key);
        *ret = EXIT_AND_RETURN_0;
        return FCS_CMD_LINE_STOP;
    }
    else if (!strncmp(argv[arg], "--help-", 7))
    {
        print_help_string(argv[arg]+7);
        *ret = EXIT_AND_RETURN_0;
        return FCS_CMD_LINE_STOP;
    }
    else if ((!strcmp(argv[arg], "-i")) || (!strcmp(argv[arg], "--iter-output")))
    {
#define set_iter_handler() \
        freecell_solver_user_set_iter_handler(   \
            instance,   \
            my_iter_handler,   \
            dc    \
            );        \
        dc->debug_iter_output_on = TRUE;

        set_iter_handler();
    }
    else if ((!strcmp(argv[arg], "-s")) || (!strcmp(argv[arg], "--state-output")))
    {
        set_iter_handler();
        dc->debug_iter_state_output = TRUE;
#undef set_iter_handler
    }
    else if ((!strcmp(argv[arg], "-p")) || (!strcmp(argv[arg], "--parseable-output")))
    {
        dc->parseable_output = TRUE;
    }
    else if ((!strcmp(argv[arg], "-c")) || (!strcmp(argv[arg], "--canonized-order-output")))
    {
        dc->canonized_order_output = TRUE;
    }
    else if ((!strcmp(argv[arg], "-o")) || (!strcmp(argv[arg], "--output")))
    {
        arg++;
        if (arg == argc)
        {
            return FCS_CMD_LINE_STOP;
        }
        *num_to_skip = 2;
        dc->output_filename = (const char *)argv[arg];
        return FCS_CMD_LINE_SKIP;
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
    else if ((!strcmp(argv[arg], "-sn")) || (!strcmp(argv[arg], "--standard-notation")))
    {
        dc->standard_notation = STANDARD_NOTATION_REGULAR;

    }
    else if ((!strcmp(argv[arg], "-snx")) || (!strcmp(argv[arg], "--standard-notation-extended")))
    {
        dc->standard_notation = STANDARD_NOTATION_EXTENDED;
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
    else if ((!strcmp(argv[arg], "--reset")))
    {
        init_debug_context(dc);
        freecell_solver_user_set_iter_handler(
            instance,
            NULL,
            NULL
            );
        *num_to_skip = 0;
        return FCS_CMD_LINE_OK;
    }
    else
    {
        printf("Unimplemented option - \"%s\"!", argv[arg]);
        exit(-1);
    }
    *num_to_skip = 1;
    return FCS_CMD_LINE_SKIP;
}


static int command_num = 0;
static int debug_iter_output_on = FALSE;

static void select_signal_handler(int signal_num GCC_UNUSED)
{
    command_num = (command_num+1)%3;
}

static void * current_instance;
static fc_solve_display_information_context_t * dc;


static void command_signal_handler(int signal_num GCC_UNUSED)
{
    if (command_num == 0)
    {
        fprintf(
            stderr,
            "The number of iterations is %li\n",
            (long)freecell_solver_user_get_num_times_long(current_instance)
            );
    }
    else if (command_num == 1)
    {
        if (debug_iter_output_on)
        {
            freecell_solver_user_set_iter_handler(
                current_instance,
                NULL,
                NULL
                );
            debug_iter_output_on = FALSE;
        }
        else
        {
            freecell_solver_user_set_iter_handler(
                current_instance,
                my_iter_handler,
                dc
                );
            debug_iter_output_on = TRUE;
        }
    }
    else if (command_num == 2)
    {
        dc->debug_iter_state_output = ! dc->debug_iter_state_output;
    }

    command_num = 0;
}


static freecell_solver_str_t known_parameters[] = {
    "-h", "--help",
        "--help-configs", "--help-options", "--help-problems",
        "--help-real-help", "--help-short-sol", "--help-summary",
    "-i", "--iter-output",
    "-s", "--state-output",
    "-p", "--parseable-output",
    "-c", "--canonized-order-output",
    "-t", "--display-10-as-t",
    "-m", "--display-moves",
    "-sn", "--standard-notation",
    "-snx", "--standard-notation-extended",
    "-sam", "--display-states-and-moves",
    "-pi", "--display-parent-iter",
    "-o", "--output",
    "--reset",
    "--version",
    NULL
    };

#define USER_STATE_SIZE 1024

int main(int argc, char * argv[])
{
    int parser_ret;
    void * instance;
    int arg;
    FILE * file;
    char user_state[USER_STATE_SIZE];
    int ret;

    fc_solve_display_information_context_t debug_context;

    init_debug_context(&debug_context);

    dc = &debug_context;

    instance = freecell_solver_user_alloc();

    current_instance = instance;


    {
        char * error_string;
        parser_ret =
            freecell_solver_user_cmd_line_parse_args(
                instance,
                argc,
                (freecell_solver_str_t *)(void *)argv,
                1,
                (freecell_solver_str_t *)known_parameters,
                cmd_line_callback,
                &debug_context,
                &error_string,
                &arg
                );

        if (parser_ret == EXIT_AND_RETURN_0)
        {
            freecell_solver_user_free(instance);
            return 0;
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
            freecell_solver_user_free(instance);
            return -1;
        }
    }

    if ((arg == argc) || (!strcmp(argv[arg], "-")))
    {
        file = stdin;
        if (!getenv("FREECELL_SOLVER_QUIET"))
        {
            fprintf(stderr, "%s",
                    "Reading the board from the standard input.\n"
                    "Type \"fc-solve --help\" for more usage information.\n"
                    "To cancel this message set the FREECELL_SOLVER_QUIET environment variable.\n"
                   );
        }
    }
    else if (argv[arg][0] == '-')
    {
        fprintf(stderr,
                "Unknown option \"%s\". "
                "Type \"%s --help\" for usage information.\n",
                argv[arg],
                argv[0]
                );
        freecell_solver_user_free(instance);

        return -1;
    }
    else
    {
        file = fopen(argv[arg], "r");
        if (file == NULL)
        {
            fprintf(stderr,
                "Could not open file \"%s\" for input. Exiting.\n",
                argv[arg]
                );
            freecell_solver_user_free(instance);

            return -1;
        }
    }
    memset(user_state, '\0', sizeof(user_state));
    fread(user_state, sizeof(user_state[0]), USER_STATE_SIZE-1, file);
    fclose(file);

    /* Win32 Does not have those signals */
#ifndef WIN32
    signal(SIGUSR1, command_signal_handler);
    signal(SIGUSR2, select_signal_handler);
#endif

#if 0
    {
        fcs_int_limit_t limit = 500;
        freecell_solver_user_limit_iterations_long(instance, limit);
        ret = freecell_solver_user_solve_board(instance, user_state);
        while (ret == FCS_STATE_SUSPEND_PROCESS)
        {
            limit += 500;
            freecell_solver_user_limit_iterations_long(instance, limit);
            ret = freecell_solver_user_resume_solution(instance);
        }
    }
#elif defined(FCS_TRACE_MEM)
    {
#define STEP 100000
        fcs_int_limit_t limit = STEP;
        char stat_fn[1024], foo_str[1024];
        fcs_portable_time_t mytime;
        FILE * stat;
        long long int rss;
        unsigned long long unsigned_foo;

        snprintf(stat_fn, sizeof(stat_fn), "/proc/%ld/stat", (long)(getpid()));

        freecell_solver_user_limit_iterations_long(instance, limit);
        ret = freecell_solver_user_solve_board(instance, user_state);
        while (ret == FCS_STATE_SUSPEND_PROCESS)
        {
            FCS_GET_TIME(mytime);

            /* This was taken from:
             *
             * http://www.brokestream.com/procstat.html
             * */
            stat = fopen(stat_fn, "r");
#define readone(foo) (fscanf(stat, "%lld ", &rss))
#define readstr(foo) (fscanf(stat, "%1000s ", foo_str))
#define readchar(foo) (fscanf(stat, "%c ", foo_str))
#define readunsigned(foo) (fscanf(stat, "%llu ", &unsigned_foo))
            readone(&pid);
            readstr(tcomm);
            readchar(&state);
            readone(&ppid);
            readone(&pgid);
            readone(&sid);
            readone(&tty_nr);
            readone(&tty_pgrp);
            readone(&flags);
            readone(&min_flt);
            readone(&cmin_flt);
            readone(&maj_flt);
            readone(&cmaj_flt);
            readone(&utime);
            readone(&stimev);
            readone(&cutime);
            readone(&cstime);
            readone(&priority);
            readone(&nicev);
            readone(&num_threads);
            readone(&it_real_value);
            readunsigned(&start_time);
            readone(&vsize);
            readone(&rss);
#undef readone
#undef readunsigned
#undef readchar
#undef readstr

            fclose(stat);

            printf("Reached:\t%d\t%li.%.6li\t%lld\n",
                    limit,
                    FCS_TIME_GET_SEC(mytime),
                    FCS_TIME_GET_USEC(mytime),
                    rss
                  );

            fflush(stdout);
            limit += STEP;
            freecell_solver_user_limit_iterations_long(instance, limit);
            ret = freecell_solver_user_resume_solution(instance);
        }
    }
#undef STEP
#else
    ret = freecell_solver_user_solve_board(instance, user_state);
#endif

    if (ret == FCS_STATE_INVALID_STATE)
    {
        char * error_string;

        error_string =
            freecell_solver_user_get_invalid_state_error_string(
                instance,
                debug_context.display_10_as_t
                );
        printf("%s\n", error_string);
        free(error_string);
        error_string = NULL;
    }
    else if (ret == FCS_STATE_FLARES_PLAN_ERROR)
    {
        const char * error_string;

        error_string = freecell_solver_user_get_last_error_string(instance);

        printf("Flares Plan: %s\n", error_string);
    }
    else
    {
        FILE * output_fh;

        if (debug_context.output_filename)
        {
            output_fh = fopen(debug_context.output_filename, "wt");
            if (! output_fh)
            {
                fprintf(stderr,
                        "Could not open output file '%s' for writing!",
                        debug_context.output_filename
                       );
                return -1;
            }
        }
        else
        {
            output_fh = stdout;
        }

        fc_solve_output_result_to_file(
            output_fh, instance, ret, &debug_context
        );

        if (debug_context.output_filename)
        {
            fclose(output_fh);
            output_fh = NULL;
        }
    }

    freecell_solver_user_free(instance);

    return 0;
}

