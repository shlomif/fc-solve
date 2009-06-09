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
 * split_cmd_line.c: split command line arguments from a big string according
 * to a subset of Bourne shell's semantics. Useful for reading command
 * line arguments from files.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "split_cmd_line.h"

#define ARGS_MAN_GROW_BY 32

args_man_t * fc_solve_args_man_alloc(void)
{
    args_man_t * ret;
    ret = malloc(sizeof(args_man_t));
    ret->argc = 0;
    ret->argv = malloc(sizeof(ret->argv[0]) * ARGS_MAN_GROW_BY);
    return ret;
}

void fc_solve_args_man_free(args_man_t * manager)
{
    int a;
    for(a=0;a<manager->argc;a++)
    {
        free(manager->argv[a]);
    }
    free(manager->argv);
    free(manager);
}

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
            if (! (manager->argc & (ARGS_MAN_GROW_BY-1))) \
            { \
                manager->argv = realloc( \
                    manager->argv,  \
                    sizeof(manager->argv[0]) * (manager->argc + ARGS_MAN_GROW_BY) \
                    ); \
            } \
       \
            /* Reset last_arg_ptr so we will have an entirely new argument */ \
            last_arg_ptr = last_arg; \
    }

#define is_whitespace(c) \
    (((c) == ' ') || ((c) == '\t') || ((c) == '\n') || ((c) == '\r'))

int fc_solve_args_man_chop(args_man_t * manager, char * string)
{
    char * s = string;
    char * new_arg;
    char * last_arg, * last_arg_ptr, * last_arg_end, * new_last_arg;
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
            char next_char = *(++s);
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
                    char next_char;

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

#if 0
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
    args_man = fc_solve_args_man_alloc();
    /* Call it on string */
    fc_solve_args_man_chop(args_man, string);

    /* Now use args_man->argc and args_man->argv */
    {
        int a;
        for(a=0;a<args_man->argc;a++)
        {
            printf("argv[%i] = \"%s\"\n", a, args_man->argv[a]);
        }
    }
    /* Free the allocated memory */
    fc_solve_args_man_free(args_man);

    free(string);

    return 0;
}
#endif
