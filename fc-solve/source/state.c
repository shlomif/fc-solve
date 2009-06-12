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

#ifdef WIN32
#define strncasecmp(a,b,c) (strnicmp((a),(b),(c)))
#endif

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

    fc_solve_append_string_t app_str_struct;
#define app_str (&app_str_struct)

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

    fc_solve_append_string_init(&app_str_struct);

    if(!parseable_output)
    {
        for(a=0;a<((freecells_num/4)+((freecells_num%4==0)?0:1));a++)
        {
            str2_ptr = str2;
            str3_ptr = str3;
            for(b=0;b<min(freecells_num-a*4, 4);b++)
            {
                str2_ptr += sprintf(str2_ptr, "%3s ",
                    fc_solve_card_perl2user(
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
                        fc_solve_card_perl2user(
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
                fc_solve_card_perl2user(
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
                fc_solve_card_perl2user(
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

    return fc_solve_append_string_finalize(&app_str_struct);
}
