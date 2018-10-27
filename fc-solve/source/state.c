/*
 * This file is part of Freecell Solver. It is subject to the license terms in
 * the COPYING.txt file found in the top-level directory of this distribution
 * and at http://fc-solve.shlomifish.org/docs/distro/COPYING.html . No part of
 * Freecell Solver, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the COPYING file.
 *
 * Copyright (c) 2000 Shlomi Fish
 */
// state.c - state functions module for Freecell Solver
#include "state.h"
#include "freecell-solver/fcs_enums.h"

#ifdef COMPACT_STATES
#define GET_CARD(s) (((const fcs_card *const)(s))[1])
static inline int fcs_stack_compare(const void *s1, const void *s2)
{
    return fc_solve_card_compare(GET_CARD(s1), GET_CARD(s2));
}
#undef GET_CARD
#endif

#ifdef COMPACT_STATES

#define DECLARE_TEMP_STACK() char temp_stack[MAX_NUM_CARDS_IN_A_STACK + 1]
#define STACK_COMPARE(a, b) (fcs_stack_compare((a), (b)))
#define GET_STACK(c) (state_key->data + (c) * (MAX_NUM_CARDS_IN_A_STACK + 1))
#define COPY_STACK(d, s) (memcpy(d, s, (MAX_NUM_CARDS_IN_A_STACK + 1)))

#elif defined(INDIRECT_STACK_STATES)

#define DECLARE_TEMP_STACK() fcs_card *temp_stack
#define STACK_COMPARE(a, b) (fc_solve_stack_compare_for_comparison(a, b))
#define GET_STACK(c) (state_key->columns[c])
#define COPY_STACK(d, s) (d = s)

#endif
#if MAX_NUM_FREECELLS > 0
#define GET_FREECELL(c) (fcs_freecell_card(*state_key, (c)))
#endif

void fc_solve_canonize_state(
    fcs_state *const ptr_state_key FREECELLS_AND_STACKS_ARGS())
{
#define state_key (ptr_state_key)
    /* Insertion-sort the columns */

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

            --c;
        }
    }

#if MAX_NUM_FREECELLS > 0
    // Insertion-sort the freecells
    for (size_t b = 1; b < FREECELLS_NUM__VAL; b++)
    {
        size_t c = b;

        while ((c > 0) && ((fc_solve_card_compare(
                               GET_FREECELL(c), GET_FREECELL(c - 1))) < 0))
        {
            const fcs_card temp_freecell = GET_FREECELL(c);
            GET_FREECELL(c) = GET_FREECELL(c - 1);
            GET_FREECELL(c - 1) = temp_freecell;

            --c;
        }
    }
#endif
}
#undef state_key

#ifdef FCS_WITH_MOVES
void fc_solve_canonize_state_with_locs(fcs_state *const ptr_state_key,
#define state_key (ptr_state_key)
    fcs_state_locs_struct *const locs FREECELLS_AND_STACKS_ARGS())
{
    // Insertion-sort the columns
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

            --c;
        }
    }

#if MAX_NUM_FREECELLS > 0
    // Insertion-sort the freecells
    for (size_t b = 1; b < FREECELLS_NUM__VAL; b++)
    {
        size_t c = b;

        while ((c > 0) && ((fc_solve_card_compare(
                               GET_FREECELL(c), GET_FREECELL(c - 1))) < 0))
        {
            const fcs_card temp_freecell = GET_FREECELL(c);
            GET_FREECELL(c) = GET_FREECELL(c - 1);
            GET_FREECELL(c - 1) = temp_freecell;

            const_AUTO(swap_loc, locs->fc_locs[c]);
            locs->fc_locs[c] = locs->fc_locs[c - 1];
            locs->fc_locs[c - 1] = swap_loc;

            --c;
        }
    }
#endif
}
#undef state_key
#endif

#if (FCS_STATE_STORAGE == FCS_STATE_STORAGE_GLIB_HASH)
int fc_solve_state_compare_equal(const void *const s1, const void *const s2)
{
    return (!memcmp(s1, s2, sizeof(fcs_state)));
}
#endif

int __attribute__((pure))
fc_solve_state_compare_with_context(const void *const s1, const void *const s2,
    fcs_compare_context context GCC_UNUSED)
{
    return memcmp(s1, s2, sizeof(fcs_state));
}

#ifdef FCS_WITH_MOVES
#include "rank2str.h"
#if MAX_NUM_FREECELLS > 0
static inline void render_freecell_card(const fcs_card card,
    char *const freecell PASS_T(const bool display_10_as_t))
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
#endif

void fc_solve_state_as_string(char *output_s, const fcs_state *const state,
    const fcs_state_locs_struct *const state_locs FREECELLS_STACKS_DECKS__ARGS()
        FC_SOLVE__PASS_PARSABLE(const bool parseable_output),
    const bool canonized_order_output PASS_T(const bool display_10_as_t))
{
    typedef char one_card_buffer[4];
    char founds[MAX_NUM_DECKS * 4][10];

#define fc_solve_append_string_sprintf(...)                                    \
    output_s += sprintf(output_s, __VA_ARGS__)
#define append_char(c) *(output_s++) = (c)

    size_t stack_locs[MAX_NUM_STACKS];
#if MAX_NUM_FREECELLS > 0
    size_t freecell_locs[MAX_NUM_FREECELLS];
#endif

    if (canonized_order_output)
    {
        for (size_t i = 0; i < STACKS_NUM__VAL; i++)
        {
            stack_locs[i] = i;
        }
#if MAX_NUM_FREECELLS > 0
        for (size_t i = 0; i < FREECELLS_NUM__VAL; i++)
        {
            freecell_locs[i] = i;
        }
#endif
    }
    else
    {
        for (size_t i = 0; i < STACKS_NUM__VAL; i++)
        {
            stack_locs[(int)(state_locs->stack_locs[i])] = i;
        }
#if MAX_NUM_FREECELLS > 0
        for (size_t i = 0; i < FREECELLS_NUM__VAL; i++)
        {
            freecell_locs[(int)(state_locs->fc_locs[i])] = i;
        }
#endif
    }

    for (size_t i = 0; i < (DECKS_NUM__VAL << 2); i++)
    {
        rank2str(
            fcs_foundation_value(*state, i), founds[i] PASS_T(display_10_as_t));
        if (founds[i][0] == ' ')
            founds[i][0] = '0';
    }

#ifndef FC_SOLVE_IMPLICIT_PARSABLE_OUTPUT
    if (!parseable_output)
    {
        size_t i = 0;
#if MAX_NUM_FREECELLS > 0
        for (; i < ((FREECELLS_NUM__VAL / 4) +
                       ((FREECELLS_NUM__VAL % 4 == (0)) ? 0 : 1));
             ++i)
        {
            char dashes_s[128];
            char freecells_s[128];
            char *dashes_ptr = dashes_s;
            char *freecells_s_end = freecells_s;
            const_AUTO(lim, min(FREECELLS_NUM__VAL - i * 4, 4));
            for (size_t b = 0; b < lim; b++)
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
                    founds[i * 4], founds[i * 4 + 1], founds[i * 4 + 2],
                    founds[i * 4 + 3]);
            }
            else
            {
                fc_solve_append_string_sprintf("%s\n", freecells_s);
            }
            fc_solve_append_string_sprintf("%s\n", dashes_s);
        }
#endif
        for (; i < DECKS_NUM__VAL; i++)
        {
            fc_solve_append_string_sprintf(
                "%-16s        H-%1s C-%1s D-%1s S-%1s\n", "", founds[i * 4],
                founds[i * 4 + 1], founds[i * 4 + 2], founds[i * 4 + 3]);
        }
        append_char('\n');
        append_char('\n');

        for (size_t s = 0; s < STACKS_NUM__VAL; s++)
        {
            fc_solve_append_string_sprintf("%s", " -- ");
        }
        append_char('\n');

        size_t max_num_cards = 0;
        for (size_t s = 0; s < STACKS_NUM__VAL; ++s)
        {
            const size_t col_len = fcs_state_col_len(*state, stack_locs[s]);
            if (col_len > max_num_cards)
            {
                max_num_cards = col_len;
            }
        }

        for (size_t card_idx = 0; card_idx < max_num_cards; ++card_idx)
        {
            for (size_t s = 0; s < STACKS_NUM__VAL; s++)
            {
                const_AUTO(col, fcs_state_get_col(*state, stack_locs[s]));
                const size_t col_len = fcs_col_len(col);
                if (card_idx >= col_len)
                {
                    fc_solve_append_string_sprintf("%s", "    ");
                }
                else
                {
                    one_card_buffer stack_card_str;
                    fc_solve_card_stringify(fcs_col_get_card(col, card_idx),
                        stack_card_str PASS_T(display_10_as_t));
                    fc_solve_append_string_sprintf("%3s ", stack_card_str);
                }
            }
            append_char('\n');
        }
    }
    else
#endif
    {
        fc_solve_append_string_sprintf("%s", "Foundations:");
        for (size_t i = 0; i < (DECKS_NUM__VAL << 2); i += 4)
        {
            fc_solve_append_string_sprintf(" H-%s C-%s D-%s S-%s", founds[i],
                founds[i + 1], founds[i + 2], founds[i + 3]);
        }
#ifndef FC_SOLVE__REMOVE_TRAILING_WHITESPACE_IN_OUTPUT
        append_char(' ');
#endif

        fc_solve_append_string_sprintf("%s", "\nFreecells:");

#if MAX_NUM_FREECELLS > 0
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
#endif
        append_char('\n');

        for (size_t s = 0; s < STACKS_NUM__VAL; ++s)
        {
            const_AUTO(col, fcs_state_get_col(*state, stack_locs[s]));
            const size_t col_len = fcs_col_len(col);
            append_char(':');

            for (size_t card_idx = 0; card_idx < col_len; ++card_idx)
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
#endif
