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
 * state.c - state functions module for Freecell Solver
 *
 */

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "config.h"
#include "state.h"
#include "card.h"
#include "fcs_enums.h"
#include "app_str.h"
#include "unused.h"
#include "inline.h"

#ifndef min
#define min(a,b) ((a)<(b)?(a):(b))
#endif

static int fcs_card_compare(const void * card1, const void * card2)
{
    const fcs_card_t * c1 = (const fcs_card_t *)card1;
    const fcs_card_t * c2 = (const fcs_card_t *)card2;

    if (fcs_card_card_num(*c1) > fcs_card_card_num(*c2))
    {
        return 1;
    }
    else if (fcs_card_card_num(*c1) < fcs_card_card_num(*c2))
    {
        return -1;
    }
    else
    {
        if (fcs_card_suit(*c1) > fcs_card_suit(*c2))
        {
            return 1;
        }
        else if (fcs_card_suit(*c1) < fcs_card_suit(*c2))
        {
            return -1;
        }
        else
        {
            return 0;
        }
    }
}

#ifdef DEBUG_STATES
static int fcs_stack_compare(const void * s1, const void * s2)
{
    fcs_card_t card1 = ((const fc_stack_t *)s1)->cards[0];
    fcs_card_t card2 = ((const fc_stack_t *)s2)->cards[0];

    return fcs_card_compare(&card1, &card2);
}
#elif defined(COMPACT_STATES)
static int fcs_stack_compare(const void * s1, const void * s2)
{
    fcs_card_t card1 = ((fcs_card_t*)s1)[1];
    fcs_card_t card2 = ((fcs_card_t*)s2)[1];

    return fcs_card_compare(&card1, &card2);
}
#elif defined(INDIRECT_STACK_STATES)

int fc_solve_stack_compare_for_comparison(const void * v_s1, const void * v_s2)
{
    const fcs_card_t * s1 = (const fcs_card_t *)v_s1;
    const fcs_card_t * s2 = (const fcs_card_t *)v_s2;

    int min_len;
    int a, ret;

    min_len = min(s1[0], s2[0]);

    for(a=0;a<min_len;a++)
    {
        ret = fcs_card_compare(s1+a+1,s2+a+1);
        if (ret != 0)
        {
            return ret;
        }
    }
    /*
     * The reason I do the stack length comparisons after the card-by-card
     * comparison is to maintain correspondence with
     * fcs_stack_compare_for_stack_sort, and with the one card comparison
     * of the other state representation mechanisms.
     * */
    if (s1[0] < s2[0])
    {
        return -1;
    }
    else if (s1[0] > s2[0])
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

#endif

#ifdef FCS_WITH_TALONS
static int fcs_talon_compare_with_context(const void * p1, const void * p2, fcs_compare_context_t context)
{
    fcs_card_t * t1 = (fcs_card_t *)p1;
    fcs_card_t * t2 = (fcs_card_t *)p2;

    if (t1[0] < t2[0])
    {
        return -1;
    }
    else if (t1[0] > t2[0])
    {
        return 1;
    }
    else
    {
        return memcmp(t1,t2,t1[0]+1);
    }
}
#endif

#ifdef DEBUG_STATES
void fc_solve_canonize_state(
    fcs_state_extra_info_t * state_val,
    int freecells_num,
    int stacks_num)
{
    int b,c;

    fc_stack_t temp_stack;
    fcs_card_t temp_freecell;
    int temp_loc;

    fcs_state_t * state_key = state_val->key;

    /* Insertion-sort the stacks */
    for(b=1;b<stacks_num;b++)
    {
        c = b;
        while(
            (c>0) &&
            (fcs_stack_compare(
                &(state_key->stacks[c]),
                &(state_key->stacks[c-1])
                ) < 0)
            )
        {
            temp_stack = state_key->stacks[c];
            state_key->stacks[c] = state_key->stacks[c-1];
            state_key->stacks[c-1] = temp_stack;

            temp_loc = state_val->stack_locs[c];
            state_val->stack_locs[c] = state_val->stack_locs[c-1];
            state_val->stack_locs[c-1] = temp_loc;

            c--;
        }
    }

    /* Insertion sort the freecells */

    for(b=1;b<freecells_num;b++)
    {
        c = b;
        while(
            (c>0)     &&
            (fcs_card_compare(
                &(state_key->freecells[c]),
                &(state_key->freecells[c-1])
                ) < 0)
            )
        {
            temp_freecell = state_key->freecells[c];
            state_key->freecells[c] = state_key->freecells[c-1];
            state_key->freecells[c-1] = temp_freecell;

            temp_loc = state_val->fc_locs[c];
            state_val->fc_locs[c] = state_val->fc_locs[c-1];
            state_val->fc_locs[c-1] = temp_loc;

            c--;
        }
    }
}

#elif defined(COMPACT_STATES)

void fc_solve_canonize_state(
    fcs_state_extra_info_t * state_val,
    int freecells_num,
    int stacks_num)
{
    int b,c;

    char temp_stack[(MAX_NUM_CARDS_IN_A_STACK+1)];
    fcs_card_t temp_freecell;
    char temp_loc;

    fcs_state_t * state_key = state_val->key;

    /* Insertion-sort the stacks */

    for(b=1;b<stacks_num;b++)
    {
        c = b;
        while(
            (c>0)    &&
            (fcs_stack_compare(
                state_key->data+c*(MAX_NUM_CARDS_IN_A_STACK+1),
                state_key->data+(c-1)*(MAX_NUM_CARDS_IN_A_STACK+1)
                ) < 0)
            )
        {
            memcpy(temp_stack, state_key->data+c*(MAX_NUM_CARDS_IN_A_STACK+1), (MAX_NUM_CARDS_IN_A_STACK+1));
            memcpy(state_key->data+c*(MAX_NUM_CARDS_IN_A_STACK+1), state_key->data+(c-1)*(MAX_NUM_CARDS_IN_A_STACK+1), (MAX_NUM_CARDS_IN_A_STACK+1));
            memcpy(state_key->data+(c-1)*(MAX_NUM_CARDS_IN_A_STACK+1), temp_stack, (MAX_NUM_CARDS_IN_A_STACK+1));

            temp_loc = state_val->stack_locs[c];
            state_val->stack_locs[c] = state_val->stack_locs[c-1];
            state_val->stack_locs[c-1] = temp_loc;

            c--;
        }
    }

    /* Insertion-sort the freecells */

    for(b=1;b<freecells_num;b++)
    {
        c = b;

        while(
            (c>0)    &&
            (fcs_card_compare(
                state_key->data+FCS_FREECELLS_OFFSET+c,
                state_key->data+FCS_FREECELLS_OFFSET+c-1
                ) < 0)
            )
        {
            temp_freecell = (state_key->data[FCS_FREECELLS_OFFSET+c]);
            state_key->data[FCS_FREECELLS_OFFSET+c] = state_key->data[FCS_FREECELLS_OFFSET+c-1];
            state_key->data[FCS_FREECELLS_OFFSET+c-1] = temp_freecell;

            temp_loc = state_val->fc_locs[c];
            state_val->fc_locs[c] = state_val->fc_locs[c-1];
            state_val->fc_locs[c-1] = temp_loc;

            c--;
        }
    }
}
#elif defined(INDIRECT_STACK_STATES)
void fc_solve_canonize_state(
    fcs_state_extra_info_t * state_val,
    int freecells_num,
    int stacks_num)
{
    int b,c;
    fcs_card_t * temp_stack;
    fcs_card_t temp_freecell;
    char temp_loc;

    fcs_state_t * state_key = state_val->key;

    /* Insertion-sort the stacks */
    for(b=1;b<stacks_num;b++)
    {
        c = b;
        while(
            (c>0) &&
            (
                fc_solve_stack_compare_for_comparison
                (
                    (state_key->stacks[c]),
                    (state_key->stacks[c-1])
                )
                < 0
            )
        )
        {
            temp_stack = state_key->stacks[c];
            state_key->stacks[c] = state_key->stacks[c-1];
            state_key->stacks[c-1] = temp_stack;

            temp_loc = state_val->stack_locs[c];
            state_val->stack_locs[c] = state_val->stack_locs[c-1];
            state_val->stack_locs[c-1] = temp_loc;

            c--;
        }
    }

    /* Insertion sort the freecells */

    for(b=1;b<freecells_num;b++)
    {
        c = b;
        while(
            (c>0)     &&
            (fcs_card_compare(
                &(state_key->freecells[c]),
                &(state_key->freecells[c-1])
                ) < 0)
            )
        {
            temp_freecell = state_key->freecells[c];
            state_key->freecells[c] = state_key->freecells[c-1];
            state_key->freecells[c-1] = temp_freecell;

            temp_loc = state_val->fc_locs[c];
            state_val->fc_locs[c] = state_val->fc_locs[c-1];
            state_val->fc_locs[c-1] = temp_loc;

            c--;
        }
    }
}

#endif

static GCC_INLINE void fcs_state_init(
    fcs_state_t * state_key,
    fcs_state_extra_info_t * state_val,
    int stacks_num
#ifdef INDIRECT_STACK_STATES
    ,char * indirect_stacks_buffer
#endif
    )
{
    int i;

    memset(state_key, 0, sizeof(*state_key));

    for(i=0;i<MAX_NUM_STACKS;i++)
    {
        state_val->stack_locs[i] = (char)i;
    }
#ifdef INDIRECT_STACK_STATES
    for(i=0;i<stacks_num;i++)
    {
        state_key->stacks[i] = &indirect_stacks_buffer[i << 7];
        memset(state_key->stacks[i], '\0', MAX_NUM_DECKS*52+1);
    }
    for(;i<MAX_NUM_STACKS;i++)
    {
        state_key->stacks[i] = NULL;
    }
#endif
    for(i=0;i<MAX_NUM_FREECELLS;i++)
    {
        state_val->fc_locs[i] = (char)i;
    }
    state_val->key = state_key;
    state_val->parent_val = NULL;
    state_val->moves_to_parent = NULL;
    state_val->depth = 0;
    state_val->visited = 0;
    state_val->visited_iter = 0;
    state_val->num_active_children = 0;
    memset(state_val->scan_visited, '\0', sizeof(state_val->scan_visited));
#ifdef INDIRECT_STACK_STATES
    state_val->stacks_copy_on_write_flags = 0;
#endif
}


#if (FCS_STATE_STORAGE != FCS_STATE_STORAGE_INDIRECT)
int fc_solve_state_compare(const void * s1, const void * s2)
{
    return memcmp(s1,s2,sizeof(fcs_state_t));
}

int fc_solve_state_compare_equal(const void * s1, const void * s2)
{
    return (!memcmp(s1,s2,sizeof(fcs_state_t)));
}


int fc_solve_state_compare_with_context(
    const void * s1,
    const void * s2,
    fcs_compare_context_t context GCC_UNUSED
    )
{
    return memcmp(s1,s2,sizeof(fcs_state_t));
}

int fc_solve_state_extra_info_compare_with_context(
    const void * s1,
    const void * s2,
    fcs_compare_context_t context GCC_UNUSED
    )
{
    return
        memcmp(
            (((fcs_state_extra_info_t *)s1)->key),
            (((fcs_state_extra_info_t *)s2)->key),
            sizeof(fcs_state_t)
        );
}
#else
int fc_solve_state_compare_indirect(const void * s1, const void * s2)
{
    return memcmp(*(fcs_state_t * *)s1, *(fcs_state_t * *)s2, sizeof(fcs_state_t));
}

int fc_solve_state_compare_indirect_with_context(const void * s1, const void * s2, void * context)
{
    return memcmp(*(fcs_state_t * *)s1, *(fcs_state_t * *)s2, sizeof(fcs_state_t));
}
#endif

static const char * const freecells_prefixes[] = { "FC:", "Freecells:", "Freecell:", ""};

static const char * const foundations_prefixes[] = { "Decks:", "Deck:", "Founds:", "Foundations:", "Foundation:", "Found:", ""};

#ifdef FCS_WITH_TALONS
static const char * const talon_prefixes[] = { "Talon:", "Queue:" , ""};
static const char * const num_redeals_prefixes[] = { "Num-Redeals:", "Readels-Num:", "Readeals-Number:", ""};
#endif


#ifdef WIN32
#define strncasecmp(a,b,c) (strnicmp((a),(b),(c)))
#endif

int fc_solve_initial_user_state_to_c(
    const char * string,
    fcs_state_t * out_state_key,
    fcs_state_extra_info_t * out_state_val,
    int freecells_num,
    int stacks_num,
    int decks_num
#ifdef FCS_WITH_TALONS
    ,int talon_type
#endif
#ifdef INDIRECT_STACK_STATES
    , char * indirect_stacks_buffer
#endif
    )
{
    int s,c;
    const char * str;
    fcs_card_t card;
    fcs_cards_column_t col;
    int first_line;

    int prefix_found;
    const char * const * prefixes;
    int i;
    int decks_index[4];

    fcs_state_init(
        out_state_key,
        out_state_val,
        stacks_num
#ifdef INDIRECT_STACK_STATES
        , indirect_stacks_buffer
#endif
        );
    str = string;

    first_line = 1;

#define ret (*out_state_key)
/* Handle the end of string - shouldn't happen */
#define handle_eos() \
    { \
        if ((*str) == '\0') \
        {  \
            return FCS_USER_STATE_TO_C__PREMATURE_END_OF_INPUT; \
        } \
    }

#ifdef FCS_WITH_TALONS
    if (talon_type == FCS_TALON_KLONDIKE)
    {
        fcs_klondike_talon_num_redeals_left(ret) = -1;
    }
#endif

    for(s=0;s<stacks_num;s++)
    {
        /* Move to the next stack */
        if (!first_line)
        {
            while((*str) != '\n')
            {
                handle_eos();
                str++;
            }
            str++;
        }
        first_line = 0;

        prefixes = freecells_prefixes;
        prefix_found = 0;
        for(i=0;prefixes[i][0] != '\0'; i++)
        {
            if (!strncasecmp(str, prefixes[i], strlen(prefixes[i])))
            {
                prefix_found = 1;
                str += strlen(prefixes[i]);
                break;
            }
        }

        if (prefix_found)
        {
            for(c=0;c<freecells_num;c++)
            {
                fcs_empty_freecell(ret, c);
            }
            for(c=0;c<freecells_num;c++)
            {
                if (c!=0)
                {
                    while(
                            ((*str) != ' ') &&
                            ((*str) != '\t') &&
                            ((*str) != '\n') &&
                            ((*str) != '\r')
                         )
                    {
                        handle_eos();
                        str++;
                    }
                    if ((*str == '\n') || (*str == '\r'))
                    {
                        break;
                    }
                    str++;
                }

                while ((*str == ' ') || (*str == '\t'))
                {
                    str++;
                }
                if ((*str == '\r') || (*str == '\n'))
                    break;

                if ((*str == '*') || (*str == '-'))
                {
                    card = fcs_empty_card;
                }
                else
                {
                    card = fcs_card_user2perl(str);
                }

                fcs_put_card_in_freecell(ret, c, card);
            }

            while (*str != '\n')
            {
                handle_eos();
                str++;
            }
            s--;
            continue;
        }

        prefixes = foundations_prefixes;
        prefix_found = 0;
        for(i=0;prefixes[i][0] != '\0'; i++)
        {
            if (!strncasecmp(str, prefixes[i], strlen(prefixes[i])))
            {
                prefix_found = 1;
                str += strlen(prefixes[i]);
                break;
            }
        }

        if (prefix_found)
        {
            int d;

            for(d=0;d<decks_num*4;d++)
            {
                fcs_set_foundation(ret, d, 0);
            }

            for(d=0;d<4;d++)
            {
                decks_index[d] = 0;
            }
            while (1)
            {
                while((*str == ' ') || (*str == '\t'))
                    str++;
                if ((*str == '\n') || (*str == '\r'))
                    break;
                d = fcs_u2p_suit(str);
                str++;
                while (*str == '-')
                    str++;
                c = fcs_u2p_card_number(str);
                while (
                        (*str != ' ') &&
                        (*str != '\t') &&
                        (*str != '\n') &&
                        (*str != '\r')
                      )
                {
                    handle_eos();
                    str++;
                }

                fcs_set_foundation(ret, (decks_index[d]*4+d), c);
                decks_index[d]++;
                if (decks_index[d] >= decks_num)
                {
                    decks_index[d] = 0;
                }
            }
            s--;
            continue;
        }

#ifdef FCS_WITH_TALONS
        prefixes = talon_prefixes;
        prefix_found = 0;
        for(i=0;prefixes[i][0] != '\0'; i++)
        {
            if (!strncasecmp(str, prefixes[i], strlen(prefixes[i])))
            {
                prefix_found = 1;
                str += strlen(prefixes[i]);
                break;
            }
        }

        if (prefix_found)
        {
            /* Input the Talon */
            int talon_size;

            talon_size = MAX_NUM_DECKS*52+16;
            ret.talon = malloc(sizeof(fcs_card_t)*talon_size);
            fcs_talon_pos(ret) = 0;

            for(c=0 ; c < talon_size ; c++)
            {
                /* Move to the next card */
                if (c!=0)
                {
                    while(
                        ((*str) != ' ') &&
                        ((*str) != '\t') &&
                        ((*str) != '\n') &&
                        ((*str) != '\r')
                    )
                    {
                        handle_eos();
                        str++;
                    }
                    if ((*str == '\n') || (*str == '\r'))
                    {
                        break;
                    }
                }

                while ((*str == ' ') || (*str == '\t'))
                {
                    str++;
                }

                if ((*str == '\n') || (*str == '\r'))
                {
                    break;
                }

                card = fcs_card_user2perl(str);

                fcs_put_card_in_talon(ret, c+(talon_type==FCS_TALON_KLONDIKE), card);
            }
            fcs_talon_len(ret) = c;

            if (talon_type == FCS_TALON_KLONDIKE)
            {
                int talon_len;

                talon_len = fcs_talon_len(ret);
                fcs_klondike_talon_len(ret) = talon_len;
                fcs_klondike_talon_stack_pos(ret) = -1;
                fcs_klondike_talon_queue_pos(ret) = 0;
            }

            s--;
            continue;
        }

        prefixes = num_redeals_prefixes;
        prefix_found = 0;
        for(i=0;prefixes[i][0] != '\0'; i++)
        {
            if (!strncasecmp(str, prefixes[i], strlen(prefixes[i])))
            {
                prefix_found = 1;
                str += strlen(prefixes[i]);
                break;
            }
        }

        if (prefix_found)
        {
            while ((*str < '0') && (*str > '9') && (*str != '\n'))
            {
                handle_eos();
                str++;
            }
            if (*str != '\n')
            {
                int num_redeals;

                num_redeals = atoi(str);
                if (talon_type == FCS_TALON_KLONDIKE)
                {
                    fcs_klondike_talon_num_redeals_left(ret) =
                        (num_redeals < 0) ?
                            (-1) :
                            ((num_redeals > 127) ? 127 : num_redeals)
                                ;
                }
            }
            s--;
            continue;
        }
#endif

        col = fcs_state_get_col(ret, s);
        for(c=0 ; c < MAX_NUM_CARDS_IN_A_STACK ; c++)
        {
            /* Move to the next card */
            if (c!=0)
            {
                while(
                    ((*str) != ' ') &&
                    ((*str) != '\t') &&
                    ((*str) != '\n') &&
                    ((*str) != '\r')
                )
                {
                    handle_eos();
                    str++;
                }
                if ((*str == '\n') || (*str == '\r'))
                {
                    break;
                }
            }

            while ((*str == ' ') || (*str == '\t'))
            {
                str++;
            }
            handle_eos();
            if ((*str == '\n') || (*str == '\r'))
            {
                break;
            }
            card = fcs_card_user2perl(str);

            fcs_col_push_card(col, card);
        }
    }

    return FCS_USER_STATE_TO_C__SUCCESS;
}

#undef ret
#undef handle_eos

int fc_solve_check_state_validity(
    fcs_state_extra_info_t * state_val,
    int freecells_num,
    int stacks_num,
    int decks_num,
#ifdef FCS_WITH_TALONS
    int talon_type,
#endif
    fcs_card_t * misplaced_card)
{
    int cards[4][14];
    int c, s, d, f;
    int col_len;

    fcs_state_t * state;
    fcs_cards_column_t col;
    fcs_card_t card;

    state = state_val->key;

    /* Initialize all cards to 0 */
    for(d=0;d<4;d++)
    {
        for(c=1;c<=13;c++)
        {
            cards[d][c] = 0;
        }
    }

    /* Mark the cards in the decks */
    for(d=0;d<decks_num*4;d++)
    {
        for(c=1;c<=fcs_foundation_value(*state, d);c++)
        {
            cards[d%4][c]++;
        }
    }

    /* Mark the cards in the freecells */
    for(f=0;f<freecells_num;f++)
    {
        if (fcs_freecell_card_num(*state, f) != 0)
        {
            cards
                [fcs_freecell_card_suit(*state, f)]
                [fcs_freecell_card_num(*state, f)] ++;
        }
    }

    /* Mark the cards in the stacks */
    for(s=0;s<stacks_num;s++)
    {
        col = fcs_state_get_col(*state, s);
        col_len = fcs_col_len(col);
        for(c=0;c<col_len;c++)
        {
            card = fcs_col_get_card(col,c);
            if (fcs_card_card_num(card) == 0)
            {
                *misplaced_card = fcs_empty_card;
                return FCS_STATE_VALIDITY__EMPTY_SLOT;
            }
            cards
                [fcs_card_suit(card)]
                [fcs_card_card_num(card)] ++;

        }
    }

#ifdef FCS_WITH_TALONS
    /* Mark the cards in the (gypsy) talon */
    if ((talon_type == FCS_TALON_GYPSY) || (talon_type == FCS_TALON_KLONDIKE))
    {
        for(c = ((talon_type == FCS_TALON_GYPSY)?fcs_talon_pos(*state):1) ;
            c < ((talon_type==FCS_TALON_GYPSY) ? fcs_talon_len(*state) : (fcs_klondike_talon_len(*state)+1)) ;
            c++)
        {
            if (fcs_get_talon_card(*state,c) != fcs_empty_card)
            {
                cards
                    [fcs_card_suit(fcs_get_talon_card(*state, c))]
                    [fcs_card_card_num(fcs_get_talon_card(*state, c))] ++;
            }
        }
    }
#endif

    /* Now check if there are extra or missing cards */

    for(d=0;d<4;d++)
    {
        for(c=1;c<=13;c++)
        {
            if (cards[d][c] != decks_num)
            {
                *misplaced_card = fcs_empty_card;
                fcs_card_set_suit(*misplaced_card, d);
                fcs_card_set_num(*misplaced_card, c);
                return ((cards[d][c] < decks_num) 
                    ? FCS_STATE_VALIDITY__MISSING_CARD
                    : FCS_STATE_VALIDITY__EXTRA_CARD
                    )
                    ;
            }
        }
    }

    return FCS_STATE_VALIDITY__OK;
}

#undef state


char * fc_solve_state_as_string(
    fcs_state_extra_info_t * state_val,
    int freecells_num,
    int stacks_num,
    int decks_num,
    int parseable_output,
    int canonized_order_output,
    int display_10_as_t
    )
{
    fcs_state_t * state;
    char freecell[10], decks[MAX_NUM_DECKS*4][10], stack_card_[10];
    int a, card_num_is_null, b;
    int max_num_cards, s, card_num;
    fcs_cards_column_t col;
    int col_len;

    char str2[128], str3[128], * str2_ptr, * str3_ptr;

    fc_solve_append_string_t * app_str;

    int stack_locs[MAX_NUM_STACKS];
    int freecell_locs[MAX_NUM_FREECELLS];

    state = state_val->key;

    if (canonized_order_output)
    {
        for(a=0;a<stacks_num;a++)
        {
            stack_locs[a] = a;
        }
        for(a=0;a<freecells_num;a++)
        {
            freecell_locs[a] = a;
        }
    }
    else
    {
        for(a=0;a<stacks_num;a++)
        {
            stack_locs[(int)(state_val->stack_locs[a])] = a;
        }
        for(a=0;a<freecells_num;a++)
        {
            freecell_locs[(int)(state_val->fc_locs[a])] = a;
        }
    }

    for(a=0;a<decks_num*4;a++)
    {
        fc_solve_p2u_card_number(
            fcs_foundation_value(*state, a),
            decks[a],
            &card_num_is_null,
            display_10_as_t
#ifndef FCS_WITHOUT_CARD_FLIPPING
            ,0
#endif
            );
        if (decks[a][0] == ' ')
            decks[a][0] = '0';
    }

    app_str = fc_solve_append_string_alloc(512);

    if(!parseable_output)
    {
        for(a=0;a<((freecells_num/4)+((freecells_num%4==0)?0:1));a++)
        {
            str2_ptr = str2;
            str3_ptr = str3;
            for(b=0;b<min(freecells_num-a*4, 4);b++)
            {
                str2_ptr += sprintf(str2_ptr, "%3s ",
                    fcs_card_perl2user(
                        fcs_freecell_card(
                            *state,
                            freecell_locs[a*4+b]
                        ),
                        freecell,
                        display_10_as_t
                    )
                );
                str3_ptr += sprintf(str3_ptr, "--- ");
            }
            if (a < decks_num)
            {
                fc_solve_append_string_sprintf(
                    app_str,
                    "%-16s        H-%1s C-%1s D-%1s S-%1s\n",
                    str2,
                    decks[a*4],
                    decks[a*4+1],
                    decks[a*4+2],
                    decks[a*4+3]
                    );
            }
            else
            {
                fc_solve_append_string_sprintf(
                    app_str,
                    "%s\n", str2
                    );
            }
            fc_solve_append_string_sprintf(
                app_str,
                "%s\n", str3
                );
        }
        for(;a<decks_num;a++)
        {
            fc_solve_append_string_sprintf(
                app_str,
                "%-16s        H-%1s C-%1s D-%1s S-%1s\n",
                "",
                decks[a*4],
                decks[a*4+1],
                decks[a*4+2],
                decks[a*4+3]
                );
        }
        fc_solve_append_string_sprintf(
            app_str,
            "%s",
            "\n\n"
            );

        for(s=0;s<stacks_num;s++)
        {
            fc_solve_append_string_sprintf(app_str, "%s", " -- ");
        }
        fc_solve_append_string_sprintf(
            app_str,
            "%s",
            "\n"
            );

        max_num_cards = 0;
        for(s=0;s<stacks_num;s++)
        {
            col = fcs_state_get_col(*state, stack_locs[s]);
            col_len = fcs_col_len(col);
            if (col_len > max_num_cards)
            {
                max_num_cards = col_len;
            }
        }

        for(card_num=0;card_num<max_num_cards;card_num++)
        {
            for(s = 0; s<stacks_num; s++)
            {
                col = fcs_state_get_col(*state, stack_locs[s]);
                col_len = fcs_col_len(col);
                if (card_num >= col_len)
                {
                    fc_solve_append_string_sprintf(
                        app_str,
                        "    "
                        );
                }
                else
                {
                    fc_solve_append_string_sprintf(
                        app_str,
                        "%3s ",
                        fcs_card_perl2user(
                            fcs_col_get_card(col, card_num),
                            stack_card_,
                            display_10_as_t
                            )
                        );
                }
            }
            fc_solve_append_string_sprintf(app_str, "%s", "\n");
        }
    }
    else
    {
        fc_solve_append_string_sprintf(app_str, "%s", "Foundations: ");
        for(a=0;a<decks_num;a++)
        {
            fc_solve_append_string_sprintf(
                app_str,
                "H-%s C-%s D-%s S-%s ",
                decks[a*4],
                decks[a*4+1],
                decks[a*4+2],
                decks[a*4+3]
                );
        }

        fc_solve_append_string_sprintf(app_str, "%s", "\nFreecells: ");

        for(a=0;a<freecells_num;a++)
        {
            fc_solve_append_string_sprintf(
                app_str,
                "%3s",
                fcs_card_perl2user(
                    fcs_freecell_card(
                        *state,
                        freecell_locs[a]
                    ),
                    freecell,
                    display_10_as_t
                )
            );
            if (a < freecells_num-1)
            {
                fc_solve_append_string_sprintf(app_str, "%s", " ");
            }
        }
        fc_solve_append_string_sprintf(app_str, "%s", "\n");

        for(s=0;s<stacks_num;s++)
        {
            col = fcs_state_get_col(*state, stack_locs[s]);
            col_len = fcs_col_len(col);
            fc_solve_append_string_sprintf(app_str, "%s", ": ");

            for(card_num=0;card_num<col_len;card_num++)
            {
                fcs_card_perl2user(
                    fcs_col_get_card(col, card_num),
                    stack_card_,
                    display_10_as_t
                );
                fc_solve_append_string_sprintf(app_str, "%s", stack_card_);
                if (card_num < col_len-1)
                {
                    fc_solve_append_string_sprintf(app_str, "%s", " ");
                }
            }
            fc_solve_append_string_sprintf(app_str, "%s", "\n");
        }
    }

    return fc_solve_append_string_finalize(app_str);
}
