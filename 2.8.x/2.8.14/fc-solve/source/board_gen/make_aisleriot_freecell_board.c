/*
 * make_aisleriot_freecell_board.c - Program to generate a GNOME Aisleriot 
 * Freecell board for input to Freecell Solver.
 *
 * Usage: make-aisleriot-frecell-board [board number] | fc-solve
 *
 * Note: this program uses srandom() and random() so it generates different
 * boards on different systems. If you want it to generate the board
 * you are playing, make sure it uses the same libc as the computer on
 * which you run GNOME Aisleriot.
 *
 * Written by Shlomi Fish, 2000
 *
 * This code is under the public domain.
 * 
 * 
 */


#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#define make_card(value, suit ) (((value)<<8)+(suit))
#define get_suit(card) ((card)&0xFF)
#define get_value(card) ((card)>>8)

#ifdef WIN32
#define random() (rand())
#define srandom(a) (srand(a))
#endif

int deck[52];

void make_standard_deck(int start_value)
{
    int suit, value, card_num;
    card_num = 0;
    for(suit=0;suit<4;suit++)
    {
        for(value=start_value;value<=13;value++)
        {
            deck[card_num] = make_card(value,suit);
            card_num++;
        }
    }
}

void shuffle_deck(int len)
{
    int ref1, ref2;
    int temp_card;
    
    for(ref1=0 ; ref1<len ; ref1++)
    {
        ref2 = ref1 + (random()%(len-ref1));
        temp_card = deck[ref2];
        deck[ref2] = deck[ref1];
        deck[ref1] = temp_card;
    }
}

char * card_to_string(char * s, int card, int not_append_ws, int print_ts)
{
    int suit = get_suit(card);
    int v = get_value(card);

    if (v == 1)
    {
        strcpy(s, "A");
    }
    else if (v < 10)
    {
        sprintf(s, "%i", v);
    }
    else if (v == 10)
    {
        if (print_ts)
        {
            strcpy(s, "T");
        }
        else
        {
            strcpy(s, "10");
        }
    }    
    else
    {
        strcpy(s, (v == 11)?"J":((v == 12)?"Q":"K"));
    }

    switch (suit)
    {
        case 0:
            strcat(s, "C");
            break;
        case 1:
            strcat(s, "D");
            break;
        case 2:
            strcat(s, "H");
            break;
        case 3:
            strcat(s, "S");
            break;
    }

    if (!not_append_ws)
    {
        strcat(s, " ");
    }
    
    return s;
}

enum GAME_TYPES
{
    GAME_FREECELL,
    GAME_KLONDIKE,
    GAME_EIGHT_OFF,
    GAME_SEAHAVEN,
    GAME_BELEAGUERED_CASTLE,
    GAME_STREETS_AND_ALLEYS,
};

int main(int argc, char * argv[])
{
    char output[10][30], freecells[30];
    char card_string[10];
    int i, j, card_num;
    int which_game = 0;
    int print_ts = 0;
    int arg;
    
    if (argc == 1)
    {
        srandom(time(NULL));
    }
    else
    {
        arg = 1;
        if (!strcmp(argv[arg], "-t"))
        {
            print_ts = 1;
            arg++;
        }        
        srandom(atoi(argv[arg]));
        arg++;
        if (arg < argc)
        {
            if (!strcmp(argv[arg], "klondike"))
            {
                which_game = GAME_KLONDIKE;
            }
            else if (
                        (!strcmp(argv[arg], "freecell")) ||
                        (!strcmp(argv[arg], "bakers_game"))
                    )
            {
                which_game = GAME_FREECELL;
            }
            else if (
                        (!strcmp(argv[arg], "eight_off"))
                    )
            {
                which_game = GAME_EIGHT_OFF;
            }
            else if (
                        (!strcmp(argv[arg], "seahaven"))
                    )
            {
                which_game = GAME_SEAHAVEN;
            }
            else if (
                        (!strcmp(argv[arg], "beleaguered_castle"))
                    )
            {
                which_game = GAME_BELEAGUERED_CASTLE;
            }
            else if (
                        (!strcmp(argv[arg], "streets_and_alleys"))
                    )
            {
                which_game = GAME_STREETS_AND_ALLEYS;
            }
            else
            {
                fprintf(stderr, "Unknown game type \"%s\"!\n", argv[2]);
                return -1;
            }            
        }
    }

    make_standard_deck((which_game == GAME_BELEAGUERED_CASTLE) ? 2 : 1);
    shuffle_deck((which_game == GAME_BELEAGUERED_CASTLE) ? 48 : 52);

    if (which_game == GAME_KLONDIKE)
    {
        for(i = 0 ; i < 7 ; i++)
        {
            output[i][0] = '\0';
        }
        card_num = 0;
        for(i = 0 ; i < 7 ; i++)
        {
            for(j = i ; j < 7 ; j++)
            {
                strcat(output[j], card_to_string(card_string, deck[card_num], (j == i), print_ts));
                card_num++;                            
            }
        }

        printf("%s", "Talon: ");
        for( ; card_num < 52; card_num++)
        {
            printf("%s", card_to_string( card_string, deck[card_num], (card_num == 52-1), print_ts));
        }
        printf("\n");

        for (i = 0; i < 7; i++) 
        {
            printf("%s\n", output[i]);
        }
        
    }
    else if (which_game == GAME_FREECELL)
    {
        /* Freecell */
        for (i = 0; i < 8; i++) 
        {
            output[i][0] = '\0';
        }

        for (i = 0; i < 52; i++) 
        {
            strcat(output[i % 8], card_to_string( card_string, deck[i], (i>=52-8), print_ts));
        }

        for (i = 0; i < 8; i++) 
        {
            printf("%s\n", output[i]);
        }
    }
    else if (which_game == GAME_EIGHT_OFF)
    {
        /* Freecell */
        for (i = 0; i < 8; i++) 
        {
            output[i][0] = '\0';
        }

        for (i = 0; i < 48; i++) 
        {
            strcat(output[i % 8], card_to_string( card_string, deck[i], (i>=48-8), print_ts));
        }
        
        freecells[0] = '\0';
        for ( ; i < 52 ; i++)
        {
            strcat(freecells, card_to_string( card_string, deck[i], (i == 52-1), print_ts));
        }

        printf("FC: %s\n", freecells);

        for (i = 0; i < 8; i++) 
        {
            printf("%s\n", output[i]);
        }
    }
    else if (which_game == GAME_SEAHAVEN)
    {
        /* Seahaven Towers */
        for( i = 0 ; i < 10 ; i++)
        {
            output[i][0] = '\0';
        }

        for( i = 0 ; i < 50 ; i++)
        {
            strcat(output[i % 10], card_to_string(card_string, deck[i], (i>=50-10), print_ts));
        }
        strcpy(freecells, "FC: - ");

        for ( ; i < 52 ; i++)
        {
            strcat(freecells, card_to_string(card_string, deck[i], (i == 52-1), print_ts));
        }
        
        printf("%s\n", freecells);

        for( i = 0 ; i < 10 ; i++)
        {
            printf("%s\n", output[i]);
        }
    }
    else if (which_game == GAME_BELEAGUERED_CASTLE)
    {
        for (i = 0; i < 8; i++)
        {
            output[i][0] = '\0';
        }
#define index_prog_to_pysol(i) ((((i)%2) == 1)*4 + ((i/2)%4))
        for (i = 0; i < 48; i++) 
        {
            strcat(output[index_prog_to_pysol(i % 8)], card_to_string( card_string, deck[i], (i>=48-8), print_ts));
        }
#undef index_prog_to_pysol
        printf("Foundations: H-A C-A D-A S-A\n");

        for (i = 0; i < 8; i++)
        {
            printf("%s\n", output[i]);
        }
    }
    else if (which_game == GAME_STREETS_AND_ALLEYS)
    {
        for (i = 0; i < 8; i++)
        {
            output[i][0] = '\0';
        }
#define index_prog_to_pysol(i) ((((i)%2) == 1)*4 + ((i/2)%4))
        for (i = 0; i < 48; i++) 
        {
            strcat(output[index_prog_to_pysol(i % 8)], card_to_string( card_string, deck[i], (i>=48-8), print_ts));
        }
#undef index_prog_to_pysol
        for(;i<52;i++)
        {
            strcat(output[(i%8)], " ");
            strcat(output[(i%8)], card_to_string(card_string, deck[i], 1, print_ts));
        }
        
        for (i = 0; i < 8; i++)
        {
            printf("%s\n", output[i]);
        }        
    }
    return 0;
}

    
 
