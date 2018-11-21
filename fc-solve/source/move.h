/*
 * This file is part of Freecell Solver. It is subject to the license terms in
 * the COPYING.txt file found in the top-level directory of this distribution
 * and at http://fc-solve.shlomifish.org/docs/distro/COPYING.html . No part of
 * Freecell Solver, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the COPYING file.
 *
 * Copyright (c) 2000 Shlomi Fish
 */
// move.h - header file for the move and move stacks functions.
#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "state.h"
#include "internal_move_struct.h"
#include "indirect_buffer.h"

#ifdef FCS_WITH_MOVES
extern const fcs_internal_move fc_solve_empty_move;
#endif

#define FCS_MOVE_STACK_GROW_BY 16

#ifdef FCS_WITH_MOVES
static inline void fcs_move_stack_push(
    fcs_move_stack *const stack, const fcs_internal_move move)
{
    // If all the moves inside the stack are taken then resize the move vector
    const size_t pos = ++stack->num_moves;
    if (!(pos & (FCS_MOVE_STACK_GROW_BY - 1)))
    {
        stack->moves = SREALLOC(stack->moves, pos + FCS_MOVE_STACK_GROW_BY);
    }
    stack->moves[pos - 1] = move;
}

static inline void fcs_move_stack_params_push(fcs_move_stack *const stack,
    const int type, const int src, const int dest, const int num_cards_in_seq)
{
    fcs_internal_move temp_move;

    fcs_int_move_set_type(temp_move, type);
    fcs_int_move_set_src(temp_move, src);
    fcs_int_move_set_dest(temp_move, dest);
    fcs_int_move_set_num_cards_in_seq(temp_move, num_cards_in_seq);

    fcs_move_stack_push(stack, temp_move);
}

static inline void fcs_move_stack_non_seq_push(
    fcs_move_stack *const stack, const int type, const int src, const int dest)
{
    fcs_move_stack_params_push(stack, type, src, dest, 1);
}

static inline void fcs_push_1card_seq(
    fcs_move_stack *const stack, const int src, const int dest)
{
    fcs_move_stack_params_push(
        stack, FCS_MOVE_TYPE_STACK_TO_STACK, src, dest, 1);
}

static inline bool fc_solve_move_stack_pop(
    fcs_move_stack *const stack, fcs_internal_move *const move)
{
    if (stack->num_moves > 0)
    {
        *move = stack->moves[--stack->num_moves];
        return FALSE;
    }
    else
    {
        return TRUE;
    }
}

#define fcs_move_stack_static_destroy(stack) free((stack).moves)
#define fcs_move_stack_reset(stack) (stack)->num_moves = 0

void fc_solve_apply_move(fcs_state *const ptr_state_key,
    fcs_state_locs_struct *const locs,
    const fcs_internal_move move FREECELLS_AND_STACKS_ARGS());

/*
    The purpose of this function is to convert the moves from using
    the canonized positions of the stacks and freecells to their
    user positions.
*/

static inline fcs_move_stack fcs_move_stack__new(void)
{
    return (fcs_move_stack){.num_moves = 0,
        .moves = malloc(sizeof(fcs_move_t) * FCS_MOVE_STACK_GROW_BY)};
}

static inline void fc_solve_move_stack_swallow_stack(
    fcs_move_stack *const stack, fcs_move_stack *const src_stack)
{
    fcs_internal_move move;
    while (!fc_solve_move_stack_pop(src_stack, &move))
    {
        fcs_move_stack_push(stack, move);
    }
}

static inline void fc_solve_move_stack_normalize(fcs_move_stack *const moves,
    const fcs_state_keyval_pair *const init_state,
    fcs_state_locs_struct *const locs FREECELLS_AND_STACKS_ARGS())
{
    fcs_internal_move in_move;
    fcs_state_keyval_pair s_and_info;
    DECLARE_IND_BUF_T(indirect_stacks_buffer)
    fcs_internal_move out_move = fc_solve_empty_move;
    FCS_STATE__DUP_keyval_pair(s_and_info, *init_state);

#ifdef INDIRECT_STACK_STATES
    for (size_t i = 0; i < STACKS_NUM__VAL; i++)
    {
        fcs_copy_stack(
            s_and_info.s, s_and_info.info, i, indirect_stacks_buffer);
    }
#endif

    fcs_move_stack temp_moves = fcs_move_stack__new();

    while (!fc_solve_move_stack_pop(moves, &in_move))
    {
        fc_solve_apply_move(&(s_and_info.s), locs,
            in_move PASS_FREECELLS(freecells_num) PASS_STACKS(stacks_num));

        const int move_type = fcs_int_move_get_type(in_move);

        if (move_type == FCS_MOVE_TYPE_CANONIZE)
        {
            continue;
        }

        fcs_int_move_set_type(out_move, move_type);

        switch (move_type)
        {
        case FCS_MOVE_TYPE_STACK_TO_STACK:
        case FCS_MOVE_TYPE_STACK_TO_FREECELL:
        case FCS_MOVE_TYPE_STACK_TO_FOUNDATION:
        case FCS_MOVE_TYPE_SEQ_TO_FOUNDATION:
            fcs_int_move_set_src(
                out_move, locs->stack_locs[(int)fcs_int_move_get_src(in_move)]);
            break;
        case FCS_MOVE_TYPE_FREECELL_TO_STACK:
        case FCS_MOVE_TYPE_FREECELL_TO_FREECELL:
        case FCS_MOVE_TYPE_FREECELL_TO_FOUNDATION:
#if MAX_NUM_FREECELLS > 0
            fcs_int_move_set_src(
                out_move, locs->fc_locs[(int)fcs_int_move_get_src(in_move)]);
#endif
            break;
        default:
            break;
        }

        switch (move_type)
        {
        case FCS_MOVE_TYPE_STACK_TO_STACK:
        case FCS_MOVE_TYPE_FREECELL_TO_STACK:
            fcs_int_move_set_dest(out_move,
                locs->stack_locs[(int)fcs_int_move_get_dest(in_move)]);
            break;

        case FCS_MOVE_TYPE_STACK_TO_FREECELL:
        case FCS_MOVE_TYPE_FREECELL_TO_FREECELL:
#if MAX_NUM_FREECELLS > 0
            fcs_int_move_set_dest(
                out_move, locs->fc_locs[(int)fcs_int_move_get_dest(in_move)]);
#endif
            break;

        case FCS_MOVE_TYPE_STACK_TO_FOUNDATION:
        case FCS_MOVE_TYPE_FREECELL_TO_FOUNDATION:
        case FCS_MOVE_TYPE_SEQ_TO_FOUNDATION:
            fcs_int_move_set_dest(out_move, fcs_int_move_get_dest(in_move));
            break;
        default:
            break;
        }
        if (move_type == FCS_MOVE_TYPE_STACK_TO_STACK)
        {
            fcs_int_move_set_num_cards_in_seq(
                out_move, fcs_int_move_get_num_cards_in_seq(in_move));
        }

        fcs_move_stack_push((&temp_moves), out_move);
    }

    // temp_moves contain the needed moves in reverse order. So let's use
    // swallow_stack to put them in the original in the correct order.
    fcs_move_stack_reset(moves);
    fc_solve_move_stack_swallow_stack(moves, (&temp_moves));
    fcs_move_stack_static_destroy(temp_moves);
}

static inline int fc_solve_move__convert_freecell_num(const int fc_idx)
{
    return (fc_idx >= 7) ? (fc_idx + 3) : fc_idx;
}

static inline char fc_solve__freecell_to_char(const int fc_idx)
{
    return 'a' + fc_solve_move__convert_freecell_num(fc_idx);
}
static inline void fc_solve_move_to_string_w_state(char *const string,
    fcs_state_keyval_pair *const state, const fcs_move_t move,
    const int standard_notation)
{
#define state_key (&(state->s))
    switch (fcs_move_get_type(move))
    {
    case FCS_MOVE_TYPE_STACK_TO_STACK:
        if ((standard_notation == FC_SOLVE__STANDARD_NOTATION_EXTENDED) &&
            /* More than one card was moved */
            (fcs_move_get_num_cards_in_seq(move) > 1) &&
            /* It was a move to an empty stack */
            (fcs_state_col_len(*state_key, fcs_move_get_dest_stack(move)) ==
                fcs_move_get_num_cards_in_seq(move)))
        {
            sprintf(string, "%d%dv%x", 1 + fcs_move_get_src_stack(move),
                1 + fcs_move_get_dest_stack(move),
                fcs_move_get_num_cards_in_seq(move));
        }
        else if (standard_notation)
        {
            sprintf(string, "%d%d", 1 + fcs_move_get_src_stack(move),
                1 + fcs_move_get_dest_stack(move));
        }
        else
        {
            sprintf(string, "Move %d cards from stack %d to stack %d",
                fcs_move_get_num_cards_in_seq(move),
                fcs_move_get_src_stack(move), fcs_move_get_dest_stack(move));
        }
        break;

    case FCS_MOVE_TYPE_FREECELL_TO_STACK:
        if (standard_notation)
        {
            sprintf(string, "%c%i",
                fc_solve__freecell_to_char(fcs_move_get_src_freecell(move)),
                1 + fcs_move_get_dest_stack(move));
        }
        else
        {
            sprintf(string, "Move a card from freecell %i to stack %i",
                fcs_move_get_src_freecell(move), fcs_move_get_dest_stack(move));
        }

        break;

    case FCS_MOVE_TYPE_FREECELL_TO_FREECELL:
        if (standard_notation)
        {
            sprintf(string, "%c%c",
                fc_solve__freecell_to_char(fcs_move_get_src_freecell(move)),
                fc_solve__freecell_to_char(fcs_move_get_dest_freecell(move)));
        }
        else
        {
            sprintf(string, "Move a card from freecell %i to freecell %i",
                fcs_move_get_src_freecell(move),
                fcs_move_get_dest_freecell(move));
        }

        break;

    case FCS_MOVE_TYPE_STACK_TO_FREECELL:
        if (standard_notation)
        {
            sprintf(string, "%d%c", 1 + fcs_move_get_src_stack(move),
                fc_solve__freecell_to_char(fcs_move_get_dest_freecell(move)));
        }
        else
        {
            sprintf(string, "Move a card from stack %d to freecell %d",
                fcs_move_get_src_stack(move), fcs_move_get_dest_freecell(move));
        }

        break;

    case FCS_MOVE_TYPE_STACK_TO_FOUNDATION:
        if (standard_notation)
        {
            sprintf(string, "%dh", 1 + fcs_move_get_src_stack(move));
        }
        else
        {
            sprintf(string, "Move a card from stack %d to the foundations",
                fcs_move_get_src_stack(move));
        }

        break;

    case FCS_MOVE_TYPE_FREECELL_TO_FOUNDATION:
        if (standard_notation)
        {
            sprintf(string, "%ch",
                fc_solve__freecell_to_char(fcs_move_get_src_freecell(move)));
        }
        else
        {
            sprintf(string, "Move a card from freecell %i to the foundations",
                fcs_move_get_src_freecell(move));
        }

        break;

    case FCS_MOVE_TYPE_SEQ_TO_FOUNDATION:
        if (standard_notation)
        {
            sprintf(string, "%dh", fcs_move_get_src_stack(move));
        }
        else
        {
            sprintf(string,
                "Move the sequence on top of Stack %d to the foundations",
                fcs_move_get_src_stack(move));
        }
        break;

    default:
        string[0] = '\0';
        break;
    }
#undef state_key
}
#define FCS__pass_moves(x) , x
#else
#define fcs_move_stack_params_push(a, b, c, d, e)
#define fcs_push_1card_seq(a, b, c)
#define fcs_move_stack_non_seq_push(a, b, c, d)
#define fcs_move_stack_reset(stack)
#define fcs_move_stack_static_destroy(stack)
#define FCS__pass_moves(x)

#endif

typedef struct
{
    fcs_collectible_state *state_ptr;
    union {
        void *ptr;
        int i;
        char c[sizeof(void *) / sizeof(char)];
    } context;
} fcs_derived_states_list_item;

typedef struct
{
    size_t num_states;
    fcs_derived_states_list_item *states;
} fcs_derived_states_list;

extern void fc_solve_derived_states_list_add_state(
    fcs_derived_states_list *, fcs_collectible_state *, int);

#ifdef __cplusplus
}
#endif
