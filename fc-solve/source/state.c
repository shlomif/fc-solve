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

#define BUILDING_DLL 1

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "config.h"
#include "state.h"
#include "fcs_enums.h"
#include "app_str.h"
#include "unused.h"
#include "inline.h"

#ifdef DEBUG_STATES
static GCC_INLINE int fcs_stack_compare(const void * s1, const void * s2)
{
#define GET_CARD(s) (((const fc_stack_t *)(s))->cards[0])
    return fc_solve_card_compare(GET_CARD(s1), GET_CARD(s2));
#undef GET_CARD
}
#elif defined(COMPACT_STATES)
static GCC_INLINE int fcs_stack_compare(const void * s1, const void * s2)
{
#define GET_CARD(s) (((const fcs_card_t *)(s))[1])
    return fc_solve_card_compare(GET_CARD(s1), GET_CARD(s2));
#undef GET_CARD
}
#endif

#ifdef COMPACT_STATES

#define DECLARE_TEMP_STACK() char temp_stack[(MAX_NUM_CARDS_IN_A_STACK+1)]
#define STACK_COMPARE(a,b) (fcs_stack_compare((a),(b)))
#define GET_STACK(c) (state_key->data+(c)*(MAX_NUM_CARDS_IN_A_STACK+1))
#define COPY_STACK(d,s) (memcpy(d, s, (MAX_NUM_CARDS_IN_A_STACK+1)))
#define GET_FREECELL(c) (state_key->data[FCS_FREECELLS_OFFSET+(c)])

#elif defined(DEBUG_STATES) || defined(INDIRECT_STACK_STATES)

#ifdef DEBUG_STATES

#define DECLARE_TEMP_STACK() fc_stack_t temp_stack
#define STACK_COMPARE(a,b) (fcs_stack_compare((&(a)),(&(b))))

#else

#define DECLARE_TEMP_STACK() fcs_card_t * temp_stack
#define STACK_COMPARE(a,b) (fc_solve_stack_compare_for_comparison(a,b))

#endif

#define GET_STACK(c) (state_key->stacks[c])
#define COPY_STACK(d,s) (d = s)
#define GET_FREECELL(c) (state_key->freecells[(c)])

#endif

void fc_solve_canonize_state(
    fcs_state_t * const ptr_state_key
    FREECELLS_AND_STACKS_ARGS()
)
{
    DECLARE_TEMP_STACK();
    fcs_card_t temp_freecell;

#define state_key (ptr_state_key)
    /* Insertion-sort the stacks */

    for (int b=1 ; b < STACKS_NUM__VAL ; b++)
    {
        int c = b;
        while(
            (c>0)    &&
            ((STACK_COMPARE(
                       GET_STACK(c),
                       GET_STACK(c-1)
                          )
                ) < 0
            )
        )
        {
            COPY_STACK(temp_stack, GET_STACK(c));
            COPY_STACK(GET_STACK(c), GET_STACK(c-1));
            COPY_STACK(GET_STACK(c-1), temp_stack);

            c--;
        }
    }

    /* Insertion-sort the freecells */

    for (int b=1 ; b < FREECELLS_NUM__VAL ; b++)
    {
        int c = b;

        while (
            (c>0)    &&
            ((fc_solve_card_compare(
                (GET_FREECELL(c)),
                (GET_FREECELL(c-1))
            )
            ) < 0)
        )
        {
            temp_freecell = GET_FREECELL(c);
            GET_FREECELL(c) = GET_FREECELL(c-1);
            GET_FREECELL(c-1) = temp_freecell;

            c--;
        }
    }
}
#undef state_key


void fc_solve_canonize_state_with_locs(
    fcs_state_t * const ptr_state_key,
#define state_key (ptr_state_key)
    fcs_state_locs_struct_t * const locs
    FREECELLS_AND_STACKS_ARGS()
)
{
    DECLARE_TEMP_STACK();
    fcs_card_t temp_freecell;
    fcs_locs_t temp_loc;

    /* Insertion-sort the stacks */

    for ( int b=1 ; b < STACKS_NUM__VAL ; b++)
    {
        int c = b;
        while(
            (c>0)    &&
            ((STACK_COMPARE(
                       GET_STACK(c),
                       GET_STACK(c-1)
                          )
                ) < 0
            )
        )
        {
            COPY_STACK(temp_stack, GET_STACK(c));
            COPY_STACK(GET_STACK(c), GET_STACK(c-1));
            COPY_STACK(GET_STACK(c-1), temp_stack);

            temp_loc = locs->stack_locs[c];
            locs->stack_locs[c] = locs->stack_locs[c-1];
            locs->stack_locs[c-1] = temp_loc;

            c--;
        }
    }

    /* Insertion-sort the freecells */

    for (int b=1 ; b < FREECELLS_NUM__VAL ; b++)
    {
        int c = b;

        while(
            (c>0)    &&
            ((fc_solve_card_compare(
                (GET_FREECELL(c)),
                (GET_FREECELL(c-1))
            )
            ) < 0)
        )
        {
            temp_freecell = GET_FREECELL(c);
            GET_FREECELL(c) = GET_FREECELL(c-1);
            GET_FREECELL(c-1) = temp_freecell;

            temp_loc = locs->fc_locs[c];
            locs->fc_locs[c] = locs->fc_locs[c-1];
            locs->fc_locs[c-1] = temp_loc;

            c--;
        }
    }
}
#undef state_key


#if (FCS_STATE_STORAGE != FCS_STATE_STORAGE_INDIRECT)

#if (FCS_STATE_STORAGE == FCS_STATE_STORAGE_GLIB_HASH)
int fc_solve_state_compare_equal(const void * const s1, const void * const s2)
{
    return (!memcmp(s1,s2,sizeof(fcs_state_t)));
}
#endif

int fc_solve_state_compare_with_context(
    const void * s1,
    const void * s2,
    fcs_compare_context_t context GCC_UNUSED
    )
{
    return memcmp(s1,s2,sizeof(fcs_state_t));
}

#else

int fc_solve_state_compare_indirect(const void * const s1, const void * const s2)
{
    return memcmp(*(fcs_state_t * *)s1, *(fcs_state_t * *)s2, sizeof(fcs_state_t));
}

int fc_solve_state_compare_indirect_with_context(const void * s1, const void * s2, void * context)
{
    return memcmp(*(fcs_state_t * *)s1, *(fcs_state_t * *)s2, sizeof(fcs_state_t));
}

#endif

char * fc_solve_state_as_string(
    const fcs_state_t * const state,
    const fcs_state_locs_struct_t * const state_locs
#define FCS_S_STACK_LOCS() (state_locs->stack_locs)
#define FCS_S_FC_LOCS() (state_locs->fc_locs)
    FREECELLS_AND_STACKS_ARGS()
    ,
    const int decks_num,
    const fcs_bool_t parseable_output,
    const fcs_bool_t canonized_order_output,
    const fcs_bool_t display_10_as_t
    )
{
    char freecell[10], decks[MAX_NUM_DECKS*4][10], stack_card_str[10];
    int a, b;
    fcs_bool_t rank_is_null;
    int max_num_cards, s, card_idx;
    fcs_cards_column_t col;
    int col_len;

    char str2[128], * str2_ptr;

    fc_solve_append_string_t app_str_struct;
#define app_str (&app_str_struct)

    int stack_locs[MAX_NUM_STACKS];
    int freecell_locs[MAX_NUM_FREECELLS];

    if (canonized_order_output)
    {
        for (a = 0 ; a < STACKS_NUM__VAL ; a++)
        {
            stack_locs[a] = a;
        }
        for (a = 0 ; a < FREECELLS_NUM__VAL ; a++)
        {
            freecell_locs[a] = a;
        }
    }
    else
    {
        for (a = 0 ; a < STACKS_NUM__VAL ; a++)
        {
            stack_locs[(int)((FCS_S_STACK_LOCS())[a])] = a;
        }
        for (a = 0 ; a < FREECELLS_NUM__VAL ; a++)
        {
            freecell_locs[(int)((FCS_S_FC_LOCS())[a])] = a;
        }
    }

    for(a=0;a<decks_num*4;a++)
    {
        fc_solve_p2u_rank(
            fcs_foundation_value(*state, a),
            decks[a],
            &rank_is_null,
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
        for(a=0;a<((FREECELLS_NUM__VAL/4)+((FREECELLS_NUM__VAL%4==0)?0:1));a++)
        {
            char dashes_s[128];
            char * dashes_ptr = dashes_s;
            str2_ptr = str2;
            for(b=0;b<min(FREECELLS_NUM__VAL-a*4, 4);b++)
            {
                fc_solve_card_perl2user(
                    fcs_freecell_card(
                        *state,
                        freecell_locs[a*4+b]
                    ),
                    freecell,
                    display_10_as_t
                );
                str2_ptr += sprintf(str2_ptr, "%3s ", freecell);
                strcpy(dashes_ptr, "--- ");
                dashes_ptr = strchr(dashes_ptr, '\0');
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
                "%s\n", dashes_ptr
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

        for (s = 0 ; s < STACKS_NUM__VAL ; s++)
        {
            fc_solve_append_string_sprintf(app_str, "%s", " -- ");
        }
        fc_solve_append_string_sprintf(
            app_str,
            "%s",
            "\n"
            );

        max_num_cards = 0;
        for(s=0;s<STACKS_NUM__VAL;s++)
        {
            col = fcs_state_get_col(*state, stack_locs[s]);
            col_len = fcs_col_len(col);
            if (col_len > max_num_cards)
            {
                max_num_cards = col_len;
            }
        }

        for (card_idx = 0 ; card_idx < max_num_cards ; card_idx++)
        {
            for(s = 0; s<STACKS_NUM__VAL; s++)
            {
                col = fcs_state_get_col(*state, stack_locs[s]);
                col_len = fcs_col_len(col);
                if (card_idx >= col_len)
                {
                    fc_solve_append_string_sprintf(
                        app_str,
                        "    "
                        );
                }
                else
                {
                    fc_solve_card_perl2user(
                        fcs_col_get_card(col, card_idx),
                        stack_card_str,
                        display_10_as_t
                    );
                    fc_solve_append_string_sprintf(
                        app_str,
                        "%3s ",
                        stack_card_str
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

        for(a=0;a<FREECELLS_NUM__VAL;a++)
        {
            fc_solve_card_perl2user(
                fcs_freecell_card(
                    *state,
                    freecell_locs[a]
                ),
                freecell,
                display_10_as_t
            );
            fc_solve_append_string_sprintf(
                app_str,
                "%3s",
                freecell
            );
            if (a < FREECELLS_NUM__VAL-1)
            {
                fc_solve_append_string_sprintf(app_str, "%s", " ");
            }
        }
        fc_solve_append_string_sprintf(app_str, "%s", "\n");

        for(s=0;s<STACKS_NUM__VAL;s++)
        {
            col = fcs_state_get_col(*state, stack_locs[s]);
            col_len = fcs_col_len(col);
            fc_solve_append_string_sprintf(app_str, "%s", ": ");

            for (card_idx = 0 ; card_idx < col_len ; card_idx++)
            {
                fc_solve_card_perl2user(
                    fcs_col_get_card(col, card_idx),
                    stack_card_str,
                    display_10_as_t
                );
                fc_solve_append_string_sprintf(app_str, "%s", stack_card_str);
                if (card_idx < col_len-1)
                {
                    fc_solve_append_string_sprintf(app_str, "%s", " ");
                }
            }
            fc_solve_append_string_sprintf(app_str, "%s", "\n");
        }
    }
#undef app_str

    return fc_solve_append_string_finalize(&app_str_struct);
}

#undef FCS_S_FC_LOCS
#undef FCS_S_STACK_LOCS

