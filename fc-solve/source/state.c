/*
 * This file is part of Freecell Solver. It is subject to the license terms in
 * the COPYING.txt file found in the top-level directory of this distribution
 * and at http://fc-solve.shlomifish.org/docs/distro/COPYING.html . No part of
 * Freecell Solver, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the COPYING file.
 *
 * Copyright (c) 2000 Shlomi Fish
 */
/*
 * state.c - state functions module for Freecell Solver
 */

#include "dll_thunk.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "config.h"
#include "state.h"
#include "fcs_enums.h"

#include "p2u_rank.h"

#ifdef DEBUG_STATES
#define GET_CARD(s) (((const fcs_DEBUG_STATES_stack_t *const)(s))->cards[0])
#elif defined(COMPACT_STATES)
#define GET_CARD(s) (((const fcs_card_t *const)(s))[1])
#endif

#ifdef GET_CARD
static GCC_INLINE int fcs_stack_compare(const void *s1, const void *s2)
{
    return fc_solve_card_compare(GET_CARD(s1), GET_CARD(s2));
}
#undef GET_CARD
#endif

#ifdef COMPACT_STATES

#define DECLARE_TEMP_STACK() char temp_stack[(MAX_NUM_CARDS_IN_A_STACK + 1)]
#define STACK_COMPARE(a, b) (fcs_stack_compare((a), (b)))
#define GET_STACK(c) (state_key->data + (c) * (MAX_NUM_CARDS_IN_A_STACK + 1))
#define COPY_STACK(d, s) (memcpy(d, s, (MAX_NUM_CARDS_IN_A_STACK + 1)))
#define GET_FREECELL(c) (state_key->data[FCS_FREECELLS_OFFSET + (c)])

#elif defined(DEBUG_STATES) || defined(INDIRECT_STACK_STATES)

#ifdef DEBUG_STATES

#define DECLARE_TEMP_STACK() fcs_DEBUG_STATES_stack_t temp_stack
#define STACK_COMPARE(a, b) (fcs_stack_compare((&(a)), (&(b))))

#else

#define DECLARE_TEMP_STACK() fcs_card_t *temp_stack
#define STACK_COMPARE(a, b) (fc_solve_stack_compare_for_comparison(a, b))

#endif

#define GET_STACK(c) (state_key->stacks[c])
#define COPY_STACK(d, s) (d = s)
#define GET_FREECELL(c) (state_key->freecells[(c)])

#endif

void fc_solve_canonize_state(
    fcs_state_t *const ptr_state_key FREECELLS_AND_STACKS_ARGS())
{

#define state_key (ptr_state_key)
    /* Insertion-sort the stacks */

    for (size_t b = 1; b < STACKS_NUM__VAL; b++)
    {
        size_t c = b;
        while ((c > 0) && ((STACK_COMPARE(GET_STACK(c), GET_STACK(c - 1))) < 0))
        {
            {
                DECLARE_TEMP_STACK();
                COPY_STACK(temp_stack, GET_STACK(c));
                COPY_STACK(GET_STACK(c), GET_STACK(c - 1));
                COPY_STACK(GET_STACK(c - 1), temp_stack);
            }

            c--;
        }
    }

    /* Insertion-sort the freecells */

    for (size_t b = 1; b < FREECELLS_NUM__VAL; b++)
    {
        size_t c = b;

        while ((c > 0) && ((fc_solve_card_compare(
                               (GET_FREECELL(c)), (GET_FREECELL(c - 1)))) < 0))
        {
            const fcs_card_t temp_freecell = GET_FREECELL(c);
            GET_FREECELL(c) = GET_FREECELL(c - 1);
            GET_FREECELL(c - 1) = temp_freecell;

            c--;
        }
    }
}
#undef state_key

void fc_solve_canonize_state_with_locs(fcs_state_t *const ptr_state_key,
#define state_key (ptr_state_key)
    fcs_state_locs_struct_t *const locs FREECELLS_AND_STACKS_ARGS())
{
    /* Insertion-sort the stacks */
    for (size_t b = 1; b < STACKS_NUM__VAL; b++)
    {
        size_t c = b;
        while ((c > 0) && ((STACK_COMPARE(GET_STACK(c), GET_STACK(c - 1))) < 0))
        {
            {
                DECLARE_TEMP_STACK();
                COPY_STACK(temp_stack, GET_STACK(c));
                COPY_STACK(GET_STACK(c), GET_STACK(c - 1));
                COPY_STACK(GET_STACK(c - 1), temp_stack);
            }

            const_AUTO(swap_loc, locs->stack_locs[c]);
            locs->stack_locs[c] = locs->stack_locs[c - 1];
            locs->stack_locs[c - 1] = swap_loc;

            c--;
        }
    }

    /* Insertion-sort the freecells */

    for (size_t b = 1; b < FREECELLS_NUM__VAL; b++)
    {
        size_t c = b;

        while ((c > 0) && ((fc_solve_card_compare(
                               (GET_FREECELL(c)), (GET_FREECELL(c - 1)))) < 0))
        {
            const fcs_card_t temp_freecell = GET_FREECELL(c);
            GET_FREECELL(c) = GET_FREECELL(c - 1);
            GET_FREECELL(c - 1) = temp_freecell;

            const_AUTO(swap_loc, locs->fc_locs[c]);
            locs->fc_locs[c] = locs->fc_locs[c - 1];
            locs->fc_locs[c - 1] = swap_loc;

            c--;
        }
    }
}
#undef state_key

#if (FCS_STATE_STORAGE == FCS_STATE_STORAGE_GLIB_HASH)
int fc_solve_state_compare_equal(const void *const s1, const void *const s2)
{
    return (!memcmp(s1, s2, sizeof(fcs_state_t)));
}
#endif

int fc_solve_state_compare_with_context(
    const void *s1, const void *s2, fcs_compare_context_t context GCC_UNUSED)
{
    return memcmp(s1, s2, sizeof(fcs_state_t));
}

static GCC_INLINE void render_freecell_card(const fcs_card_t card,
    char *const freecell PASS_T(const fcs_bool_t display_10_as_t))
{
    if (fcs_card_is_empty(card))
    {
        freecell[0] = '\0';
    }
    else
    {
        fc_solve_card_stringify(card, freecell PASS_T(display_10_as_t));
    }
}

void fc_solve_state_as_string(char *output_s, const fcs_state_t *const state,
    const fcs_state_locs_struct_t *const state_locs
        FREECELLS_STACKS_DECKS__ARGS()
            FC_SOLVE__PASS_PARSABLE(const fcs_bool_t parseable_output),
    const fcs_bool_t canonized_order_output PASS_T(
        const fcs_bool_t display_10_as_t))
{
    typedef char one_card_buffer[4];
    char decks[MAX_NUM_DECKS * 4][10];

#define fc_solve_append_string_sprintf(...)                                    \
    output_s += sprintf(output_s, __VA_ARGS__)
#define append_char(c) *(output_s++) = (c)

    int stack_locs[MAX_NUM_STACKS];
    int freecell_locs[MAX_NUM_FREECELLS];

    if (canonized_order_output)
    {
        for (int i = 0; i < STACKS_NUM__VAL; i++)
        {
            stack_locs[i] = i;
        }
        for (int i = 0; i < FREECELLS_NUM__VAL; i++)
        {
            freecell_locs[i] = i;
        }
    }
    else
    {
        for (int i = 0; i < STACKS_NUM__VAL; i++)
        {
            stack_locs[(int)(state_locs->stack_locs[i])] = i;
        }
        for (int i = 0; i < FREECELLS_NUM__VAL; i++)
        {
            freecell_locs[(int)(state_locs->fc_locs[i])] = i;
        }
    }

    for (int i = 0; i < (DECKS_NUM__VAL << 2); i++)
    {
        fc_solve_p2u_rank(
            fcs_foundation_value(*state, i), decks[i] PASS_T(display_10_as_t));
        if (decks[i][0] == ' ')
            decks[i][0] = '0';
    }

#if 0
    fc_solve_append_string_init(&app_str_struct);
#endif

#ifndef FC_SOLVE_IMPLICIT_PARSABLE_OUTPUT
    if (!parseable_output)
    {
        int i;
        for (i = 0; i < ((FREECELLS_NUM__VAL / 4) +
                            ((FREECELLS_NUM__VAL % 4 == 0) ? 0 : 1));
             i++)
        {
            char dashes_s[128];
            char *dashes_ptr = dashes_s;
            char freecells_s[128];
            char *freecells_s_end = freecells_s;
            for (int b = 0; b < min(FREECELLS_NUM__VAL - i * 4, 4); b++)
            {
                one_card_buffer freecell;
                render_freecell_card(
                    fcs_freecell_card(*state, freecell_locs[i * 4 + b]),
                    freecell PASS_T(display_10_as_t));
                freecells_s_end += sprintf(freecells_s_end, "%3s ", freecell);
                strcpy(dashes_ptr, "--- ");
                dashes_ptr = strchr(dashes_ptr, '\0');
            }
            if (i < DECKS_NUM__VAL)
            {
                fc_solve_append_string_sprintf(
                    "%-16s        H-%1s C-%1s D-%1s S-%1s\n", freecells_s,
                    decks[i * 4], decks[i * 4 + 1], decks[i * 4 + 2],
                    decks[i * 4 + 3]);
            }
            else
            {
                fc_solve_append_string_sprintf("%s\n", freecells_s);
            }
            fc_solve_append_string_sprintf("%s\n", dashes_s);
        }
        for (; i < DECKS_NUM__VAL; i++)
        {
            fc_solve_append_string_sprintf(
                "%-16s        H-%1s C-%1s D-%1s S-%1s\n", "", decks[i * 4],
                decks[i * 4 + 1], decks[i * 4 + 2], decks[i * 4 + 3]);
        }
        fc_solve_append_string_sprintf("%s", "\n\n");

        for (int s = 0; s < STACKS_NUM__VAL; s++)
        {
            fc_solve_append_string_sprintf("%s", " -- ");
        }
        append_char('\n');

        int max_num_cards = 0;
        for (int s = 0; s < STACKS_NUM__VAL; s++)
        {
            fcs_const_cards_column_t col =
                fcs_state_get_col(*state, stack_locs[s]);
            const int col_len = fcs_col_len(col);
            if (col_len > max_num_cards)
            {
                max_num_cards = col_len;
            }
        }

        for (int card_idx = 0; card_idx < max_num_cards; card_idx++)
        {
            for (int s = 0; s < STACKS_NUM__VAL; s++)
            {
                fcs_const_cards_column_t col =
                    fcs_state_get_col(*state, stack_locs[s]);
                const int col_len = fcs_col_len(col);
                if (card_idx >= col_len)
                {
                    fc_solve_append_string_sprintf("    ");
                }
                else
                {
                    one_card_buffer stack_card_str;
                    fc_solve_card_stringify(fcs_col_get_card(col, card_idx),
                        stack_card_str PASS_T(display_10_as_t));
                    fc_solve_append_string_sprintf("%3s ", stack_card_str);
                }
            }
            fc_solve_append_string_sprintf("%s", "\n");
        }
    }
    else
#endif
    {
        fc_solve_append_string_sprintf("%s", "Foundations:");
        for (int i = 0; i < (DECKS_NUM__VAL << 2); i += 4)
        {
            fc_solve_append_string_sprintf(" H-%s C-%s D-%s S-%s", decks[i],
                decks[i + 1], decks[i + 2], decks[i + 3]);
        }
#ifndef FC_SOLVE__REMOVE_TRAILING_WHITESPACE_IN_OUTPUT
        append_char(' ');
#endif

        fc_solve_append_string_sprintf("%s", "\nFreecells:");

#ifdef FC_SOLVE__REMOVE_TRAILING_WHITESPACE_IN_OUTPUT
        int max_freecell_idx = -1;
        for (int i = FREECELLS_NUM__VAL - 1; i >= 0; i--)
        {
            if (fcs_card_is_valid(fcs_freecell_card(*state, freecell_locs[i])))
            {
                max_freecell_idx = i;
                break;
            }
        }
#else
        const int max_freecell_idx = FREECELLS_NUM__VAL - 1;
#endif

        if (max_freecell_idx >= 0)
        {
            append_char(' ');
        }
        for (int i = 0; i <= max_freecell_idx; i++)
        {
            one_card_buffer freecell;
            render_freecell_card(fcs_freecell_card(*state, freecell_locs[i]),
                freecell PASS_T(display_10_as_t));
            fc_solve_append_string_sprintf("%3s", freecell);
            if (i < max_freecell_idx)
            {
                append_char(' ');
            }
        }
        append_char('\n');

        for (int s = 0; s < STACKS_NUM__VAL; s++)
        {
            fcs_const_cards_column_t col =
                fcs_state_get_col(*state, stack_locs[s]);
            const int col_len = fcs_col_len(col);
            append_char(':');

            for (int card_idx = 0; card_idx < col_len; card_idx++)
            {
                one_card_buffer stack_card_str;
                fc_solve_card_stringify(fcs_col_get_card(col, card_idx),
                    stack_card_str PASS_T(display_10_as_t));
                fc_solve_append_string_sprintf(" %s", stack_card_str);
            }

#ifndef FC_SOLVE__REMOVE_TRAILING_WHITESPACE_IN_OUTPUT
            if (!col_len)
            {
                append_char(' ');
            }
#endif
            append_char('\n');
        }
    }
    *(output_s) = '\0';
}
