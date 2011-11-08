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
 * A test for the bit reader and writer.
 */

#include <string.h>
#include <stdio.h>

#include <tap.h>
#include "../bit_rw.h"

int main_tests()
{
    {
        unsigned char buffer[10];
        fc_solve_bit_writer_t writer;

        fc_solve_bit_writer_init(&writer, buffer);

        fc_solve_bit_writer_write(&writer, 4, 5);
        fc_solve_bit_writer_write(&writer, 2, 1);

        /* TEST
         * */
        ok (buffer[0] == (5 | (1 << 4)), "Write works.");

        fc_solve_bit_writer_write(&writer, 4, (2 | (3 << 2)));

        /* TEST
         * */
        ok (buffer[0] == (5 | (1 << 4) | (2 << 6)), "Extra write works.");

        /* TEST
         * */
        ok (buffer[1] == 3, "Extra byte write works.");

        {
            fc_solve_bit_reader_t reader;

            fc_solve_bit_reader_init(&reader, buffer);

            /* TEST
             * */
            ok (fc_solve_bit_reader_read(&reader, 4) == 5,
                    "reader 1");

            /* TEST 
             * */
            ok (fc_solve_bit_reader_read(&reader, 2) == 1,
                    "reader 2");

            /* TEST
             * */
            ok (fc_solve_bit_reader_read(&reader, 4) == (2 | (3 << 2)),
                    "reader 3");
        }
    }
 
    return 0;
}

int main(int argc, char * argv[])
{
    plan_tests(6);
    main_tests();
    return exit_status();
}
