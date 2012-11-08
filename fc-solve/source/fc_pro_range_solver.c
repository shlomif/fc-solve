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

#include "portable_time.h"
#include "portable_int64.h"

#include "fcs_user.h"
#include "fcs_cl.h"

#include "fc_pro_iface_pos.h"
#define FCS_WITHOUT_GET_BOARD
#include "range_solvers_gen_ms_boards.h"
#include "unused.h"
#include "bool.h"

#define     SUIT(card)      ((card) % 4)
#define     VALUE(card)     ((card) / 4)

static GCC_INLINE void fc_pro_get_board(long gamenumber, Position * pos)
{
    int  i, j;                /*  generic counters */
    int  wLeft = 52;          /*  cards left to be chosen in shuffle */
    CARD deck[52];            /* deck of 52 unique cards */
    int col;
    CARD card, suit;

    memset(pos, '\0', sizeof(*pos));

    /* shuffle cards */

    for (i = 0; i < 52; i++)      /* put unique card in each deck loc. */
    {
        deck[i] = i;
    }

    for (i = 0; i < 52; i++)
    {
        j = microsoft_rand_rand(&gamenumber) % wLeft;
        col = (i%8);
        card = deck[j];
        suit = SUIT(card);
        pos->tableau[col].cards[pos->tableau[col].count++]
            = (Card)((VALUE(card)+1)
            + (((suit == 3) ? suit : ((suit+1)%3))<<4))
            ;
        deck[j] = deck[--wLeft];
    }
}

struct fc_solve_display_information_context_struct
{
    fcs_bool_t debug_iter_state_output;
    int freecells_num;
    int stacks_num;
    int decks_num;
    fcs_bool_t parseable_output;
    fcs_bool_t canonized_order_output;
    fcs_bool_t display_10_as_t;
    fcs_bool_t display_parent_iter_num;
    fcs_bool_t debug_iter_output_on;
    fcs_bool_t display_moves;
    fcs_bool_t display_states;
    fcs_bool_t standard_notation;
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
    else if ((!strcmp(argv[arg], "-sn")) || (!strcmp(argv[arg], "--standard-notation")))
    {
        dc->standard_notation = TRUE;
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

#if 0
#define total_iterations_limit_per_board 100000
#endif

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

int main(int argc, char * argv[])
{
    pack_item_t user;
    /* char buffer[2048]; */
    int ret;
    int len;
    int board_num;
    int start_board, end_board, stop_at;
    char * buffer;
    const char * variant = "freecell";
    char temp_str[10];
    fcs_portable_time_t mytime;

    fcs_int64_t total_num_iters = 0;

    char * error_string;
    int parser_ret;
    fcs_bool_t variant_is_freecell;

    fcs_int_limit_t total_iterations_limit_per_board = -1;

    char * binary_output_filename = NULL;

    binary_output_t binary_output;

    int arg = 1, start_from_arg;

    Position pos;

    if (argc < 4)
    {
        fprintf(stderr, "Not Enough Arguments!\n");
        print_help();
        exit(-1);
    }
    start_board = atoi(argv[arg++]);
    end_board = atoi(argv[arg++]);
    stop_at = atoi(argv[arg++]);

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
        else if (!strcmp(argv[arg], "--binary-output-to"))
        {
            arg++;
            if (arg == argc)
            {
                fprintf(stderr, "--binary-output-to came without an argument!\n");
                print_help();
                exit(-1);
            }
            binary_output_filename = argv[arg];
        }
        else if (!strcmp(argv[arg], "--total-iterations-limit"))
        {
            arg++;
            if (arg == argc)
            {
                fprintf(stderr, "--total-iterations-limit came without an argument!\n");
                print_help();
                exit(-1);
            }
            total_iterations_limit_per_board = atol(argv[arg]);
        }
        else
        {
            break;
        }
    }

    start_from_arg = arg;



    FCS_PRINT_STARTED_AT(mytime);
    fflush(stdout);

    if (binary_output_filename)
    {
        FILE * in;

        binary_output.buffer = malloc(sizeof(int) * BINARY_OUTPUT_NUM_INTS);
        binary_output.ptr = binary_output.buffer;
        binary_output.buffer_end = binary_output.buffer + sizeof(int)*BINARY_OUTPUT_NUM_INTS;


        in = fopen(binary_output_filename, "rb");
        if (in == NULL)
        {
            binary_output.file = fopen(binary_output_filename, "wb");
            if (! binary_output.file)
            {
                fprintf(stderr, "Could not open \"%s\" for writing!\n", binary_output_filename);
                exit(-1);
            }

            print_int_wrapper(start_board);
            print_int_wrapper(end_board);
            print_int_wrapper(((int)total_iterations_limit_per_board));
        }
        else
        {
            long file_len;
#define read_int_wrapper(var) \
            {       \
                if (read_int(in, &var))  \
                {        \
                    fprintf(stderr, "%s", \
                        "Output file is too short to deduce the configuration!\n"    \
                           );     \
                    exit(-1);       \
                }       \
            }
            read_int_wrapper(start_board);
            read_int_wrapper(end_board);
            {
                int val;
                read_int_wrapper(val);
                total_iterations_limit_per_board = (fcs_int_limit_t)val;
            }

            fseek(in, 0, SEEK_END);
            file_len = ftell(in);
            if (file_len % 4 != 0)
            {
                fprintf(stderr, "%s", "Output file has an invalid length. Terminating.\n");
                exit(-1);
            }
            start_board += (file_len-12)/4;
            if (start_board >= end_board)
            {
                fprintf(stderr, "%s", "Output file was already finished being generated.\n");
                exit(-1);
            }
            fclose(in);
            binary_output.file = fopen(binary_output_filename, "ab");
            if (! binary_output.file)
            {
                fprintf(stderr, "Could not open \"%s\" for writing!\n", binary_output_filename);
                exit(-1);
            }
        }
    }
    else
    {
        binary_output.file = NULL;
        binary_output.buffer
            = binary_output.ptr
            = binary_output.buffer_end
            = NULL;
    }

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

    for(board_num=start_board;board_num<=end_board;board_num++)
    {
        if (variant_is_freecell)
        {
            fc_pro_get_board(board_num, &pos);

            buffer = fc_solve_fc_pro_position_to_string(&pos, 4);
        }
        else
        {
            FILE * from_make_pysol;
            char command[1000];
#define BUF_SIZE 2000
            buffer = calloc(BUF_SIZE, sizeof(buffer[0]));

            sprintf(command, "make_pysol_freecell_board.py -F -t %d %s",
                    board_num,
                    variant
                   );

            from_make_pysol = popen(command, "r");
            fread(buffer, sizeof(buffer[0]), BUF_SIZE-1, from_make_pysol);
            pclose(from_make_pysol);
#undef BUF_SIZE
        }
#if 0
        printf("%s\n", buffer);
#endif

        freecell_solver_user_limit_iterations_long(user.instance, total_iterations_limit_per_board);

        ret =
            freecell_solver_user_solve_board(
                user.instance,
                buffer
                );

        free(buffer);

        int num_iters;
        int num_moves;
        int num_fcpro_moves;
        moves_processed_t * fc_pro_moves = NULL;

        if (ret == FCS_STATE_SUSPEND_PROCESS)
        {
            FCS_PRINT_INTRACTABLE_BOARD(mytime, board_num);

            num_iters = num_moves = num_fcpro_moves = -1;
        }
        else if (ret == FCS_STATE_IS_NOT_SOLVEABLE)
        {
            FCS_PRINT_UNSOLVED_BOARD(mytime, board_num);
            num_iters = num_moves = num_fcpro_moves = -1;
        }
        else
        {
            num_iters = (int)freecell_solver_user_get_num_times_long(user.instance);
            num_moves = freecell_solver_user_get_moves_left(user.instance);

            if (variant_is_freecell)
            {
                fc_pro_moves = moves_processed_gen(&pos, 4, user.instance);

                num_fcpro_moves = moves_processed_get_moves_left(fc_pro_moves);
            }
            else
            {
                num_fcpro_moves = num_moves;
            }
        }
        print_int_wrapper(num_iters);
        printf("[[Num Iters]]=%d\n[[Num FCS Moves]]=%d\n[[Num FCPro Moves]]=%d\n", num_iters, num_moves, num_fcpro_moves);

        printf("\n%s\n", "[[Start]]");
        if (fc_pro_moves)
        {
            fcs_extended_move_t move;
            len = 0;
            while (! moves_processed_get_next_move(fc_pro_moves, &move))
            {
                moves_processed_render_move(move, temp_str);
                printf("%s%c", temp_str,
                    ((((++len) % 10) == 0) ? '\n' : ' ')
                );
            }
            moves_processed_free(fc_pro_moves);
        }
        printf("\n%s\n", "[[End]]");
        fflush(stdout);
        total_num_iters += freecell_solver_user_get_num_times_long(user.instance);

        if (board_num % stop_at == 0)
        {
            FCS_PRINT_REACHED_BOARD(
                mytime,
                board_num,
                total_num_iters
            );
            fflush(stdout);

        }

        freecell_solver_user_recycle(user.instance);
    }

    freecell_solver_user_free(user.instance);

    if (binary_output_filename)
    {
        fwrite(binary_output.buffer, 1, binary_output.ptr - binary_output.buffer, binary_output.file);
        fflush(binary_output.file);
        fclose(binary_output.file);
    }

    return 0;
}
