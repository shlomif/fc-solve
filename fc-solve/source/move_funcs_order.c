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
 * move_funcs_order.c - contains the fc_solve_apply_tests_order function.
 */
#include "dll_thunk.h"
#include "fcs_back_compat.h"
#include "move_funcs_order.h"
#include "set_weights.h"

#ifdef FCS_WITH_ERROR_STRS
#define SET_ERR(s) strcpy(error_string, s);
#else
#define SET_ERR(s)
#endif

int fc_solve_apply_tests_order(fcs_tests_order_t *const tests_order,
    const char *string FCS__PASS_ERR_STR(char *const error_string))
{
    int i;
    fc_solve_free_tests_order(tests_order);
    tests_order->groups = SMALLOC(tests_order->groups, TESTS_ORDER_GROW_BY);
    tests_order->groups[tests_order->num_groups].num = 0;
    tests_order->groups[tests_order->num_groups].order_group_tests =
        SMALLOC(tests_order->groups[tests_order->num_groups].order_group_tests,
            TESTS_ORDER_GROW_BY);
    tests_order->groups[tests_order->num_groups].shuffling_type =
        FCS_NO_SHUFFLING;

    tests_order->num_groups++;

    const_AUTO(len, strlen(string));
    fcs_bool_t is_group = FALSE;
    fcs_bool_t is_start_group = FALSE;

    for (i = 0; i < len; i++)
    {
        if ((string[i] == '(') || (string[i] == '['))
        {
            if (is_group)
            {
                SET_ERR("There's a nested random group.");
                return 1;
            }
            is_group = TRUE;
            is_start_group = TRUE;
            if (tests_order->groups[tests_order->num_groups - 1].num)
            {
                if (!(tests_order->num_groups & (TESTS_ORDER_GROW_BY - 1)))
                {
                    tests_order->groups = SREALLOC(tests_order->groups,
                        tests_order->num_groups + TESTS_ORDER_GROW_BY);
                }

                tests_order->groups[tests_order->num_groups].num = 0;
                tests_order->groups[tests_order->num_groups].order_group_tests =
                    SMALLOC(tests_order->groups[tests_order->num_groups]
                                .order_group_tests,
                        TESTS_ORDER_GROW_BY);

                tests_order->num_groups++;
            }
            tests_order->groups[tests_order->num_groups - 1].shuffling_type =
                FCS_RAND;
            continue;
        }

        if ((string[i] == ')') || (string[i] == ']'))
        {
            if (is_start_group)
            {
                SET_ERR("There's an empty group.");
                return 2;
            }
            if (!is_group)
            {
                SET_ERR("There's a renegade right parenthesis or bracket.");
                return 3;
            }
            /* Try to parse the ordering function. */
            if (string[i + 1] == '=')
            {
                i += 2;
                const char *const open_paren = strchr(string + i, '(');
                if (!open_paren)
                {
                    SET_ERR("A = ordering function is missing its "
                            "open parenthesis - (");
                    return 5;
                }
                if (string_starts_with(string + i, "rand", open_paren))
                {
                    tests_order->groups[tests_order->num_groups - 1]
                        .shuffling_type = FCS_RAND;
                }
                else if (string_starts_with(string + i, "asw", open_paren))
                {
                    tests_order->groups[tests_order->num_groups - 1]
                        .shuffling_type = FCS_WEIGHTING;
                }
                else
                {
                    SET_ERR("Unknown = ordering function");
                    return 6;
                }
                const char *const aft_open_paren = open_paren + 1;
                const char *const close_paren = strchr(aft_open_paren, ')');
                if (!close_paren)
                {
                    SET_ERR("= ordering function not terminated with a ')'");
                    return 7;
                }
                if (tests_order->groups[tests_order->num_groups - 1]
                        .shuffling_type == FCS_WEIGHTING)
                {
                    fc_solve_set_weights(aft_open_paren, close_paren,
                        tests_order->groups[tests_order->num_groups - 1]
                            .weighting.befs_weights.weights);
                }
                else
                {
                    if (close_paren != aft_open_paren)
                    {
                        SET_ERR("=rand() arguments are not empty.");
                        return 8;
                    }
                }
                /*
                 * a will be incremented so it should be -1 here -
                 * at the end of the token/expression.
                 *
                 * */
                i = close_paren - string;
            }
            is_group = FALSE;
            is_start_group = FALSE;

            if (tests_order->groups[tests_order->num_groups - 1].num)
            {
                if (!(tests_order->num_groups & (TESTS_ORDER_GROW_BY - 1)))
                {
                    tests_order->groups = SREALLOC(tests_order->groups,
                        tests_order->num_groups + TESTS_ORDER_GROW_BY);
                }
                tests_order->groups[tests_order->num_groups].num = 0;
                tests_order->groups[tests_order->num_groups].order_group_tests =
                    SMALLOC(tests_order->groups[tests_order->num_groups]
                                .order_group_tests,
                        TESTS_ORDER_GROW_BY);

                tests_order->num_groups++;
            }
            tests_order->groups[tests_order->num_groups - 1].shuffling_type =
                FCS_NO_SHUFFLING;
            continue;
        }

        if (!(tests_order->groups[tests_order->num_groups - 1].num &
                (TESTS_ORDER_GROW_BY - 1)))
        {
            tests_order->groups[tests_order->num_groups - 1].order_group_tests =
                SREALLOC(tests_order->groups[tests_order->num_groups - 1]
                             .order_group_tests,
                    tests_order->groups[tests_order->num_groups - 1].num +
                        TESTS_ORDER_GROW_BY);
        }

        const char test_name[2] = {string[i], '\0'};
        tests_order->groups[tests_order->num_groups - 1].order_group_tests
            [tests_order->groups[tests_order->num_groups - 1].num++] =
            fc_solve_string_to_test_num(test_name);
        is_start_group = FALSE;
    }
    if (i != len)
    {
        SET_ERR("The Input string is too long.");
        return 4;
    }

    if (!tests_order->groups[tests_order->num_groups - 1].num)
    {
        tests_order->num_groups--;
        free(tests_order->groups[tests_order->num_groups].order_group_tests);
        tests_order->groups[tests_order->num_groups].order_group_tests = NULL;
    }

#ifdef FCS_WITH_ERROR_STRS
    error_string[0] = '\0';
#endif

    return 0;
}
