/*
 * This file is part of Freecell Solver. It is subject to the license terms in
 * the COPYING.txt file found in the top-level directory of this distribution
 * and at http://fc-solve.shlomifish.org/docs/distro/COPYING.html . No part of
 * Freecell Solver, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the COPYING file.
 *
 * Copyright (c) 2000 Shlomi Fish
 */
// split_cmd_line.c: split command line arguments from a big string according
// to a subset of Bourne shell's semantics. Useful for reading command
// line arguments from files.
#include "split_cmd_line.h"

typedef struct
{
    fcs_args_man args_man;
    /* These fields are for internal use only. */
    char *last_arg, *last_arg_ptr, *last_arg_end;
} args_man_wrapper;

void fc_solve_args_man_free(fcs_args_man *const manager)
{
    const_SLOT(argc, manager);
    const_SLOT(argv, manager);

    for (int i = 0; i < argc; i++)
    {
        free(argv[i]);
    }
    free(argv);
    manager->argc = 0;
    manager->argv = NULL;
}

static inline void add_to_last_arg(
    args_man_wrapper *const manager, const char c)
{
    *(manager->last_arg_ptr++) = c;

    if (manager->last_arg_ptr == manager->last_arg_end)
    {
        char *const new_last_arg = SREALLOC(manager->last_arg,
            manager->last_arg_end - manager->last_arg + 1024);
        manager->last_arg_ptr += new_last_arg - manager->last_arg;
        manager->last_arg_end += new_last_arg - manager->last_arg + 1024;
        manager->last_arg = new_last_arg;
    }
}

static inline void push_args_last_arg(args_man_wrapper *const manager)
{
    const int len = manager->last_arg_ptr - manager->last_arg;
    char *const new_arg = SMALLOC(new_arg, len + 1);
    strncpy(new_arg, manager->last_arg, len);
    new_arg[len] = '\0';

    manager->args_man.argv[(manager->args_man.argc)++] = new_arg;

    if (!(manager->args_man.argc & (FC_SOLVE__ARGS_MAN_GROW_BY - 1)))
    {
        manager->args_man.argv = SREALLOC(manager->args_man.argv,
            manager->args_man.argc + FC_SOLVE__ARGS_MAN_GROW_BY);
    }

    /* Reset last_arg_ptr so we will have an entirely new argument */
    manager->last_arg_ptr = manager->last_arg;
}

static inline bool is_whitespace(const char c)
{
    return ((c == ' ') || (c == '\t') || (c == '\n') || (c == '\r'));
}

static inline fcs_args_man fc_solve_args_man_alloc(void)
{
    const fcs_args_man ret = {
        .argc = 0, .argv = SMALLOC(ret.argv, FC_SOLVE__ARGS_MAN_GROW_BY)};
    return ret;
}

fcs_args_man fc_solve_args_man_chop(const char *const string)
{
    const char *s = string;

    args_man_wrapper manager = {.args_man = fc_solve_args_man_alloc()};
    manager.last_arg_ptr = manager.last_arg = SMALLOC(manager.last_arg, 1024);
    manager.last_arg_end = manager.last_arg + 1023;

    while (*s != '\0')
    {
        bool push_next_arg_flag = FALSE;

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
            while ((*s != '\0') && (*s != '\n'))
            {
                s++;
            }
            continue;
        }

        bool should_still_loop = TRUE;
        bool in_arg = FALSE;
        while (should_still_loop)
        {
            switch (*s)
            {
            case ' ':
            case '\t':
            case '\n':
            case '\0':
            case '\r':

                push_next_arg_flag = TRUE;
                should_still_loop = FALSE;

                break;

            case '\\':
            {
                const_AUTO(next_char, *(++s));
                s++;
                switch (next_char)
                {
                case '\0':
                    s--;
                    push_next_arg_flag = TRUE;
                    should_still_loop = FALSE;
                    break;
                case '\r':
                case '\n':
                    /* Skip to the next line. */
                    if (!in_arg)
                    {
                        should_still_loop = FALSE;
                    }
                    break;
                default:
                    add_to_last_arg(&manager, next_char);
                    break;
                }
            }
            break;

            case '\"':

                in_arg = TRUE;
                while (TRUE)
                {
                    const_AUTO(c, *(++s));
                    switch (c)
                    {
                    case '\"':
                        ++s;
                    case '\0':
                        goto after_quote;

                    case '\\':
                    {
                        const_AUTO(next_char, *(++s));

                        switch (next_char)
                        {
                        case '\0':
                            push_args_last_arg(&manager);
                            goto END_OF_LOOP;

                        case '\n':
                        case '\r':
                            /* Do nothing */
                            break;

                        case '\\':
                        case '\"':
                            add_to_last_arg(&manager, next_char);
                            break;

                        default:
                            add_to_last_arg(&manager, '\\');
                            add_to_last_arg(&manager, next_char);
                            break;
                        }
                    }
                    break;
                    default:
                    {
                        add_to_last_arg(&manager, c);
                    }
                    break;
                    }
                }
            after_quote:
                break;

            case '#':

                in_arg = FALSE;
                /* Skip to the next line */
                while ((*s != '\0') && (*s != '\n'))
                {
                    s++;
                }
                push_next_arg_flag = TRUE;
                should_still_loop = FALSE;
                break;

            default:
                in_arg = TRUE;
                add_to_last_arg(&manager, *s);
                s++;
                break;
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
    return manager.args_man;
}
