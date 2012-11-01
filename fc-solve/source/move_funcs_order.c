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
 * move_funcs_order.c - contains the fc_solve_apply_tests_order function.
 *
 */

#define BUILDING_DLL 1

#include <string.h>

#include "config.h"

#include "move_funcs_order.h"
#include "set_weights.h"

#include "bool.h"
#include "str_utils.h"
#include "inline.h"

#ifndef HAVE_STRNDUP
static GCC_INLINE char * strndup(const char * s, size_t n)
{
    char * ret = strdup(s);

    if (n < strlen(ret))
    {
        ret[n] = '\0';
    }

    return ret;
}
#endif

int fc_solve_apply_tests_order(
    fcs_tests_order_t * tests_order,
    const char * string,
    char * * error_string
    )
{
    int i;
    int len;
    fcs_bool_t is_group, is_start_group;
    char test_name[2] = {0};

    fc_solve_free_tests_order(tests_order);

    tests_order->groups = SMALLOC(tests_order->groups, TESTS_ORDER_GROW_BY);
    tests_order->groups[tests_order->num_groups].num = 0;
    tests_order->groups[tests_order->num_groups].tests =
        SMALLOC(
            tests_order->groups[tests_order->num_groups].tests,
            TESTS_ORDER_GROW_BY
        );
    tests_order->groups[tests_order->num_groups].shuffling_type = FCS_NO_SHUFFLING;

    tests_order->num_groups++;

    len = strlen(string);
    is_group = FALSE;
    is_start_group = FALSE;

    for (i = 0 ; i < len ; i++)
    {
        if ((string[i] == '(') || (string[i] == '['))
        {
            if (is_group)
            {
                *error_string = strdup("There's a nested random group.");
                return 1;
            }
            is_group = TRUE;
            is_start_group = TRUE;
            if (tests_order->groups[tests_order->num_groups - 1].num)
            {
                if (! (tests_order->num_groups & (TESTS_ORDER_GROW_BY - 1)))
                {
                    tests_order->groups =
                        SREALLOC(
                            tests_order->groups,
                            TESTS_ORDER_GROW_BY
                        );
                }

                tests_order->groups[tests_order->num_groups].num = 0;
                tests_order->groups[tests_order->num_groups].tests =
                    SMALLOC(
                        tests_order->groups[tests_order->num_groups].tests,
                        TESTS_ORDER_GROW_BY
                    );

                tests_order->num_groups++;
            }
            tests_order->groups[tests_order->num_groups-1].shuffling_type
                = FCS_RAND;
            continue;
        }

        if ((string[i] == ')') || (string[i] == ']'))
        {
            if (is_start_group)
            {
                *error_string = strdup("There's an empty group.");
                return 2;
            }
            if (! is_group)
            {
                *error_string = strdup("There's a renegade right parenthesis or bracket.");
                return 3;
            }
            /* Try to parse the ordering function. */
            if (string[i+1] == '=')
            {
                i+=2;
                const char * open_paren = strchr(string+i, '(');
                if (! open_paren)
                {
                    *error_string = strdup("A = ordering function is missing its open parenthesis - (");
                    return 5;
                }
                if (string_starts_with(string+i, "rand", open_paren))
                {
                    tests_order->groups[tests_order->num_groups-1].shuffling_type
                        = FCS_RAND;
                }
                else if (string_starts_with(string+i, "asw", open_paren))
                {
                    tests_order->groups[tests_order->num_groups-1].shuffling_type
                        = FCS_WEIGHTING;
                }
                else
                {
                    *error_string = strdup("Unknown = ordering function");
                    return 6;
                }
                const char * const aft_open_paren = open_paren + 1;
                const char * const close_paren = strchr(aft_open_paren, ')');
                if (!close_paren)
                {
                    *error_string = strdup("= ordering function not terminated with a ')'");
                    return 7;
                }
                if (tests_order->groups[tests_order->num_groups-1].shuffling_type == FCS_WEIGHTING)
                {
                    char * copy = strndup(aft_open_paren, close_paren-aft_open_paren);
                    fc_solve_set_weights(
                        copy,
                        tests_order->groups[tests_order->num_groups-1].weighting.befs_weights
                    );
                    free (copy);
                }
                else
                {
                    if (close_paren != aft_open_paren)
                    {
                        *error_string = strdup("=rand() arguments are not empty.");
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
                if (! (tests_order->num_groups & (TESTS_ORDER_GROW_BY - 1)))
                {
                    tests_order->groups =
                        SREALLOC(
                            tests_order->groups,
                            tests_order->num_groups + TESTS_ORDER_GROW_BY
                        );
                }
                tests_order->groups[tests_order->num_groups].num = 0;
                tests_order->groups[tests_order->num_groups].tests =
                    SMALLOC(
                        tests_order->groups[tests_order->num_groups].tests,
                        TESTS_ORDER_GROW_BY
                    );

                tests_order->num_groups++;
            }
            tests_order->groups[tests_order->num_groups-1].shuffling_type
                = FCS_NO_SHUFFLING;
            continue;
        }

        if (! (tests_order->groups[tests_order->num_groups-1].num & (TESTS_ORDER_GROW_BY - 1)))
        {
            tests_order->groups[tests_order->num_groups-1].tests =
                SREALLOC(
                    tests_order->groups[tests_order->num_groups-1].tests,
                    TESTS_ORDER_GROW_BY
                );
        }

        test_name[0] = string[i];
        tests_order->groups[tests_order->num_groups-1].tests[
            tests_order->groups[tests_order->num_groups-1].num++
            ] = (fc_solve_string_to_test_num(test_name)%FCS_TESTS_NUM);

        is_start_group = FALSE;
    }
    if (i != len)
    {
        *error_string = strdup("The Input string is too long.");
        return 4;
    }

    if (! tests_order->groups[tests_order->num_groups - 1].num)
    {
        tests_order->num_groups--;
        free(tests_order->groups[tests_order->num_groups].tests);
        tests_order->groups[tests_order->num_groups].tests = NULL;
    }

    *error_string = NULL;

    return 0;
}

