#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>

#include "fcs_cl.h"

struct freecell_solver_display_information_context_struct
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

typedef struct freecell_solver_display_information_context_struct freecell_solver_display_information_context_t;

static void init_debug_context(
    freecell_solver_display_information_context_t * dc
    )
{
    dc->parseable_output = 0;
    dc->canonized_order_output = 0;
    dc->display_10_as_t = 0;
    dc->display_parent_iter_num = 0;
    dc->display_moves = 0;
    dc->display_states = 1;
    dc->standard_notation = 0;
}



static void my_iter_handler(
    void * user_instance,
    int iter_num,
    int depth,
    void * ptr_state,
    int parent_iter_num,
    void * lp_context
    )
{
    freecell_solver_display_information_context_t * context;
    context = (freecell_solver_display_information_context_t*)lp_context;

    fprintf(stdout, "Iteration: %i\n", iter_num);
    fprintf(stdout, "Depth: %i\n", depth);
    fprintf(stdout, "Stored-States: %i\n",
        freecell_solver_user_get_num_states_in_collection(user_instance)
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


static char * help_string =
"fc-solve [options] board_file\n"
"\n"
"If board_file is - or unspecified reads standard input\n"
"\n"
"Available Options:\n"
"-h   --help           \n"
"     display this help screen\n"
"-i   --iter-output    \n"
"     display the iteration number and depth in every state that is checked\n"
"-s   --state-output   \n"
"     also output the state in every state that is checked\n"
"-p   --parseable-output \n"
"     Output the states in a format that is friendly to perl, grep and\n"
"     friends.\n"
"-c   --canonized-order-output \n"
"     Output the stacks and freecells according to their canonic order.\n"
"     (That means that stacks and freecells won't retain their place.)\n"
"-t   --display-10-as-t \n"
"     Display the card 10 as a capital T instead of \"10\".\n"
"-m   --display-moves \n"
"     Display the moves instead of the intermediate states.\n"
"-sam      --display-states-and-moves \n"
"     Display both intermediate states and moves.\n"
"-sn  --standard-notation \n"
"     Display the moves in standard (non-verbose) notation.\n"
"     (Applicable only if -m was specified)\n"
"-pi  --display-parent-iter \n"
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
"        'D' - move a freecel card to a parent.\n"
"        'E' - move a freecel card to an empty stack.\n"
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
"\n"
"        Tests are grouped with parenthesis or square brackets. Each group\n"
"        will be randomized as a whole by the random-dfs scan.\n"
"\n"
"\n"
"-me [solving_method]   --method [solving_method]\n"
"     Specify a solving method. Available methods are:\n"
"        \"a-star\" - A*\n"
"        \"bfs\" - Breadth-First Search\n"
"        \"dfs\" - Depth-First Search (default)\n"
"        \"random-dfs\" - A randomized DFS\n"
"        \"soft-dfs\" - \"Soft\" DFS\n"
"\n"
"-asw [A* Weights]   --a-star-weight [A* Weights]\n"
"     Specify weights for the A* scan, assuming it is used. The parameter\n"
"     should be a comma-separated list of numbers, each one is proportional\n"
"     to the weight of its corresponding test.\n"
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
"Signals:\n"
"SIGUSR1 - Prints the number of states that were checked so far to stderr.\n"
"SIGUSR2 SIGUSR1 - Turns iteration output on/off.\n"
"SIGUSR2 SIGUSR2 SIGUSR1 - Turns iteration's state output on/off.\n"
"\n"
"\n"
"Freecell Solver was written by Shlomi Fish.\n"
"Homepage: http://vipe.technion.ac.il/~shlomif/freecell-solver/\n"
"Send comments and suggestions to shlomif@vipe.technion.ac.il\n"
;

enum MY_FCS_CMD_LINE_RET_VALUES
{
    EXIT_AND_RETURN_0 = FCS_CMD_LINE_USER,

};

static int cmd_line_callback(
    void * instance,
    int argc,
    char * argv[],
    int arg,
    int * num_to_skip,
    int * ret,
    void * context
    )
{
    freecell_solver_display_information_context_t * dc;
    *num_to_skip = 0;

    dc = (freecell_solver_display_information_context_t * )context;

    if ((!strcmp(argv[arg], "-h")) || (!strcmp(argv[arg], "--help")))
    {
        printf("%s", help_string);
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
        dc->debug_iter_output_on = 1;

        set_iter_handler();
    }
    else if ((!strcmp(argv[arg], "-s")) || (!strcmp(argv[arg], "--state-output")))
    {
        set_iter_handler();
        dc->debug_iter_state_output = 1;
#undef set_iter_handler
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
static int debug_iter_output_on = 0;

static void select_signal_handler(int signal_num)
{
    command_num = (command_num+1)%3;
}

static void * current_instance;
static freecell_solver_display_information_context_t * dc;


static void command_signal_handler(int signal_num)
{
    if (command_num == 0)
    {
        fprintf(
            stderr,
            "The number of iterations is %i\n",
            freecell_solver_user_get_num_times(current_instance)
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
            debug_iter_output_on = 0;
        }
        else
        {
            freecell_solver_user_set_iter_handler(
                current_instance,
                my_iter_handler,
                dc
                );
            debug_iter_output_on = 1;
        }
    }
    else if (command_num == 2)
    {
        dc->debug_iter_state_output = ! dc->debug_iter_state_output;
    }

    command_num = 0;
}


static char * known_parameters[] = {
    "-h", "--help",
    "-i", "--iter-output",
    "-s", "--state-output",
    "-p", "--parseable-output",
    "-c", "--canonized-order-output",
    "-t", "--display-10-as-t",
    "-m", "--display-moves",
    "-sn", "--standard-notation",
    "-sam", "--display-states-and-moves",
    "-pi", "--display-parent-iter",
    "--reset",
    NULL
    };


int main(int argc, char * argv[])
{
    int parser_ret;
    void * instance;
    char * error_string;
    int arg;
    FILE * file;
    char user_state[1024];
    int ret;

    freecell_solver_display_information_context_t debug_context;

    init_debug_context(&debug_context);

    dc = &debug_context;

    instance = freecell_solver_user_alloc();

    current_instance = instance;


    parser_ret =
        freecell_solver_user_cmd_line_parse_args(
            instance,
            argc,
            argv,
            1,
            known_parameters,
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
    else if (
        (parser_ret == FCS_CMD_LINE_PARAM_WITH_NO_ARG)
            )
    {
        fprintf(stderr, "The command line parameter \"%s\" requires an argument"
                " and was not supplied with one.\n", argv[arg]);
        return (-1);
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
        freecell_solver_user_free(instance);
        return -1;
    }

    if ((arg == argc) || (!strcmp(argv[arg], "-")))
    {
        file = stdin;
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
    fread(user_state, sizeof(user_state[0]), sizeof(user_state)/sizeof(user_state[0]), file);
    fclose(file);

    /* Win32 Does not have those signals */
#ifndef WIN32
    signal(SIGUSR1, command_signal_handler);
    signal(SIGUSR2, select_signal_handler);
#endif

#if 1
    {
        int limit = 500;
        freecell_solver_user_limit_iterations(instance, limit);
        ret = freecell_solver_user_solve_board(instance, user_state);
        while (ret == FCS_STATE_SUSPEND_PROCESS)
        {
            limit += 500;
            freecell_solver_user_limit_iterations(instance, limit);
            ret = freecell_solver_user_resume_solution(instance);
        }
    }
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
    }
    else
    {
        if (ret == FCS_STATE_WAS_SOLVED)
        {
            printf("-=-=-=-=-=-=-=-=-=-=-=-\n\n");
            {
                fcs_move_t move;
                FILE * move_dump;
                char * as_string;
                int move_num = 0;

                move_dump = stdout;


                if (debug_context.display_states)
                {
                    as_string =
                        freecell_solver_user_current_state_as_string(
                            instance,
                            debug_context.parseable_output,
                            debug_context.canonized_order_output,
                            debug_context.display_10_as_t
                            );

                    fprintf(move_dump, "%s\n", as_string);

                    free(as_string);

                    fprintf(move_dump, "%s", "\n====================\n\n");
                }

                while (
                        freecell_solver_user_get_next_move(
                            instance,
                            &move
                            ) == 0
                        )
                {
                    if (debug_context.display_moves)
                    {
                        as_string =
                            freecell_solver_user_move_to_string(
                                move,
                                debug_context.standard_notation
                                );

                        if (debug_context.display_states && debug_context.standard_notation)
                        {
                            fprintf(move_dump, "Move: ");
                        }

                        fprintf(
                            move_dump,
                            (debug_context.standard_notation ?
                                "%s " :
                                "%s\n"
                            ),
                            as_string
                            );
                        move_num++;
                        if (debug_context.standard_notation)
                        {
                            if ((move_num % 10 == 0) || debug_context.display_states)
                            {
                                fprintf(move_dump, "\n");
                            }
                        }
                        if (debug_context.display_states)
                        {
                            fprintf(move_dump, "\n");
                        }
                        fflush(move_dump);
                        free(as_string);
                    }

                    if (debug_context.display_states)
                    {
                        as_string =
                            freecell_solver_user_current_state_as_string(
                                instance,
                                debug_context.parseable_output,
                                debug_context.canonized_order_output,
                                debug_context.display_10_as_t
                                );

                        fprintf(move_dump, "%s\n", as_string);

                        free(as_string);
                    }

                    if (debug_context.display_states || (!debug_context.standard_notation))
                    {
                        fprintf(move_dump, "%s", "\n====================\n\n");
                    }
                }

                if (debug_context.standard_notation && (!debug_context.display_states))
                {
                    fprintf(move_dump, "\n\n");
                }
            }

            printf("This game is solveable.\n");
        }
        else
        {
            printf ("I could not solve this game.\n");
        }

        printf(
            "Total number of states checked is %i.\n",
            freecell_solver_user_get_num_times(instance)
            );
#if 1
        printf(
            "This scan generated %i states.\n",
            freecell_solver_user_get_num_states_in_collection(instance)
            );
#endif
    }

    freecell_solver_user_free(instance);

    return 0;
}

