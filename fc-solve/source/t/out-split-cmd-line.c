/* Copyright (c) 2009 Shlomi Fish
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
 * t/out-split-cmd-line.c - helper for testing the split_cmd_line.c module.
 *
 * What this program does is accept sh-like input on STDIN and on STDOUT
 * output the parsed / splitted output in a well-formed format.
 *
 * The format is <<$DELIM\n[TEXT]\n$DELIM\n . $DELIM is guaranteed not to
 * appear anywhere in [TEXT].
 *
 */

#include <string.h>
#include <stdio.h>

#include "../split_cmd_line.c"


int main(int argc, char * argv[])
{
    char buffer[64 * 1024];
    args_man_t * args;
    int i;

    memset(buffer, '\0', sizeof(buffer));
    fread(buffer, sizeof(buffer[0]), sizeof(buffer)-1, stdin);

    args = fc_solve_args_man_alloc();

    fc_solve_args_man_chop(args, buffer);

    for ( i=0 ; i < args->argc ; i++)
    {
        const char * s;
        char * terminator;

        s = args->argv[i];
        /* Handle terminal case of delimiting an argument */
        terminator = malloc(strlen(s)+50);
        strcpy(terminator, "FCS_END_OF_STRING");
        while(strstr(s, terminator))
        {
            strcat(terminator, "G");
        }

        printf("<<%s\n%s\n%s\n", terminator, s, terminator);

        free(terminator);
    }

    fc_solve_args_man_free(args);

    return 0;
}
