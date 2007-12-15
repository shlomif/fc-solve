/*
    make_gnome_freecell_board.c - Program to generate a GNOME Freecell
    board for input to Freecell Solver.
    
    Usage: make-gnome-freecell-board [board number] | fc-solve
    
    Note: this program uses srand() and rand() so it generates different
    boards on different systems. If you want it to generate the board
    you are playing, make sure it uses the same libc as the computer on
    which you run GNOME Freecell.
    
    Written By Eric Warmenhoven, 2000
    Modified by Shlomi Fish, 2000
    
    This program is distributed under the public domain
    (It's not copyrighted)

*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

int  *deck_shuffle();
char *num_to_card (char *, int, int, int);

int main(int argc, char **argv) {
    char output[8][30];
    char card_string[10];
    int *deck;
    int  i;
    int print_ts = 0;
    int arg;

    if (argc == 1) {
        srand(time(NULL));
    } else {
        arg = 1;
        if (!strcmp(argv[arg], "-t"))
        {
            print_ts = 1;
            arg++;
        }
        srand(atoi(argv[arg]));
    }

    deck = deck_shuffle();

    for (i = 0; i < 8; i++) {
        output[i][0] = '\0';
    }

    for (i = 0; i < 52; i++) {
        strcat(output[i % 8], num_to_card(card_string, deck[i], (i>=52-8), print_ts));
    }

    for (i = 0; i < 8; i++) {
        printf("%s\n", output[i]);
    }
    
    free(deck);

    return 0;
}

int *deck_shuffle() {
    int *deck;
    int *deck_temp;
    int  index;
    int  i;
    int  j;

    deck      = malloc(sizeof *deck * 52);
    deck_temp = malloc(sizeof *deck * 52);

    for (i = 0; i < 52; i++) {
        index = (rand ()) % (i + 1);
        for (j = i; j > index; j--) {
            deck[j] = deck[j - 1];
        }
        deck[index] = 51 - i;
    }

    for (i = 0; i < 52; i++) {
        index = (rand ()) % (i + 1);
        for (j = i; j > index; j--) {
            deck_temp[j] = deck_temp[j - 1];
        }
        deck_temp[index] = deck[51 - i];
    }

    for (i = 0; i < 52; i++) {
        deck[i] = deck_temp[51 - i];
    }
    
    free(deck_temp);

    return deck;
}

char *num_to_card(char * card, int num, int not_append_ws, int print_ts) {

    if (num % 13 == 9) {
        if (print_ts)
        {   
            sprintf(card, "T");
        }
        else
        {
            sprintf(card, "10");
        }
    } else {
        if (num % 13 == 0) {
            sprintf(card, "A");
        } else if (num % 13 < 9) {
            sprintf(card, "%d", (num % 13) + 1);
        } else if (num % 13 == 10) {
            sprintf(card, "J");
        } else if (num % 13 == 11) {
            sprintf(card, "Q");
        } else if (num % 13 == 12) {
            sprintf(card, "K");
        }
    }

    switch(num / 13) {
    case 0:
        strcat(card, "C");
        break;
    case 1:
        strcat(card, "D");
        break;
    case 2:
        strcat(card, "H");
        break;
    case 3:
        strcat(card, "S");
        break;
    }

    if (!not_append_ws)
    {
        strcat(card, " ");
    }

    return card;
}
