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

#ifdef DMALLOC
#include "dmalloc.h"
#endif


struct microsoft_rand_struct
{
    long seed;
};

typedef struct microsoft_rand_struct microsoft_rand_t;

microsoft_rand_t * microsoft_rand_alloc(unsigned int seed)
{
    microsoft_rand_t * ret;

    ret = malloc(sizeof(microsoft_rand_t));
    ret->seed = (long)seed;

    return ret;
}

void microsoft_rand_free(microsoft_rand_t * rand)
{
    free(rand);
}

int microsoft_rand_rand(microsoft_rand_t * rand)
{
    rand->seed = (rand->seed * 214013 + 2531011);
    return (rand->seed >> 16) & 0x7fff;
}

typedef int CARD;

#define     BLACK           0               /* COLOUR(card) */
#define     RED             1

#define     ACE             0               /*  VALUE(card) */
#define     DEUCE           1

#define     CLUB            0               /*  SUIT(card)  */
#define     DIAMOND         1
#define     HEART           2
#define     SPADE           3

#define     SUIT(card)      ((card) % 4)
#define     VALUE(card)     ((card) / 4)
#define     COLOUR(card)    (SUIT(card) == DIAMOND || SUIT(card) == HEART)

#define     MAXPOS         21
#define     MAXCOL          9    /* includes top row as column 0 */

char * card_to_string(char * s, CARD card, int not_append_ws)
{
    int suit = SUIT(card);
    int v = VALUE(card)+1;

    if (v == 1)
    {
        strcpy(s, "A");
    }
    else if (v <= 10)
    {
        sprintf(s, "%i", v);
    }
    else
    {
        strcpy(s, (v == 11)?"J":((v == 12)?"Q":"K"));
    }

    switch (suit)
    {
        case CLUB:
            strcat(s, "C");
            break;
        case DIAMOND:
            strcat(s, "D");
            break;
        case HEART:
            strcat(s, "H");
            break;
        case SPADE:
            strcat(s, "S");
            break;
    }
    if (!not_append_ws)
    {
        strcat(s, " ");
    }


    return s;
}

char * get_board(int gamenumber)
{

    CARD    card[MAXCOL][MAXPOS];    /* current layout of cards, CARDs are ints */

    int  i, j;                /*  generic counters */
    int  wLeft = 52;          /*  cards left to be chosen in shuffle */
    CARD deck[52];            /* deck of 52 unique cards */
    char * ret;

    microsoft_rand_t * randomizer;


    ret = malloc(1024);
    ret[0] = '\0';

    /* shuffle cards */

    for (i = 0; i < 52; i++)      /* put unique card in each deck loc. */
    {
        deck[i] = i;
    }

    randomizer = microsoft_rand_alloc(gamenumber);            /* gamenumber is seed for rand() */
    for (i = 0; i < 52; i++)
    {
        j = microsoft_rand_rand(randomizer) % wLeft;
        card[(i%8)+1][i/8] = deck[j];
        deck[j] = deck[--wLeft];
    }

    microsoft_rand_free(randomizer);


    {
        int stack;
        int c;

        char card_string[10];

        for(stack=1 ; stack<9 ; stack++ )
        {
            for(c=0 ; c < (6+(stack<5)) ; c++)
            {
                sprintf(ret+strlen(ret), "%s",
                    card_to_string(
                        card_string,
                        card[stack][c],
                        (c == (6+(stack<5)))
                    )
                );
            }
            sprintf(ret+strlen(ret), "%s", "\n");
        }
    }

    return ret;
}

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
    freecell_solver_display_information_context_t display_context;
    void * instance;
};

typedef struct pack_item_struct pack_item_t;


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
    pack_item_t * item;
    freecell_solver_display_information_context_t * dc;
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
#if 0
#define total_iterations_limit_per_board 100000
#endif

static char * known_parameters[] = {
    "-i", "--iter-output",
    "-s", "--state-output",
    "-p", "--parseable-output",
    "-t", "--display-10-as-t",
    "-pi", "--display-parent-iter",
    NULL
    };

void print_int(FILE * f, int val)
{
    unsigned char buffer[4];
    int p;
    if (f == NULL)
    {
        return;
    }
            
    for(p=0;p<4;p++)
    {
        buffer[p] = (val & 0xFF);
        val >>= 8;
    }
    fwrite(buffer, 1, 4, f);
}

int main(int argc, char * argv[])
{
    pack_item_t user;
    /* char buffer[2048]; */
    int ret;
    int board_num;
    char * buffer;
    int start_board, end_board, stop_at;
#ifndef WIN32
    struct timeval tv;
    struct timezone tz;
#else
    struct _timeb tb;
#endif
    int board_num_iters;
    int total_num_iters_temp = 0;
#ifndef WIN32
    long long total_num_iters = 0;
#else
    __int64 total_num_iters = 0;
#endif
    char * error_string;
    int parser_ret;

    int pack_num_iters;
    int total_iterations_limit_per_board = 100000;

    char * binary_output_filename = NULL;
    FILE * binary_output = NULL;


    int arg = 1, start_from_arg;
    if (argc < 5)
    {
        fprintf(stderr, "Not Enough Arguments!\n");
        exit(-1);
    }
    start_board = atoi(argv[arg++]);
    end_board = atoi(argv[arg++]);
    stop_at = atoi(argv[arg++]);

    if (!strcmp(argv[arg], "--total-iterations-limit"))
    {
        arg++;
        total_iterations_limit_per_board = atoi(argv[arg++]);
    }

    if (!strcmp(argv[arg], "--binary-output-to"))
    {
        arg++;
        binary_output_filename = argv[arg++];
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
    printf("Started at %li.%.6li\n",
        tb.time,
        tb.millitm*1000
        );
#endif
    fflush(stdout);

    if (binary_output_filename)
    {
        binary_output = fopen(binary_output_filename, "wb");
        if (! binary_output)
        {
            fprintf(stderr, "Could not open \"%s\" for writing!\n", binary_output_filename);
            exit(-1);
        }
        print_int(binary_output, start_board);
        print_int(binary_output, end_board);
        print_int(binary_output, total_iterations_limit_per_board);
        fflush(binary_output);
    }

    for(board_num=start_board;board_num<=end_board;board_num++)
    {
        buffer = get_board(board_num);

        board_num_iters = 0;

        pack_num_iters = 0;

        arg = start_from_arg;
        user.instance = freecell_solver_user_alloc();
        
        parser_ret =
            freecell_solver_user_cmd_line_parse_args(
                user.instance,
                argc,
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
            exit(-1);
        }

        ret = 0;

        freecell_solver_user_limit_iterations(user.instance, total_iterations_limit_per_board);

        ret = 
            freecell_solver_user_solve_board(
                user.instance,
                buffer
                );

        free(buffer);


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
            printf("Intractable Board No. %i at %li.%.6li\n",
                board_num,
                tb.time,
                tb.millitm*1000
            );
#endif
            fflush(stdout);
            print_int(binary_output, -1);
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
            printf("Unsolved Board No. %i at %li.%.6li\n",
                board_num,
                tb.time,
                tb.millitm*1000
            );
#endif
            print_int(binary_output, -2);
        }
        else
        {
            print_int(binary_output, freecell_solver_user_get_num_times(user.instance));
        }

        total_num_iters_temp += freecell_solver_user_get_num_times(user.instance);
        if (total_num_iters_temp > 1000000)
        {
            total_num_iters += total_num_iters_temp;
            total_num_iters_temp = 0;
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
            printf("Reached Board No. %i at %li.%.6li (total_num_iters=%I64i)\n",
                board_num,
                tb.time,
                tb.millitm*1000,
                total_num_iters
            );
#endif
            fflush(stdout);

            if (binary_output_filename)
            {
                fflush(binary_output);
            }
        }
        

        freecell_solver_user_free(user.instance);
    }

    if (binary_output_filename)
    {
        fclose(binary_output);
    }


    return 0;
}
