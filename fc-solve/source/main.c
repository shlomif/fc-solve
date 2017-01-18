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
 * main.c - the main() and related functions of the fc-solve command line
 * executable.
 *
 * It is documented in the documents "README", "USAGE", etc. in the
 * Freecell Solver distribution from http://fc-solve.shlomifish.org/ .
 */
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>

#include "fcs_cl.h"
#include "rinutils.h"
#include "output_to_file.h"
#include "handle_parsing.h"
#include "read_state.h"

typedef struct
{
    const char *key;
    const char *screen;
} help_screen_t;

help_screen_t help_screens[] = {
    {"configs",
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
        "USAGE file in the Freecell Solver distribution\n"},
    {"options",
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
        "     display the iteration number and depth in every state that is "
        "checked\n"
        "-s     --state-output\n"
        "     also output the state in every state that is checked\n"
        "-p     --parseable-output\n"
        "     Output the states in a format that is friendly to perl, grep "
        "and\n"
        "     friends.\n"
        "-c     --canonized-order-output\n"
        "     Output the stacks and freecells according to their canonic "
        "order.\n"
        "     (That means that stacks and freecells won't retain their "
        "place.)\n"
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
        "     Display the moves in extended standard notation while specifying "
        "the\n"
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
        "     Specifies the type of game. (Implies several of the game "
        "settings\n"
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
        "     Specify a test order string. Each test is represented by one "
        "character.\n"
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
        "        'c' - move a whole stack sequence to a false parent (in order "
        "to\n"
        "              clear the stack)\n"
        "        'd' - move a sequence to a true parent that has some cards "
        "above "
        "it.\n"
        "        'e' - move a sequence with some cards above it to a true "
        "parent.\n"
        "        'f' - move a sequence with a junk sequence above it to a true "
        "parent\n"
        "              that has some cards above it.\n"
        "        'g' - move a whole stack sequence to a false parent which has "
        "some\n"
        "              cards above it.\n"
        "        'h' - move a sequence to a parent on the same stack.\n"
        "        'i' - move a sequence to a false parent.\n"
        "\n"
        "        Tests are grouped with parenthesis or square brackets. Each "
        "group\n"
        "        will be randomized as a whole by the random-dfs scan.\n"
        "\n"
        "-dto [min_d],[tests_order]   --depth-tests-order "
        "[min_d],[tests_order]\n"
        "     Like --to / --tests-order only assigns the tests starting from "
        "the\n"
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
        "     Specify weights for the Best-first-search scan, assuming it is "
        "used.\n"
        "     The parameter should be a comma-separated list of numbers, each "
        "one "
        "is\n"
        "     proportional to the weight of its corresponding test.\n"
        "\n"
        "     The numbers are, in order:\n"
        "     1. The number of cards out.\n"
        "     2. The maximal sequence move.\n"
        "     3. The number of cards under sequences.\n"
        "     4. The length of the sequences which are found over renegade "
        "cards.\n"
        "     5. The depth of the board in the solution.\n"
        "     6. Inverse of number of cards not above their parents.\n"
        "\n"
        "-seed [seed_number]\n"
        "     Set the seed for the random number generator used by the\n"
        "     \"random-dfs\" scan.\n"
        "\n"
        "-nst         --next-soft-thread\n"
        "     Move to the next Soft-Thread. I.e: input another scan to run in\n"
        "     parallel.\n"
        "-step [step iterations]     --soft-thread-step [step iterations]\n"
        "     Set the number of iterations in the step of the current "
        "soft-thread.\n"
        "-nht        --next-hard-thread\n"
        "     Move to the next Hard-Thread. This is a new group of scans to "
        "run\n"
        "     in their own system thread (assuming the executable was compiled "
        "with\n"
        "     support for them.)\n"
        "--st-name\n"
        "     Set the name of the soft-thread.\n"
        "\n"
        "--prelude [prelude_string]\n"
        "     Set the prelude string of the hard thread. A prelude is a "
        "static\n"
        "     sequence of iterations quotas that are executed at the beginning "
        "of\n"
        "     the search. The format is a list of [Limit]@[Soft-Thread Name]\n"
        "     delimited by commas.\n"
        "\n"
        "-ni         --next-instance\n"
        "     Move to the next distinct solver instance. This is a separate "
        "scan\n"
        "     which would run only if the previous ones returned an "
        "unsolvable\n"
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
        "     If --reparent-states is enabled, then explicitly calculate the "
        "real\n"
        "     depth of a state by tracing its path to the initial state\n"
        "--scans-synergy {none|dead-end-marks}\n"
        "     Specifies the cooperation between the scans.\n"
        "\n"
        "\n"
        "--reset\n"
        "     Reset the program to its initial, unconfigured state.\n"
        "--read-from-file [{num_skip},]filename\n"
        "     Reads configuration parameter with the file while skipping "
        "num_skip\n"
        "     arguments from the beginning.\n"
        "-l [configuration]      --load-config [configuration]\n"
        "     Reads the configuration [configruration] and configures the "
        "solver\n"
        "     accordingly.\n"
        "\n"
        "\n"
        "Signals:\n"
        "SIGUSR1 - Prints the number of states that were checked so far to "
        "stderr.\n"
        "SIGUSR2 SIGUSR1 - Turns iteration output on/off.\n"
        "SIGUSR2 SIGUSR2 SIGUSR1 - Turns iteration's state output on/off.\n"
        "\n"
        "\n"
        "Freecell Solver was written by Shlomi Fish.\n"
        "Homepage: http://fc-solve.shlomifish.org/\n"
        "Send comments and suggestions to "
        "http://www.shlomifish.org/me/contact-me/\n"},
    {"real-help", "The environment variable FREECELL_SOLVER_DEFAULT_HELP sets "
                  "the default help\n"
                  "screen. The name of the help screen is the same name as its "
                  "\"--help-\" flag\n"
                  "but without the preceding \"--help-\". Type:\n"
                  "\n"
                  "    fc-solve --help-summary\n"
                  "\n"
                  "for the available help screens.\n"
                  "\n"
                  "Refer to your system's documentation for information on how "
                  "to set environment\n"
                  "variables.\n"},
    {"problems", "To be discussed.\n"},
    {"short-sol",
        "The following configurations may produce shorter solutions:\n"
        "\n"
        "    fc-solve -l children-playing-ball\n"
        "    fc-solve -l gooey-unknown-thing\n"
        "    fc-solve -l slick-rock\n"
        "\n"
        "You may also try adding the \"-opt\" and/or \"--reparent-states\" "
        "options\n"
        "which may make things a little better.\n"
        "\n"
        "Refer to the file 'USAGE' for more information.\n"},
    {"summary",
        "fc-solve [flags] [board_file|-]\n"
        "\n"
        "Reads board from standard input by default or if a \"-\" is "
        "specified.\n"
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
        "Contact Shlomi Fish, http://www.shlomifish.org/ for more "
        "information.\n"},
    {NULL, NULL}};

static void print_help_string(const char *key)
{
    int i;
    for (i = 0; help_screens[i].key != NULL; i++)
    {
        if (!strcmp(key, help_screens[i].key))
        {
            printf("%s", help_screens[i].screen);
        }
    }
}

#include "cl_callback_common.h"
static int fc_solve__cmd_line_callback(void *const instance, const int argc,
    const char *argv[], const int arg, int *const num_to_skip, int *const ret,
    void *const context)
{
    fc_solve_display_information_context_t *const display_context =
        (fc_solve_display_information_context_t *)context;
    const char *s;

    *num_to_skip = 0;

    const char *const arg_str = argv[arg];

    if (cmd_line_cb__handle_common(arg_str, instance, display_context))
    {
    }
    else if (IS_ARG("--version"))
    {
        printf("fc-solve\nlibfreecell-solver version %s\n",
            freecell_solver_user_get_lib_version(instance));
        *ret = EXIT_AND_RETURN_0;
        return FCS_CMD_LINE_STOP;
    }
    else if (IS_ARG("-h") || IS_ARG("--help"))
    {
        const char *help_key = getenv("FREECELL_SOLVER_DEFAULT_HELP");
        if (help_key == NULL)
        {
            help_key = "summary";
        }
        print_help_string(help_key);
        *ret = EXIT_AND_RETURN_0;
        return FCS_CMD_LINE_STOP;
    }
    else if ((s = try_str_prefix(arg_str, "--help-")))
    {
        print_help_string(s);
        *ret = EXIT_AND_RETURN_0;
        return FCS_CMD_LINE_STOP;
    }
    else if (IS_ARG("-o") || IS_ARG("--output"))
    {
        const int next_arg = arg + 1;
        if (next_arg == argc)
        {
            return FCS_CMD_LINE_STOP;
        }
        *num_to_skip = 2;
        display_context->output_filename = (const char *)argv[next_arg];
        return FCS_CMD_LINE_SKIP;
    }
    else if (IS_ARG("--reset"))
    {
        *display_context = INITIAL_DISPLAY_CONTEXT;
        freecell_solver_user_set_iter_handler_long(instance, NULL, NULL);
        *num_to_skip = 0;
        return FCS_CMD_LINE_OK;
    }
    else
    {
        printf("Unimplemented option - \"%s\"!", arg_str);
        exit(-1);
    }
    *num_to_skip = 1;
    return FCS_CMD_LINE_SKIP;
}

#undef IS_ARG

static freecell_solver_str_t known_parameters[] = {"-h", "--help",
    "--help-configs", "--help-options", "--help-problems", "--help-real-help",
    "--help-short-sol", "--help-summary", "-i", "--iter-output", "-s",
    "--state-output", "-p", "--parseable-output", "-c",
    "--canonized-order-output", "-t", "--display-10-as-t", "-m",
    "--display-moves", "-sn", "--standard-notation", "-snx",
    "--standard-notation-extended", "-sam", "--display-states-and-moves", "-pi",
    "--display-parent-iter", "-sel", "--show-exceeded-limits", "-o", "--output",
    "--reset", "--version", NULL};

typedef enum {
    SUCCESS = 0,
    ERROR = -1,
} exit_code_t;

static inline int fc_solve_main__main(int argc, char *argv[])
{
    fc_solve_display_information_context_t display_context =
        INITIAL_DISPLAY_CONTEXT;

    int arg = 1;
    void *const instance = alloc_instance_and_parse(argc, argv, &arg,
        known_parameters, fc_solve__cmd_line_callback, &display_context, FALSE);
}
#include "default_iter_handler.h"

int main(int argc, char *argv[]) { return fc_solve_main__main(argc, argv); }
