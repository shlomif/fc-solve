#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "cl_chop.h"

#ifdef DMALLOC
#include <dmalloc.h>
#endif

#define ARGS_MAN_GROW_BY 30

args_man_t * freecell_solver_args_man_alloc(void)
{
    args_man_t * ret;
    ret = malloc(sizeof(args_man_t));
    ret->argc = 0;
    ret->max_num_argv = ARGS_MAN_GROW_BY;
    ret->argv = malloc(sizeof(ret->argv[0]) * ret->max_num_argv);
    return ret;
}

void freecell_solver_args_man_free(args_man_t * manager)
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

#define add_to_last_arg(c)  \
    {         \
        *(last_arg_ptr++) = (c);     \
        if (last_arg_ptr == last_arg_end) \
        {        \
            new_last_arg = realloc(last_arg, last_arg_end-last_arg+1024);  \
            last_arg_ptr += new_last_arg - last_arg; \
            last_arg_end += new_last_arg - last_arg + 1024;  \
            last_arg = new_last_arg;   \
        }       \
    }

#define push_args_last_arg() {  \
            new_arg = malloc(last_arg_ptr-last_arg+1); \
            strncpy(new_arg, last_arg, last_arg_ptr-last_arg); \
            new_arg[last_arg_ptr-last_arg] = '\0'; \
            manager->argv[manager->argc] = new_arg; \
            manager->argc++; \
            if (manager->argc == manager->max_num_argv) \
            { \
                manager->max_num_argv += ARGS_MAN_GROW_BY; \
                manager->argv = realloc(manager->argv, sizeof(manager->argv[0]) * manager->max_num_argv); \
            } \
       \
            /* Reset last_arg_ptr so we will have an entirely new argument */ \
            last_arg_ptr = last_arg; \
    }

#define is_whitespace(c) \
    (((c) == ' ') || ((c) == '\t') || ((c) == '\n') || ((c) == '\r'))

int freecell_solver_args_man_chop(args_man_t * manager, char * string)
{
    char * s = string;
    char * new_arg;
    char * last_arg, * last_arg_ptr, * last_arg_end, * new_last_arg;
    char next_char;
    int in_arg;

    last_arg_ptr = last_arg = malloc(1024);
    last_arg_end = last_arg + 1023;
    
    while (*s != '\0')
    {
LOOP_START:
        in_arg = 0;
        while (is_whitespace(*s))
        {
            s++;
        }
        if (*s == '\0')
        {
            break;
        }
        if (*s == '#')
        {
            in_arg = 0;
            /* Skip to the next line */
            while((*s != '\0') && (*s != '\n'))
            {
                s++;
            }
            continue;
        }
AFTER_WS:
        while ((*s != ' ') && (*s != '\t') && (*s != '\n') && 
               (*s != '\r') &&
               (*s != '\\') && (*s != '\"') && (*s != '\0') && 
               (*s != '#'))
        {
            in_arg = 1;
            add_to_last_arg(*s);
            s++;
        }

        
        if ((*s == ' ') || (*s == '\t') || (*s == '\n') || (*s == '\0') || (*s == '\r'))
        {
NEXT_ARG:
            push_args_last_arg();
            in_arg = 0;
            
            if (*s == '\0')
            {
                break;
            }
        }
        else if (*s == '\\')
        {
            next_char = *(++s);
            s++;
            if (next_char == '\0')
            {
                s--;
                goto NEXT_ARG;
            }
            else if ((next_char == '\n') || (next_char == '\r'))
            {
                if (in_arg)
                {
                    goto AFTER_WS;
                }
                else
                {
                    goto LOOP_START;
                }
            }
            else
            {
                add_to_last_arg(next_char);
            }
        }
        else if (*s == '\"')
        {
            s++;
            in_arg = 1;
            while ((*s != '\"') && (*s != '\0'))
            {
                if (*s == '\\')
                {
                    next_char = *(++s);
                    if (next_char == '\0')
                    {
                        push_args_last_arg();
                        
                        goto END_OF_LOOP;
                    }
                    else if ((next_char == '\n') || (next_char == '\r'))
                    {
                        /* Do nothing */
                    }
                    else if ((next_char == '\\') || (next_char == '\"'))
                    {
                        add_to_last_arg(next_char);
                    }
                    else
                    {
                        add_to_last_arg('\\');
                        add_to_last_arg(next_char);
                    }
                }
                else
                {
                    add_to_last_arg(*s);
                }
                s++;   
            }
            s++;
            goto AFTER_WS;
        }
        else if (*s == '#')
        {
            in_arg = 0;
            /* Skip to the next line */
            while((*s != '\0') && (*s != '\n'))
            {
                s++;
            }
            goto NEXT_ARG;
        }
    }
END_OF_LOOP:

    free(last_arg);
    
    return 0;
}

#ifdef CMD_LINE_CHOP_WITH_MAIN
int main(int argc, char * * argv)
{
    args_man_t * args_man;
    char * string;

#if 0
    string = argv[1];
#else
    {
        FILE * f;
        
        f = fopen(argv[1],"rb");
        string = calloc(4096,1);
        fread(string, 4095, 1, f);
        fclose(f);
    }
        
#endif
    
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

    free(string);

    return 0;
}
#endif
