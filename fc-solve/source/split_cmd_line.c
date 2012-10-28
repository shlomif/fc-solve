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

#define BUILDING_DLL 1

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "split_cmd_line.h"

#include "inline.h"
#include "bool.h"
#include "alloc_wrap.h"

#define ARGS_MAN_GROW_BY 32

args_man_t * fc_solve_args_man_alloc(void)
{
    args_man_t * ret = SMALLOC1(ret);
    ret->argc = 0;
    ret->argv = SMALLOC(ret->argv, ARGS_MAN_GROW_BY);
    return ret;
}

void fc_solve_args_man_free(args_man_t * manager)
{
    const typeof(manager->argc) argc = manager->argc;
    const typeof(manager->argv) argv = manager->argv;

    for (int i = 0 ; i < argc ; i++)
    {
        free(argv[i]);
    }
    free(argv);
    free(manager);
}

static GCC_INLINE void add_to_last_arg(args_man_t * manager, char c)
{
    *(manager->last_arg_ptr++) = c;

    if (manager->last_arg_ptr == manager->last_arg_end)
    {
        char * new_last_arg = SREALLOC(manager->last_arg,
            manager->last_arg_end-manager->last_arg+1024
        );
        manager->last_arg_ptr += new_last_arg - manager->last_arg;
        manager->last_arg_end += new_last_arg - manager->last_arg + 1024;
        manager->last_arg = new_last_arg;
    }

    return;
}

static GCC_INLINE void push_args_last_arg(args_man_t * manager)
{
    const int length = manager->last_arg_ptr - manager->last_arg;

    char * const new_arg = SMALLOC(new_arg, length+1);

    strncpy(
        new_arg, manager->last_arg,
        length
    );
    new_arg[length] = '\0';

    manager->argv[(manager->argc)++] = new_arg;

    if (! (manager->argc & (ARGS_MAN_GROW_BY-1)))
    {
        manager->argv = SREALLOC(
            manager->argv,
            manager->argc + ARGS_MAN_GROW_BY
        );
    }

    /* Reset last_arg_ptr so we will have an entirely new argument */
    manager->last_arg_ptr = manager->last_arg;

    return;
}

static GCC_INLINE fcs_bool_t is_whitespace(char c)
{
    return ((c == ' ') || (c == '\t') || (c == '\n') || (c == '\r'));
}

int fc_solve_args_man_chop(args_man_t * manager, char * string)
{
    char * s = string;
    fcs_bool_t in_arg;

    manager->last_arg_ptr = manager->last_arg =
        SMALLOC(manager->last_arg, 1024);
    manager->last_arg_end = manager->last_arg + 1023;

    while (*s != '\0')
    {
        fcs_bool_t push_next_arg_flag = FALSE;

        in_arg = FALSE;
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
            in_arg = FALSE;
            /* Skip to the next line */
            while((*s != '\0') && (*s != '\n'))
            {
                s++;
            }
            continue;
        }

        {
            fcs_bool_t still_loop = TRUE;
            while (still_loop)
            {
                switch(*s)
                {
                    case ' ':
                    case '\t':
                    case '\n':
                    case '\0':
                    case '\r':

                    push_next_arg_flag = TRUE;
                    still_loop = FALSE;

                    break;

                    case '\\':

                    {
                        char next_char = *(++s);
                        s++;
                        if (next_char == '\0')
                        {
                            s--;
                            push_next_arg_flag = TRUE;
                            still_loop = FALSE;
                        }
                        else if ((next_char == '\n') || (next_char == '\r'))
                        {
                            /* Skip to the next line. */
                            if (! in_arg)
                            {
                                still_loop = FALSE;
                            }
                        }
                        else
                        {
                            add_to_last_arg(manager, next_char);
                        }
                    }
                    break;

                    case '\"':

                    s++;
                    in_arg = TRUE;
                    while ((*s != '\"') && (*s != '\0'))
                    {
                        if (*s == '\\')
                        {
                            char next_char;

                            next_char = *(++s);
                            if (next_char == '\0')
                            {
                                push_args_last_arg(manager);

                                goto END_OF_LOOP;
                            }
                            else if ((next_char == '\n') || (next_char == '\r'))
                            {
                                /* Do nothing */
                            }
                            else if ((next_char == '\\') || (next_char == '\"'))
                            {
                                add_to_last_arg(manager, next_char);
                            }
                            else
                            {
                                add_to_last_arg(manager, '\\');
                                add_to_last_arg(manager, next_char);
                            }
                        }
                        else
                        {
                            add_to_last_arg(manager, *s);
                        }
                        s++;
                    }
                    s++;
                    break;

                    case '#':

                    in_arg = FALSE;
                    /* Skip to the next line */
                    while((*s != '\0') && (*s != '\n'))
                    {
                        s++;
                    }
                    push_next_arg_flag = TRUE;
                    still_loop = FALSE;
                    break;

                    default:

                    in_arg = TRUE;
                    add_to_last_arg(manager, *s);
                    s++;

                    break;
                }
            }
        }

        if (push_next_arg_flag)
        {
            push_args_last_arg(manager);
            in_arg = FALSE;

            if (*s == '\0')
            {
                break;
            }
        }
    }

END_OF_LOOP:
    if (manager->last_arg_ptr != manager->last_arg)
    {
        push_args_last_arg(manager);
    }

    free(manager->last_arg);
    manager->last_arg = manager->last_arg_ptr = manager->last_arg_end = NULL;

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
