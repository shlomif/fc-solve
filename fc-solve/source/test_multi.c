#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "fcs_user.h"

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

#define LIMIT_STEP 500
#define LIMIT_MAX 50000

int main(int argc, char * argv[])
{
    void * user;
    /* char buffer[2048]; */
    int ret;
#if 0
    fcs_move_t move;
#endif
    int board_num;
    char * buffer;
    int limit;
    int start_board, end_board, stop_at;
    char line[80];


    start_board = atoi(argv[1]);
    end_board = atoi(argv[2]);
    stop_at = atoi(argv[3]);

    /* for(board_num=1;board_num<100000;board_num++) */
    for(board_num=start_board;board_num<=end_board;board_num++)
    {
        user = freecell_solver_user_alloc();
        freecell_solver_user_set_solving_method(
            user,
            FCS_METHOD_A_STAR
            );

#if 0
        freecell_solver_user_set_solution_optimization(
            user,
            1
            );
#endif

        buffer = get_board(board_num);

        limit = LIMIT_STEP;

        freecell_solver_user_limit_iterations(user, limit);

        ret = freecell_solver_user_solve_board(user, buffer);

        printf("%i, %i\n", board_num, limit);
        fflush(stdout);

        free(buffer);

        while ((ret == FCS_STATE_SUSPEND_PROCESS) && (limit < LIMIT_MAX))
        {
            limit += LIMIT_STEP;
            freecell_solver_user_limit_iterations(user, limit);
            ret = freecell_solver_user_resume_solution(user);
            printf("%i, %i\n", board_num, limit);
            fflush(stdout);

        }

#if 0
        if (ret == FCS_STATE_WAS_SOLVED)
        {
            while (freecell_solver_user_get_next_move(user, &move) == 0)
            {
                printf("%i\n", (int)fcs_move_get_type(move)); printf("%i\n",
                (int)fcs_move_get_num_cards_in_seq(move));
            }
        }
#endif
        printf("\n");
        fflush(stdout);

        freecell_solver_user_free(user);

        if (board_num % stop_at == 0)
        {
            printf ("Press Return to continue:\n");
            fgets(line, sizeof(line), stdin);
        }
    }

    return 0;
}
