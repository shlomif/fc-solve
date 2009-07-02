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
