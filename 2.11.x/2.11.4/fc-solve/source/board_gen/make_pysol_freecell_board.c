/*
 * make_pysol_freecell_board.c - Program to generate a PySol Freecell board
 * for input to Freecell Solver.
 *
 * Usage: make-pysol-freecell-board [board number] | fc-solve
 *
 * Note: You should probably use the Python script (i.e:
 * make_pysol_freecell_board.py) instead of this program, because this program
 * doesn't handle integers above 2^63 well, nor is the 64-bit arithmetics
 * very portable.
 *
 * Another Note: Boards 1 through 32000 are the same as Microsoft Freecell's.
 * 
 * Written by Shlomi Fish, 2000
 *
 * This code is under the public domain.
 * 
 * 
 */

#include <stdio.h>
#include <string.h>
#include <time.h>

#define NUM_CARDS 52
#define NUM_STACKS 8

#ifdef _MSC_VER
typedef __int64 integer64;
#define INT64_CONSTANT(c) (c##i64)
#elif defined(__GNUC__)
typedef __int64_t integer64;
#define INT64_CONSTANT(c) (c##ll)
#endif

typedef int card_t;

#define make_card(suit, value ) (((value)<<8)+(suit))
#define get_suit(card) ((card)&0xFF)
#define get_value(card) ((card)>>8)


struct struct_pysol_random_context
{
    integer64 seed;
    double (*random)(struct struct_pysol_random_context *); 
    int (*randint)(struct struct_pysol_random_context *, int a, int b);
};

typedef struct struct_pysol_random_context pysol_random_context;

int pysol_random64_randint(pysol_random_context * context, int a, int b)
{
    return (a + (int)(context->random(context) * (b+1-a)));
}


double pysol_random64(pysol_random_context * context)
{
    context->seed = (context->seed*INT64_CONSTANT(6364136223846793005) + INT64_CONSTANT(1)) & INT64_CONSTANT(0xffffffffffffffff);
    return ((context->seed >> 21) & INT64_CONSTANT(0x7fffffff)) / 2147483648.0;
}

double pysol_random31(pysol_random_context * context)
{
    context->seed = (context->seed*INT64_CONSTANT(214013) + INT64_CONSTANT(2531011)) &  INT64_CONSTANT(0x7fffffff);
    return (context->seed >> 16) / 32768.0;
}

int pysol_random31_randint(pysol_random_context * context, int a, int b)
{
    context->seed = (context->seed*INT64_CONSTANT(214013) + INT64_CONSTANT(2531011)) & INT64_CONSTANT(0x7fffffff);
    return a + (((int)(context->seed >> 16)) % (b+1-a));
}


void pysol_shuffle(pysol_random_context * context, card_t * seq, int len)
{
    int j;
    card_t temp;
    int n;
    
    n = len-1;    
    while (n > 0)
    {
    	j = context->randint(context,0,n);
    	temp = seq[n];
    	seq[n] = seq[j];
    	seq[j] = temp;
    	n--;
    }
}

void pysol_create_cards(card_t * cards)
{
    int id, suit, rank;
    
    id = 0;
    for (suit=0;suit<4;suit++)
    {
    	for(rank=0;rank<13;rank++)
    	{
    	    cards[id] = make_card(suit,rank+1);
            id++;
    	}
    }
}

char * card_to_string(char * s, int card, int not_append_ws)
{
    int suit = get_suit(card);
    int v = get_value(card);

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
        case 0:
            strcat(s, "S");
            break;
        case 1:
            strcat(s, "C");
            break;
        case 2:
            strcat(s, "H");
            break;
        case 3:
            strcat(s, "D");
            break;
    }

    if (!not_append_ws)
    {
        strcat(s, " ");
    }
    
    return s;
}


int main(int argc, char * argv[])
{
    integer64 game_num;
    card_t cards[NUM_CARDS], orig_cards[NUM_CARDS];
    pysol_random_context r;
    char output[NUM_STACKS][30];
    char card_string[10];
    int i;

    if (argc == 1)
    {
    	game_num = time(NULL);
    }
    else
    {
    	char * s;

    	game_num = 0;
    	s = argv[1];
    	while (((*s) >= '0') && ((*s) <= '9'))
    	{
    	    game_num = game_num*10+(*s-'0');
    	    s++;
    	}
    }

    pysol_create_cards(orig_cards);
    
    if (game_num <= 32000)
    {
    	int fcards[NUM_CARDS];
    	int j_pos[4] = {0,39,26,13};
    	int pos,j;

    	r.seed = game_num;
    	r.random = pysol_random31;
    	r.randint = pysol_random31_randint;

    	pos = 0;
    	for(i=0;i<13;i++)
    	{
    	    for(j=0;j<4;j++)
    	    {
        		fcards[pos] = orig_cards[i+j_pos[j]];
        		pos++;
    	    }
    	}
    	for(i=0;i<NUM_CARDS;i++)
    	{
    	    orig_cards[i] = fcards[i];
    	}
    	pysol_shuffle(&r, orig_cards, NUM_CARDS);
    }
    else
    {
    	r.seed = game_num;
    	r.random = pysol_random64;
    	r.randint = pysol_random64_randint;

    	pysol_shuffle(&r, orig_cards, NUM_CARDS);
    }

    for(i=0;i<NUM_CARDS;i++)
    {
    	cards[i] = orig_cards[NUM_CARDS-i-1];
    }
    
    for (i = 0; i < NUM_STACKS; i++) {
        output[i][0] = '\0';
    }

    for (i = 0; i < NUM_CARDS; i++) {
        strcat(output[i % NUM_STACKS], 
            card_to_string(card_string, 
                cards[i], 
                (i>=NUM_CARDS-NUM_STACKS)
            )
        );
    }

    for (i = 0; i < NUM_STACKS; i++) {
        printf("%s\n", output[i]);
    }

    return 0;
}
