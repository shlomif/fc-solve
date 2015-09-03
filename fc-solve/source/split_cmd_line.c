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

typedef struct
{
    args_man_t args_man;
    /* These fields are for internal use only. */
    char * last_arg, * last_arg_ptr, * last_arg_end;
} args_man_wrapper_t;

void fc_solve_args_man_free(args_man_t * const manager)
{
    const typeof(manager->argc) argc = manager->argc;
    const typeof(manager->argv) argv = manager->argv;

    for (int i = 0 ; i < argc ; i++)
    {
        free(argv[i]);
    }
    free(argv);
    manager->argc = 0;
    manager->argv = NULL;
}

static GCC_INLINE void add_to_last_arg(args_man_wrapper_t * const manager, const char c)
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

static GCC_INLINE void push_args_last_arg(args_man_wrapper_t * const manager)
{
    const int length = manager->last_arg_ptr - manager->last_arg;

    char * const new_arg = SMALLOC(new_arg, length+1);

    strncpy(
        new_arg, manager->last_arg,
        length
    );
    new_arg[length] = '\0';

    manager->args_man.argv[(manager->args_man.argc)++] = new_arg;

    if (! (manager->args_man.argc & (FC_SOLVE__ARGS_MAN_GROW_BY-1)))
    {
        manager->args_man.argv = SREALLOC(
            manager->args_man.argv,
            manager->args_man.argc + FC_SOLVE__ARGS_MAN_GROW_BY
        );
    }

    /* Reset last_arg_ptr so we will have an entirely new argument */
    manager->last_arg_ptr = manager->last_arg;

    return;
}

static GCC_INLINE const fcs_bool_t is_whitespace(const char c)
{
    return ((c == ' ') || (c == '\t') || (c == '\n') || (c == '\r'));
}

static GCC_INLINE args_man_t fc_solve_args_man_alloc(void)
{
    const args_man_t ret = {.argc = 0, .argv = SMALLOC(ret.argv, FC_SOLVE__ARGS_MAN_GROW_BY) };
    return ret;
}

args_man_t fc_solve_args_man_chop(const char * const string)
{
    const char * s = string;

    args_man_wrapper_t manager = {.args_man = fc_solve_args_man_alloc()};

    manager.last_arg_ptr = manager.last_arg =
        SMALLOC(manager.last_arg, 1024);
    manager.last_arg_end = manager.last_arg + 1023;

    while (*s != '\0')
    {
        fcs_bool_t push_next_arg_flag = FALSE;

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
            /* Skip to the next line */
            while((*s != '\0') && (*s != '\n'))
            {
                s++;
            }
            continue;
        }

        {
            fcs_bool_t still_loop = TRUE;
            fcs_bool_t in_arg = FALSE;
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
                            add_to_last_arg(&manager, next_char);
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
                                push_args_last_arg(&manager);

                                goto END_OF_LOOP;
                            }
                            else if ((next_char == '\n') || (next_char == '\r'))
                            {
                                /* Do nothing */
                            }
                            else if ((next_char == '\\') || (next_char == '\"'))
                            {
                                add_to_last_arg(&manager, next_char);
                            }
                            else
                            {
                                add_to_last_arg(&manager, '\\');
                                add_to_last_arg(&manager, next_char);
                            }
                        }
                        else
                        {
                            add_to_last_arg(&manager, *s);
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
                    add_to_last_arg(&manager, *s);
                    s++;

                    break;
                }
            }
        }

        if (push_next_arg_flag)
        {
            push_args_last_arg(&manager);

            if (*s == '\0')
            {
                break;
            }
        }
    }

END_OF_LOOP:
    if (manager.last_arg_ptr != manager.last_arg)
    {
        push_args_last_arg(&manager);
    }

    free(manager.last_arg);
#if 0
    /* Not needed because they get discarded. */
    manager.last_arg = manager.last_arg_ptr = manager.last_arg_end = NULL;
#endif

    return manager.args_man;
}

