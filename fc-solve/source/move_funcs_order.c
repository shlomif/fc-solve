/*
 * This file is part of Freecell Solver. It is subject to the license terms in
 * the COPYING.txt file found in the top-level directory of this distribution
 * and at http://fc-solve.shlomifish.org/docs/distro/COPYING.html . No part of
 * Freecell Solver, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the COPYING file.
 *
 * Copyright (c) 2000 Shlomi Fish
 */
// move_funcs_order.c - contains the fc_solve_apply_moves_order function.
#include "freecell-solver/fcs_back_compat.h"
#include "move_funcs_order.h"
#include "set_weights.h"

static int parse_group(fcs_moves_group *const moves_order,
    const char **ptr_s FCS__PASS_ERR_STR(char *const error_string))
{
    int i;
    moves_order->shuffling_type = FCS_RAND;
    moves_order->m.move_funcs =
        SMALLOC(moves_order->m.move_funcs, MOVES_GROW_BY);
    moves_order->num = 0;

    const_AUTO(string, *ptr_s);
    const_AUTO(len, strlen(string));
#ifndef FCS_UNSAFE
#ifdef FCS_WITH_ERROR_STRS
#define SET_ERR(s) strcpy(error_string, s);
#else
#define SET_ERR(s)
#endif
#endif

    for (i = 0; i < len; i++)
    {
        if ((string[i] == '(') || (string[i] == '['))
        {
            const char *new_s = string + i + 1;
            const_AUTO(
                ret, parse_group(&moves_order->m.move_funcs[moves_order->num++],
                         &new_s FCS__PASS_ERR_STR(error_string)));
            if (ret)
            {
                return ret;
            }
            i = new_s - string;
            if (!(moves_order->num & (MOVES_GROW_BY - 1)))
            {
                moves_order->m.move_funcs = SREALLOC(moves_order->m.move_funcs,
                    moves_order->num + MOVES_GROW_BY);
            }

            continue;
        }

        if ((string[i] == ')') || (string[i] == ']'))
        {
#ifndef FCS_UNSAFE
            if (!i)
            {
                SET_ERR("There's an empty group.");
                return 2;
            }
#endif
            /* Try to parse the ordering function. */
            if (string[i + 1] == '=')
            {
                i += 2;
                const char *const open_paren = strchr(string + i, '(');
#ifndef FCS_UNSAFE
                if (!open_paren)
                {
                    SET_ERR("A = ordering function is missing its "
                            "open parenthesis - (");
                    return 5;
                }
#endif
                if (string_starts_with(string + i, "rand", open_paren))
                {
                    moves_order->shuffling_type = FCS_RAND;
                }
                else if (string_starts_with(string + i, "asw", open_paren))
                {
                    moves_order->shuffling_type = FCS_WEIGHTING;
                }
                else if (string_starts_with(string + i, "seq", open_paren))
                {
                    moves_order->shuffling_type = FCS_MOVE_KIND_SEQ;
                }
#ifndef FCS_UNSAFE
                else
                {
                    SET_ERR("Unknown = ordering function");
                    return 6;
                }
#endif
                const char *const aft_open_paren = open_paren + 1;
                const char *const close_paren = strchr(aft_open_paren, ')');
#ifndef FCS_UNSAFE
                if (!close_paren)
                {
                    SET_ERR("= ordering function not terminated with a ')'");
                    return 7;
                }
#endif
                if (moves_order->shuffling_type == FCS_WEIGHTING)
                {
                    fc_solve_set_weights(aft_open_paren, close_paren,
                        moves_order->weighting.befs_weights.weights);
                }
#ifndef FCS_UNSAFE
                else
                {
                    if (close_paren != aft_open_paren)
                    {
                        SET_ERR((moves_order->shuffling_type == FCS_RAND)
                                    ? "=rand() arguments are not empty."
                                    : "=seq() arguments are not empty.");
                        return 8;
                    }
                }
#endif
                /*
                 * a will be incremented so it should be -1 here -
                 * at the end of the token/expression.
                 *
                 * */
                i = close_paren - string;
            }
            *ptr_s = string + i;
            return 0;
        }

        if (!(++moves_order->num & (MOVES_GROW_BY - 1)))
        {
            moves_order->m.move_funcs = SREALLOC(
                moves_order->m.move_funcs, moves_order->num + MOVES_GROW_BY);
        }
        moves_order->m.move_funcs[moves_order->num - 1] =
            (fcs_moves_group){.m.idx = fc_solve_string_to_move_num(string[i]),
                .shuffling_type = FCS_SINGLE};
    }
#ifndef FCS_UNSAFE
    *ptr_s = string + i;
    SET_ERR("Unclosed group.");
#endif
    return 4;
}

int fc_solve_apply_moves_order(fcs_moves_group *const moves_order,
    const char *string FCS__PASS_ERR_STR(char *const error_string))
{
    int i;
    moves_order__free(moves_order);
    moves_order->num = 0;
    moves_order->shuffling_type = FCS_NO_SHUFFLING;
    moves_order->m.move_funcs =
        SMALLOC(moves_order->m.move_funcs, MOVES_GROW_BY);

    const_AUTO(len, strlen(string));
    for (i = 0; i < len; i++)
    {
        if ((string[i] == '(') || (string[i] == '['))
        {
            const char *new_s = string + i + 1;
            const_AUTO(
                ret, parse_group(&moves_order->m.move_funcs[moves_order->num++],
                         &new_s FCS__PASS_ERR_STR(error_string)));
            if (ret)
            {
                return ret;
            }
            i = new_s - string;
            if (!(moves_order->num & (MOVES_GROW_BY - 1)))
            {
                moves_order->m.move_funcs = SREALLOC(moves_order->m.move_funcs,
                    moves_order->num + MOVES_GROW_BY);
            }
            continue;
        }

#ifndef FCS_UNSAFE
        if ((string[i] == ')') || (string[i] == ']'))
        {
            SET_ERR("There's an empty group.");
            return 2;
        }
#endif

        if (!(++moves_order->num & (MOVES_GROW_BY - 1)))
        {
            moves_order->m.move_funcs = SREALLOC(
                moves_order->m.move_funcs, moves_order->num + MOVES_GROW_BY);
        }
        moves_order->m.move_funcs[moves_order->num - 1] =
            (fcs_moves_group){.m.idx = fc_solve_string_to_move_num(string[i]),
                .shuffling_type = FCS_SINGLE,
                .num = 1};
    }
#ifndef FCS_UNSAFE
    if (i != len)
    {
        SET_ERR("The Input string is too long.");
        return 4;
    }
#endif

#ifdef FCS_WITH_ERROR_STRS
    error_string[0] = '\0';
#endif

    return 0;
}
