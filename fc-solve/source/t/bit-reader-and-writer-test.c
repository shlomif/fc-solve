/*
 * This file is part of Freecell Solver. It is subject to the license terms in
 * the COPYING.txt file found in the top-level directory of this distribution
 * and at http://fc-solve.shlomifish.org/docs/distro/COPYING.html . No part of
 * Freecell Solver, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the COPYING file.
 *
 * Copyright (c) 2011 Shlomi Fish
 */
// A test for the bit reader and writer.
#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>
#include <string.h>
#include <stdio.h>
#include "freecell-solver/fcs_conf.h"
#include "rinutils/unused.h"
#include "bit_rw.h"

static void main_tests(void **state GCC_UNUSED)
{
    {
        unsigned char buffer[10];
        fc_solve_bit_writer writer;

        fc_solve_bit_writer_init(&writer, buffer);

        fc_solve_bit_writer_write(&writer, 4, 5);
        fc_solve_bit_writer_write(&writer, 2, 1);

        /* TEST
         * */
        assert_int_equal(buffer[0], (5 | (1 << 4))); // "Write works."

        fc_solve_bit_writer_write(&writer, 4, (2 | (3 << 2)));

        /* TEST
         * */
        assert_int_equal(
            buffer[0], (5 | (1 << 4) | (2 << 6))); // "Extra write works."

        /* TEST
         * */
        assert_int_equal(buffer[1], 3); // "Extra byte write works."

        {
            fcs_bit_reader reader;

            fc_solve_bit_reader_init(&reader, buffer);

            /* TEST
             * */
            assert_int_equal(
                fc_solve_bit_reader_read(&reader, 4), 5); // "reader 1"

            /* TEST
             * */
            assert_int_equal(
                fc_solve_bit_reader_read(&reader, 2), 1); // "reader 2"

            /* TEST
             * */
            assert_int_equal(fc_solve_bit_reader_read(&reader, 4),
                (2 | (3 << 2))); // "reader 3"
        }
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
