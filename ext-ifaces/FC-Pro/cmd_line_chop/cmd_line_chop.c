#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "cmd_line_chop.h"

#ifdef DMALLOC
#include <dmalloc.h>
#endif

#define ARGS_MAN_GROW_BY 30

args_man_t * args_man_alloc(void)
{
    args_man_t * ret;
    ret = malloc(sizeof(args_man_t));
    ret->argc = 0;
    ret->max_num_argv = ARGS_MAN_GROW_BY;
    ret->argv = malloc(sizeof(ret->argv[0]) * ret->max_num_argv);
    return ret;
}

void args_man_free(args_man_t * manager)
{
    int a;
    for(a=0;a<manager->argc;a++)
    {
        free(manager->argv[a]);
    }
    free(manager->argv);
    free(manager);
}

#define skip_ws() { while((*s == ' ') || (*s == '\t')) { s++; } }
#define skip_non_ws() { while((*s != ' ') && (*s != '\t') && (*s)) { s++; }}

#define add_to_last_arg(c) { *(last_arg_ptr++) = (c); if (last_arg_ptr == last_arg_end) { new_last_arg = realloc(last_arg, last_arg_end-last_arg+1024); last_arg_ptr += new_last_arg - last_arg; last_arg_end += new_last_arg - last_arg + 1024; last_arg = new_last_arg } }
int args_man_chop(args_man_t * manager, char * string)
{
    char * s = string;
    char * start;
    char * end;
    char * new_arg;
    char c;
    char * last_arg, * last_arg_ptr, * last_arg_end, * new_last_arg;

    last_arg_ptr = last_arg = malloc(1024);
    last_arg_end = last_arg + 1023;
    
    while (*s != '\0')
    {
        while ((*s == ' ') || (*s == '\t') || (*s == '\n'))
        {
            s++;
        }
        if (*s == '\0')
        {
            break;
        }
AFTER_WS:
        c = *s;
        while ((c != ' ') && (c != '\t') && (c != '\n') && 
               (c != '\\') && && (c != '\"') && (c != '\0') )
        {
            add_to_last_arg(c);
            c = *(++s);
        }

        
        if ((c == ' ') || (c == '\t') || (c == '\n') || (c == '\0'))
        {
NEXT_ARG:
            new_arg = malloc(last_arg_ptr-last_arg+1);
            strncpy(new_arg, last_arg, last_arg_ptr-last_arg);
            new_arg[last_arg_ptr-last_arg] = '\0';
            manager->argv[manager->argc] = new_arg;
            manager->argc++;
            if (manager->argc == manager->max_num_argv)
            {
                manager->max_num_argv += ARGS_MAN_GROW_BY;
                manager->argv = realloc(manager->argv, sizeof(manager->argv[0]) * manager->max_num_argv);
            }

            /* Reset last_arg_ptr so we will have an entirely new argument */
            last_arg_ptr = last_arg;

            if (c == '\0')
            {
                break;
            }
        }
        else if (c == '\\')
        {
            char next_char = *(++s);
            s++;
            if (next_char == '\0')
            {
                s--;
                goto NEXT_ARG;
            }
            else if ((next_char == '\n') || (next_char == '\r'))
            {
                goto NEXT_ARG;
            }
            else
            {
                add_to_last_arg(next_char);
            }
        }
        
        start = s;
        skip_non_ws();
        end = s;
        new_arg = malloc(end-start+1);
        strncpy(new_arg, start, end-start);
        new_arg[end-start] = '\0';
        manager->argv[manager->argc] = new_arg;
        manager->argc++;
        if (manager->argc == manager->max_num_argv)
        {
            manager->max_num_argv += ARGS_MAN_GROW_BY;
            manager->argv = realloc(manager->argv, sizeof(manager->argv[0]) * manager->max_num_argv);
        }
    }
    return 0;
}

#if 0
int main(int argc, char * * argv)
{
    args_man_t * args_man;
    char * string;

    string = argv[1];
    
    /* Initialize an arg man */
    args_man = args_man_alloc();
    /* Call it on string */
    args_man_chop(args_man, string);

    /* Now use args_man->argc and args_man->argv */
    {
        int a;
        for(a=0;a<args_man->argc;a++)
        {
            printf("argv[%i] = \"%s\"\n", a, args_man->argv[a]);
        }
    }
    /* Free the allocated memory */
    args_man_free(args_man);

    return 0;
}
#endif
