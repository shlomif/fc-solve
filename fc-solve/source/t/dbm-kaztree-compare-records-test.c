/*
 * This file is part of Freecell Solver. It is subject to the license terms in
 * the COPYING.txt file found in the top-level directory of this distribution
 * and at http://fc-solve.shlomifish.org/docs/distro/COPYING.html . No part of
 * Freecell Solver, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the COPYING file.
 *
 * Copyright (c) 2011 Shlomi Fish
 */

/*
 * A test for the DBM kaztree compare records.
 */

#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>

#ifndef FCS_COMPILE_DEBUG_FUNCTIONS
#define FCS_COMPILE_DEBUG_FUNCTIONS
#endif

#include "dbm_kaztree_compare.h"

static void main_tests(void **state)
{
    {
        bool all_good = true;

        for (size_t pos_idx = 1; pos_idx < sizeof(fcs_encoded_state_buffer);
             pos_idx++)
        {
            fcs_dbm_record rec_a, rec_b;

            memset(&rec_a, '\0', sizeof(rec_a));
            memset(&rec_b, '\0', sizeof(rec_a));

#ifdef FCS_DBM_RECORD_POINTER_REPR
            rec_a.key_and_move_to_parent.s[0] =
                sizeof(rec_a.key_and_move_to_parent) - 1;
            rec_a.key_and_move_to_parent.s[pos_idx] = '\x01';
#else
            rec_a.key.s[0] = sizeof(rec_a.key) - 1;
            rec_a.key.s[pos_idx] = '\x01';
#endif

            if (!(compare_records__noctx(&rec_a, &rec_b) > 0))
            {
                fail_msg(
                    "compare_records(rec_a, rec_b) returned a wrong value for "
                    "position %zu.\n",
                    pos_idx);
                all_good = false;
                break;
            }
            if (!(compare_records__noctx(&rec_b, &rec_a) < 0))
            {
                fail_msg(
                    "compare_records(rec_b, rec_a) returned a wrong value for "
                    "position %zu.\n",
                    pos_idx);
                all_good = false;
                break;
            }
        }

        /* TEST
         * */
        assert_true(all_good); // "All compare_records were successful.");
    }
}

int main(void)
{
    // plan([% num_tests %]);
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(main_tests),
    };
    return cmocka_run_group_tests(tests, NULL, NULL);
}
