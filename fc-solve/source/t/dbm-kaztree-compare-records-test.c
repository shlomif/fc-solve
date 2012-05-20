/* Copyright (c) 2011 Shlomi Fish
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
 * A test for the delta states routines.
 */

#include <string.h>
#include <stdio.h>

#include <tap.h>

#define FCS_COMPILE_DEBUG_FUNCTIONS
#include "../dbm_kaztree_compare.h"

int main_tests()
{
    {
        int pos_idx;
        int all_good = TRUE;

        for (pos_idx = 0 ; pos_idx < sizeof( fcs_encoded_state_buffer_t ) ; pos_idx++)
        {
            fcs_dbm_record_t rec_a, rec_b;

            memset(&rec_a, '\0', sizeof(rec_a));
            memset(&rec_b, '\0', sizeof(rec_a));

            rec_a.key.s[pos_idx] = '\x01';
            
            if (! (compare_records(&rec_a, &rec_b, NULL) > 0))
            {
                diag("compare_records(rec_a, rec_b) returned a wrong value for position %d.\n", pos_idx);
                all_good = FALSE;
                break;
            }
            if (! (compare_records(&rec_b, &rec_a, NULL) < 0))
            {
                diag("compare_records(rec_b, rec_a) returned a wrong value for position %d.\n", pos_idx);
                all_good = FALSE;
                break;
            }
        }

        /* TEST
         * */
        ok (all_good, "All compare_records were successful.");
    }
    return 0;
}

int main(int argc, char * argv[])
{
    plan_tests(1);
    main_tests();
    return exit_status();
}
