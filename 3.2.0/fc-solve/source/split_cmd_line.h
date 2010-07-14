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
 * split_cmd_line.h: split command line arguments from a big string according
 * to a subset of Bourne shell's semantics. Useful for reading command
 * line arguments from files.
 */
#ifndef FC_SOLVE__SPLIT_CMD_LINE_H
#define FC_SOLVE__SPLIT_CMD_LINE_H

typedef struct 
{
    int argc;
    char * * argv;
    /* These fields are for internal use only. */
    char * last_arg, * last_arg_ptr, * last_arg_end;
} args_man_t;

extern args_man_t * fc_solve_args_man_alloc(void);
extern void fc_solve_args_man_free(args_man_t * manager);
extern int fc_solve_args_man_chop(args_man_t * manager, char * string);

#endif /* #ifndef FC_SOLVE__SPLIT_CMD_LINE_H */

