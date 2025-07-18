// This file is part of Freecell Solver. It is subject to the license terms in
// the COPYING.txt file found in the top-level directory of this distribution
// and at http://fc-solve.shlomifish.org/docs/distro/COPYING.html . No part of
// Freecell Solver, including this file, may be copied, modified, propagated,
// or distributed except according to the terms contained in the COPYING file.
//
// Copyright (c) 2012 Shlomi Fish
// dbm_calc_derived.h - contains functions to calculate the derived states
// from a certain position.
#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "meta_alloc.h"
#include "freecell-solver/fcs_enums.h"
#include "freecell-solver/fcs_dllexport.h"
#include "delta_states.h"
#include "dbm_calc_derived_iface.h"
#include "indirect_buffer.h"
#include "fcc_brfs_test.h"

typedef struct fcs_derived_state_struct
{
    fcs_state_keyval_pair state;
    fcs_dbm_record key_and_parent;
    struct fcs_derived_state_struct *next;
    size_t core_irreversible_moves_count;
    fcs_which_moves_bitmask which_irreversible_moves_bitmask;
    fcs_fcc_move move;
    size_t num_non_reversible_moves_including_prune;
    DECLARE_IND_BUF_T(indirect_stacks_buffer)
} fcs_derived_state;

static inline void fcs_derived_state_list__recycle(
    fcs_derived_state **const p_recycle_bin, fcs_derived_state **const p_list)
{
    var_AUTO(list, *p_list);
    var_AUTO(bin, *p_recycle_bin);
    while (list)
    {
        var_AUTO(list_next, list->next);
        list->next = bin;
        bin = list;
        list = list_next;
    }
    *p_recycle_bin = bin;
    *p_list = NULL;
}

#define MAKE_MOVE(src, dest) ((fcs_fcc_move)((src) | ((dest) << 4U)))
#define COL2MOVE(idx) (idx)
#define FREECELL2MOVE(idx) (idx + 8)
#define FOUND2MOVE(idx) ((idx) + (8 + 4))

#define CALC_SEQUENCES_ARE_BUILT_BY()                                          \
    ((local_variant == FCS_DBM_VARIANT_BAKERS_DOZEN)                           \
            ? FCS_SEQ_BUILT_BY_RANK                                            \
            : FCS_SEQ_BUILT_BY_ALTERNATE_COLOR)

#ifdef INDIRECT_STACK_STATES

#define COPY_INDIRECT_COLS()                                                   \
    for (int copy_col_idx = 0; copy_col_idx < LOCAL_STACKS_NUM;                \
        copy_col_idx++)                                                        \
    {                                                                          \
        const_AUTO(copy_stack_col,                                             \
            fcs_state_get_col((ptr_new_state->state.s), copy_col_idx));        \
        memcpy(&(ptr_new_state->indirect_stacks_buffer[copy_col_idx << 6]),    \
            copy_stack_col, (size_t)(fcs_col_len(copy_stack_col)) + 1);        \
        fcs_state_get_col((ptr_new_state->state.s), copy_col_idx) =            \
            &(ptr_new_state->indirect_stacks_buffer[copy_col_idx << 6]);       \
    }

#else
#define COPY_INDIRECT_COLS()
#endif

#define BEGIN_NEW_STATE()                                                      \
    {                                                                          \
        if (*derived_list_recycle_bin)                                         \
        {                                                                      \
            (*derived_list_recycle_bin) =                                      \
                (ptr_new_state = (*derived_list_recycle_bin))->next;           \
        }                                                                      \
        else                                                                   \
        {                                                                      \
            ptr_new_state = (fcs_derived_state *)fcs_compact_alloc_ptr(        \
                derived_list_allocator, sizeof(*ptr_new_state));               \
        }                                                                      \
        memset(&(ptr_new_state->which_irreversible_moves_bitmask), '\0',       \
            sizeof(ptr_new_state->which_irreversible_moves_bitmask));          \
        fcs_duplicate_state(&(ptr_new_state->state), init_state_kv_ptr);       \
                                                                               \
        COPY_INDIRECT_COLS()                                                   \
    }

static inline void fc_solve_add_to_irrev_moves_bitmask(
    fcs_which_moves_bitmask *const which_irreversible_moves_bitmask,
    const fcs_card moved_card, const uint_fast32_t count)
{
    unsigned char *const by_rank_ptr =
        which_irreversible_moves_bitmask->s + fcs_card_rank(moved_card) - 1;
    const size_t suit_times_two = (fcs_card_suit(moved_card) << 1);
    const size_t new_count =
        ((((*by_rank_ptr) >> (suit_times_two)) & ((1U << 2U) - 1U)) + count);
    *by_rank_ptr &= (~((((unsigned char)0x3) << (suit_times_two))));
    *by_rank_ptr |= (new_count << (suit_times_two));
}

#define COMMIT_NEW_STATE_WITH_COUNT(src, dest, count_init, moved_card)         \
    {                                                                          \
        const size_t count_constant = (count_init);                            \
        if (count_constant)                                                    \
        {                                                                      \
            fc_solve_add_to_irrev_moves_bitmask(                               \
                &(ptr_new_state->which_irreversible_moves_bitmask),            \
                moved_card, count_constant);                                   \
        }                                                                      \
        if (parent_ptr)                                                        \
        {                                                                      \
            ptr_new_state->key_and_parent.parent = parent_ptr->key;            \
        }                                                                      \
        else                                                                   \
        {                                                                      \
            fcs_init_encoded_state(&ptr_new_state->key_and_parent.parent);     \
        }                                                                      \
        ptr_new_state->move = MAKE_MOVE((src), (dest));                        \
                                                                               \
        ptr_new_state->core_irreversible_moves_count = (count_constant);       \
        ptr_new_state->next = (*derived_list);                                 \
        (*derived_list) = ptr_new_state;                                       \
    }

#define COMMIT_NEW_STATE(src, dest, is_reversible, moved_card)                 \
    COMMIT_NEW_STATE_WITH_COUNT(                                               \
        src, dest, ((is_reversible) ? 0 : 1), moved_card)

#define CALC_FOUNDATION_ARG()                                                  \
    const fcs_dbm_variant_type local_variant GCC_UNUSED
#define CALC_FOUNDATION__calc_sequences_are_built_by()                         \
    FCS_ON_NOT_FC_ONLY(                                                        \
        const int sequences_are_built_by = CALC_SEQUENCES_ARE_BUILT_BY())
#include "calc_foundation.h"

#define FROM_COL_IS_REVERSIBLE_MOVE()                                          \
    ((cards_num <= 1)                                                          \
            ? true                                                             \
            : fcs_is_parent_card(card, fcs_col_get_card(col, cards_num - 2)))
#define COUNT_NON_REV(is_reversible) ((is_reversible) ? 1 : 2)

// Returns the number of amortized irreversible moves performed.
static inline size_t horne_prune(const fcs_dbm_variant_type local_variant,
    fcs_state_keyval_pair *const init_state_kv_ptr,
    fcs_which_moves_bitmask *const which_irreversible_moves_bitmask)
{
    fcs_fcc_move additional_moves[RANK_KING * 4 * DECKS_NUM];
    size_t count_moves_so_far = 0;
    size_t count_additional_irrev_moves = 0;
    FCS_ON_NOT_FC_ONLY(
        const int sequences_are_built_by = CALC_SEQUENCES_ARE_BUILT_BY());

#define the_state (init_state_kv_ptr->s)
    uint_fast32_t num_cards_moved;
    do
    {
        num_cards_moved = 0;
        for (stack_i stack_idx = 0; stack_idx < LOCAL_STACKS_NUM; stack_idx++)
        {
            var_AUTO(col, fcs_state_get_col(the_state, stack_idx));
            const int cards_num = fcs_col_len(col);
            if (!cards_num)
            {
                continue;
            }
            const fcs_card card = fcs_col_get_card(col, cards_num - 1);
            const_AUTO(dest_foundation, calc_foundation_to_put_card_on(
                                            local_variant, &the_state, card));
            if (dest_foundation >= 0)
            {
                const_AUTO(is_reversible, FROM_COL_IS_REVERSIBLE_MOVE());
                if (!is_reversible)
                {
                    count_additional_irrev_moves++;
                }
                // We can safely move it.
                ++num_cards_moved;
                fc_solve_add_to_irrev_moves_bitmask(
                    which_irreversible_moves_bitmask, card,
                    COUNT_NON_REV(is_reversible));

                fcs_col_pop_top(col);

                fcs_increment_foundation(the_state, dest_foundation);

                additional_moves[count_moves_so_far++] = MAKE_MOVE(
                    COL2MOVE(stack_idx), FOUND2MOVE((stack_i)dest_foundation));
            }
        }

#if MAX_NUM_FREECELLS > 0
        // Now check the same for the free cells
        for (stack_i fc = 0; fc < LOCAL_FREECELLS_NUM; fc++)
        {
            const fcs_card card = fcs_freecell_card(the_state, fc);
            if (fcs_card_is_valid(card))
            {
                const_AUTO(
                    dest_foundation, calc_foundation_to_put_card_on(
                                         local_variant, &the_state, card));
                if (dest_foundation >= 0)
                {
                    ++num_cards_moved;
                    fc_solve_add_to_irrev_moves_bitmask(
                        which_irreversible_moves_bitmask, card, 1);

                    fcs_empty_freecell(the_state, fc);
                    fcs_increment_foundation(the_state, dest_foundation);
                    additional_moves[count_moves_so_far++] = MAKE_MOVE(
                        COL2MOVE(fc), FOUND2MOVE((stack_i)dest_foundation));
                }
            }
        }
#endif
    } while (num_cards_moved);

    return count_moves_so_far + count_additional_irrev_moves;
}

static inline size_t horne_prune__simple(
    const fcs_dbm_variant_type local_variant,
    fcs_state_keyval_pair *const init_state_kv_ptr)
{
    fcs_which_moves_bitmask no_use = {{'\0'}};
    return horne_prune(local_variant, init_state_kv_ptr, &no_use);
}

static inline bool card_cannot_be_placed(const fcs_state *const s,
    const stack_i ds, const fcs_card card,
    const int sequences_are_built_by GCC_UNUSED)
{
    const_AUTO(col, fcs_state_get_col(*s, ds));
    const_AUTO(col_len, fcs_col_len(col));
    return ((col_len == 0) ||
            (!fcs_is_parent_card(card, fcs_col_get_card(col, col_len - 1))));
}

#define the_state (init_state_kv_ptr->s)
static inline bool is_state_solved(
    fcs_state_keyval_pair *const init_state_kv_ptr)
{
    for (stack_i suit = 0; suit < DECKS_NUM * 4; suit++)
    {
        if (fcs_foundation_value(the_state, suit) < RANK_KING)
        {
            return false;
        }
    }
    return true;
}

static inline bool instance_solver_thread_calc_derived_states(
    const fcs_dbm_variant_type local_variant,
    fcs_state_keyval_pair *const init_state_kv_ptr,
    fcs_dbm_store_val parent_ptr, fcs_derived_state **const derived_list,
    fcs_derived_state **const derived_list_recycle_bin,
    compact_allocator *const derived_list_allocator,
    const bool perform_horne_prune)
{
    fcs_derived_state *ptr_new_state;
    bool has_empty_stack = false;
    stack_i empty_stack_idx = 0;
    const int sequences_are_built_by = CALC_SEQUENCES_ARE_BUILT_BY();
#define new_state (ptr_new_state->state.s)
    if (is_state_solved(init_state_kv_ptr))
    {
        return true;
    }

    // Move top stack cards to foundations.
    for (stack_i stack_idx = 0; stack_idx < LOCAL_STACKS_NUM; stack_idx++)
    {
        const_AUTO(col, fcs_state_get_col(the_state, stack_idx));
        const_AUTO(cards_num, fcs_col_len(col));
        if (cards_num == 0)
        {
            empty_stack_idx = stack_idx;
            has_empty_stack = true;
            continue;
        }
        // Get the top card in the stack
        const_AUTO(card, fcs_col_get_card(col, cards_num - 1));
        const_AUTO(suit, fcs_card_suit(card));
        for (stack_i deck = 0; deck < INSTANCE_DECKS_NUM; deck++)
        {
            if (fcs_foundation_value(the_state, deck * 4 + suit) !=
                fcs_card_rank(card) - 1)
            {
                continue;
            }
            BEGIN_NEW_STATE()

            fcs_state_pop_col_top(&new_state, stack_idx);
            fcs_increment_foundation(new_state, deck * 4 + suit);

            COMMIT_NEW_STATE_WITH_COUNT(COL2MOVE(stack_idx), FOUND2MOVE(suit),
                COUNT_NON_REV(FROM_COL_IS_REVERSIBLE_MOVE()), card)
            break;
        }
    }

#if MAX_NUM_FREECELLS > 0
    int empty_fc_idx = -1;
    // Move freecell cards to foundations.
    for (stack_i fc_idx = 0; fc_idx < LOCAL_FREECELLS_NUM; ++fc_idx)
    {
        const_AUTO(card, fcs_freecell_card(the_state, fc_idx));
        if (fcs_card_is_empty(card))
        {
            empty_fc_idx = (int)fc_idx;
            continue;
        }
        const_AUTO(suit, fcs_card_suit(card));
        for (stack_i deck = 0; deck < INSTANCE_DECKS_NUM; ++deck)
        {
            if (fcs_foundation_value(the_state, deck * 4 + suit) !=
                fcs_card_rank(card) - 1)
            {
                continue;
            }
            BEGIN_NEW_STATE()

            fcs_empty_freecell(new_state, fc_idx);
            fcs_increment_foundation(new_state, deck * 4 + suit);

            COMMIT_NEW_STATE(
                FREECELL2MOVE(fc_idx), FOUND2MOVE(suit), false, card)
            break;
        }
    }
#endif

    const int cards_num_min_limit =
        ((local_variant == FCS_DBM_VARIANT_BAKERS_DOZEN) ? 1 : 0);

    // Move stack card on top of a parent
    for (stack_i stack_idx = 0; stack_idx < LOCAL_STACKS_NUM; stack_idx++)
    {
        const_AUTO(col, fcs_state_get_col(the_state, stack_idx));
        const_AUTO(cards_num, fcs_col_len(col));
        if (cards_num <= cards_num_min_limit)
        {
            continue;
        }
        const_AUTO(card, fcs_col_get_card(col, cards_num - 1));

        for (stack_i ds = 0; ds < LOCAL_STACKS_NUM; ++ds)
        {
            if (ds == stack_idx || card_cannot_be_placed(&the_state, ds, card,
                                       sequences_are_built_by))
            {
                continue;
            }
            BEGIN_NEW_STATE()

            fcs_state_pop_col_top(&new_state, stack_idx);
            fcs_state_push(&new_state, ds, card);

            COMMIT_NEW_STATE(COL2MOVE(stack_idx), COL2MOVE(ds),
                FROM_COL_IS_REVERSIBLE_MOVE(), card)
        }
    }

#if MAX_NUM_FREECELLS > 0
    // Move freecell card on top of a parent
    for (stack_i fc_idx = 0; fc_idx < LOCAL_FREECELLS_NUM; fc_idx++)
    {
        const_AUTO(card, fcs_freecell_card(the_state, fc_idx));
        if (!fcs_card_is_valid(card))
        {
            continue;
        }
        for (stack_i ds = 0; ds < LOCAL_STACKS_NUM; ++ds)
        {
            if (card_cannot_be_placed(
                    &the_state, ds, card, sequences_are_built_by))
            {
                continue;
            }
            BEGIN_NEW_STATE()

            fcs_state_push(&new_state, ds, card);
            fcs_empty_freecell(new_state, fc_idx);

            COMMIT_NEW_STATE(FREECELL2MOVE(fc_idx), COL2MOVE(ds), true, card)
        }
    }
#endif

    if ((local_variant != FCS_DBM_VARIANT_BAKERS_DOZEN) && has_empty_stack)
    {
        // Stack Card to Empty Stack
        for (stack_i stack_idx = 0; stack_idx < LOCAL_STACKS_NUM; stack_idx++)
        {
            const_AUTO(col, fcs_state_get_col(the_state, stack_idx));
            const_AUTO(cards_num, fcs_col_len(col));
            // Bug fix: if there's only one card in a column, there's no
            // point moving it to a new empty column.
            if (cards_num <= 1)
            {
                continue;
            }

            const_AUTO(card, fcs_col_get_card(col, cards_num - 1));
            BEGIN_NEW_STATE()

            fcs_state_pop_col_top(&new_state, stack_idx);
            fcs_state_push(&new_state, empty_stack_idx, card);

            COMMIT_NEW_STATE(COL2MOVE(stack_idx), COL2MOVE(empty_stack_idx),
                FROM_COL_IS_REVERSIBLE_MOVE(), card)
        }

#if MAX_NUM_FREECELLS > 0
        // Freecell card -> Empty Stack.
        for (stack_i fc_idx = 0; fc_idx < LOCAL_FREECELLS_NUM; fc_idx++)
        {
            const_AUTO(card, fcs_freecell_card(the_state, fc_idx));
            if (fcs_card_is_empty(card))
            {
                continue;
            }

            BEGIN_NEW_STATE()

            fcs_state_push(&new_state, empty_stack_idx, card);
            fcs_empty_freecell(new_state, fc_idx);

            COMMIT_NEW_STATE(
                FREECELL2MOVE(fc_idx), COL2MOVE(empty_stack_idx), true, card);
        }
#endif
    }

#if MAX_NUM_FREECELLS > 0
    if (empty_fc_idx >= 0)
    {
        // Stack Card to Empty Freecell
        for (stack_i stack_idx = 0; stack_idx < LOCAL_STACKS_NUM; stack_idx++)
        {
            const_AUTO(col, fcs_state_get_col(the_state, stack_idx));
            const_AUTO(cards_num, fcs_col_len(col));
            if (cards_num <= cards_num_min_limit)
            {
                continue;
            }
            const_AUTO(card, fcs_col_get_card(col, cards_num - 1));
            BEGIN_NEW_STATE()

            fcs_state_pop_col_top(&new_state, stack_idx);
            fcs_put_card_in_freecell(new_state, empty_fc_idx, card);

            COMMIT_NEW_STATE(COL2MOVE(stack_idx),
                FREECELL2MOVE((stack_i)empty_fc_idx),
                FROM_COL_IS_REVERSIBLE_MOVE(), card)
        }
    }
#endif
    // Perform Horne's Prune on all the states,
    // or just set their num irreversible moves counts.
    for (var_AUTO(derived_iter, *derived_list); derived_iter;
        derived_iter = derived_iter->next)
    {
        derived_iter->num_non_reversible_moves_including_prune =
            (derived_iter->core_irreversible_moves_count +
                (perform_horne_prune
                        ? horne_prune(local_variant, &(derived_iter->state),
                              &(derived_iter->which_irreversible_moves_bitmask))
                        : 0));
    }

    return false;
}

#undef FROM_COL_IS_REVERSIBLE_MOVE
#undef COUNT_NON_REV
#undef the_state
#undef new_state

#ifdef __cplusplus
}
#endif
