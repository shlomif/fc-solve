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
 * temp-fc_pro_range_solver_with_flair_support.c - the FC-Pro range 
 * solver. Solves a range of boards and displays the moves counts and 
 * the number of moves in their solution.
 *
 * This program was meant to be a temporary support in order
 * to experiment with "flairs" (working names) which are instances 
 * of the solvers that are all being ran to solve the board, one 
 * after the other and then the shortest solution is chosen to be
 * displayed.
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

#include "fcs_user.h"
#include "fcs_cl.h"

#include "fc_pro_iface_pos.h"
#define FCS_WITHOUT_GET_BOARD
#include "range_solvers_gen_ms_boards.h"
#include "unused.h"

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
}

typedef struct
{
    void * instance;
    int quota;
    char * id;
} fcs_flair_t;

struct pack_item_struct
{
    fc_solve_display_information_context_t display_context;
    int num_flairs;
    fcs_flair_t * flairs;
};

typedef struct pack_item_struct pack_item_t;


static int cmd_line_callback(
    void * instance,
    int argc GCC_UNUSED,
    const char * argv[],
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

static fcs_flair_t * next_flair(pack_item_t * user)
{
    fcs_flair_t * ret;
    user->flairs = realloc(user->flairs, ++(user->num_flairs));
    ret = &(user->flairs[user->num_flairs-1]);
    
    ret->instance = freecell_solver_user_alloc();
    ret->quota = 0;
    ret->id = NULL;

    return ret;
}

int main(int argc, char * argv[])
{
    pack_item_t user;
    fcs_flair_t * last_flair, * flair, * good_flair;

    /* char buffer[2048]; */
    int ret;
    int len;
    int board_num;
    int start_board, end_board, stop_at;
    char * buffer;
    char temp_str[10];
#ifndef WIN32
    struct timeval tv;
    struct timezone tz;
#else
    struct _timeb tb;
#endif
    int total_num_iters_temp = 0;
#ifndef WIN32
    long long total_num_iters = 0;
#else
    __int64 total_num_iters = 0;
#endif
    char * error_string;
    int parser_ret;

    int total_iterations_limit_per_board = -1;

    char * binary_output_filename = NULL;

    binary_output_t binary_output;

    int arg = 1, end_arg;

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
        if (!strcmp(argv[arg], "--binary-output-to"))
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
            total_iterations_limit_per_board = atoi(argv[arg]);
        }
        else
        {
            break;
        }
    }

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
            print_int_wrapper(total_iterations_limit_per_board);
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
            read_int_wrapper(total_iterations_limit_per_board);

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
    }

    user.num_flairs = 0;
    user.flairs = NULL;
    
    for(; arg < argc ; arg++)
    {
        last_flair = next_flair(&user);

        if (!strcmp(argv[arg++], "--flair-id"))
        {
            fprintf(stderr, "No --flair-id\n");
            exit(-1);
        }
        else if (arg == argc)
        {
            fprintf(stderr, "Reached --flair-id end");
            exit(-1);
        }
        last_flair->id = strdup(argv[arg++]);

        if (!strcmp(argv[arg++], "--flair-quota"))
        {
            fprintf(stderr, "No --flair-quota\n");
            exit(-1);
        }
        else if (arg == argc)
        {
            fprintf(stderr, "Reached --flair-quota end");
            exit(-1);
        }
        last_flair->quota = atoi(argv[arg++]);

        end_arg = arg;

        while ((end_arg < argc) && strcmp(argv[end_arg], "--next-flair"))
        {
            end_arg++;
        }

        parser_ret =
            freecell_solver_user_cmd_line_parse_args(
                last_flair->instance,
                end_arg,
                (const char * *)argv,
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
            return (-1);
        }

        arg = end_arg;
    }

    last_flair++;

    for(board_num=start_board;board_num<=end_board;board_num++)
    {
        fc_pro_get_board(board_num, &pos);

        buffer = fc_solve_fc_pro_position_to_string(&pos, 4);
#if 0
        printf("%s\n", buffer);
#endif

#define FLAIR_LOOP() \
        for(flair = user.flairs ; flair < last_flair ; flair++)

        FLAIR_LOOP()
        {
            freecell_solver_user_limit_iterations(
                    flair->instance, 
                    flair->quota
                    );
        }

        good_flair = NULL;

        FLAIR_LOOP()
        {
            ret =
                freecell_solver_user_solve_board(
                    flair->instance,
                    buffer
                );


            if (ret == FCS_STATE_SUSPEND_PROCESS)
            {
                /* 
                 * Do nothing - this board could not be solved by
                 * this flair - move on to the next.
                 * */
#if 0
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
                printf("%s", "[[Num FCS Moves]]=-1\n[[Num FCPro Moves]]=-1\n");
#endif
            }
            else if (ret == FCS_STATE_IS_NOT_SOLVEABLE)
            {
#if 0
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
                printf("%s", "[[Num FCS Moves]]=-2\n[[Num FCPro Moves]]=-2\n");
#endif
            }
            else /* The state could be solved. */
            {
                if (!good_flair)
                {
                    good_flair = flair;
                }
                else
                {
                    if (freecell_solver_user_get_moves_left(
                            flair->instance
                        )
                            < 
                        freecell_solver_user_get_moves_left(
                            good_flair->instance
                        )
                    )
                    {
                        good_flair = flair;
                    }
                }
            }
        }
        
        flair = good_flair;
        if (good_flair)
        {
            moves_processed_t * fc_pro_moves;
            fcs_extended_move_t move;

            print_int_wrapper(freecell_solver_user_get_num_times(
                        good_flair->instance));
            printf("[[Num FCS Moves]]=%d\n",
                freecell_solver_user_get_num_times(
                    good_flair->instance
                    )
            );

            fc_pro_moves = moves_processed_gen(&pos, 4, good_flair->instance);

            printf("[[Num FCPro Moves]]=%d\n[[Start]]\n",
                    moves_processed_get_moves_left(fc_pro_moves)
                  );

            len = 0;
            while (! moves_processed_get_next_move(fc_pro_moves, &move))
            {
                moves_processed_render_move(move, temp_str);
                printf("%s%c", temp_str,
                        ((((++len) % 10) == 0) ? '\n' : ' ')
                    );
            }
            moves_processed_free(fc_pro_moves);
            printf("\n%s\n", "[[End]]");
        }
        else
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
                printf("%s", "[[Num FCS Moves]]=-1\n[[Num FCPro Moves]]=-1\n");
        }

        FLAIR_LOOP()
        {
            total_num_iters_temp +=
                freecell_solver_user_get_num_times(flair->instance);
            if (total_num_iters_temp > 1000000)
            {
                total_num_iters += total_num_iters_temp;
                total_num_iters_temp = 0;
            }

            freecell_solver_user_recycle(flair->instance);
        }

        if (board_num % stop_at == 0)
        {
            total_num_iters += total_num_iters_temp;
            total_num_iters_temp = 0;

#ifndef WIN32
            gettimeofday(&tv,&tz);
            printf("Reached Board No. %i at %li.%.6li (total_num_iters=%lli)\n",
                board_num,
                tv.tv_sec,
                tv.tv_usec,
                total_num_iters
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
                total_num_iters
            );
#endif
            fflush(stdout);

        }

        free(buffer);
    }

    FLAIR_LOOP()
    {
        freecell_solver_user_free(flair->instance);
    }

    if (binary_output_filename)
    {
        fwrite(binary_output.buffer, 1, binary_output.ptr - binary_output.buffer, binary_output.file);
        fflush(binary_output.file);
        fclose(binary_output.file);
    }

    return 0;
}
