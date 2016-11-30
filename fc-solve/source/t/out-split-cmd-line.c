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

int main(void)
{
    char buffer[64 * 1024];
    int i;

    memset(buffer, '\0', sizeof(buffer));
    fread(buffer, sizeof(buffer[0]), sizeof(buffer)-1, stdin);

    args_man_t args = fc_solve_args_man_chop(buffer);

    for ( i=0 ; i < args.argc ; i++)
    {
        const char * s;
        char * terminator;

        s = args.argv[i];
        /* Handle terminal case of delimiting an argument */
        terminator = SMALLOC(terminator, strlen(s)+50);
        strcpy(terminator, "FCS_END_OF_STRING");
        while(strstr(s, terminator))
        {
            strcat(terminator, "G");
        }

        printf("<<%s\n%s\n%s\n", terminator, s, terminator);

        free(terminator);
    }

    fc_solve_args_man_free(&args);

    return 0;
}
