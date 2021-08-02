// This file is part of Freecell Solver. It is subject to the license terms in
// the COPYING.txt file found in the top-level directory of this distribution
// and at http://fc-solve.shlomifish.org/docs/distro/COPYING.html . No part of
// Freecell Solver, including this file, may be copied, modified, propagated,
// or distributed except according to the terms contained in the COPYING file.
//
// Copyright (c) 2000 Shlomi Fish
#include <stdio.h>
#include <string.h>
#include "range_solvers_gen_ms_boards.h"
#include "deals_populator.h"
#include "try_param.h"
#include "rinutils/rinutils.h"
#ifdef RINUTILS__IS_UNIX
#include <fcntl.h>
#endif

static void __attribute__((noreturn)) print_help(void)
{
    printf("\n%s", "make-multi-board [deal1_idx] [deal2_idx] .. -- \n"
                   "   [--variant variant_str] [fc-solve theme args]\n"
                   "\n"
                   "Generate the initial layouts of several arbitrary deal "
                   "indexes from the\n"
                   "Microsoft/Freecell Pro deals.");
    exit(-1);
}

#define CARD_STR_LEN 3
#define OUTPUT_LEN (CARD_STR_LEN * 4 * 13)
#ifndef RINUTILS__IS_UNIX
static inline void output_file(const char *const filename, const char *const s)
{
    FILE *f = fopen(filename, "wt");
    fwrite(s, OUTPUT_LEN, 1, f);
    fclose(f);
}
#else
static inline void output_file(const char *const filename, const char *const s)
{
    const int fh = open(filename, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    write(fh, s, OUTPUT_LEN);
    close(fh);
}
#endif

int main(int argc, char *argv[])
{
    int arg = 1;
#define DIR_LEN 99
    char dir[DIR_LEN + 1] = "";
#define SUFFIX_LEN 19
    char suffix[SUFFIX_LEN + 1] = "";
    for (; arg < argc; ++arg)
    {
        const char *param;
        if ((param = TRY_P("--dir")))
        {
            if (strlen(param) > DIR_LEN)
            {
                fprintf(stderr, "--dir's argument is too long!\n");
                print_help();
            }
            strcpy(dir, param);
        }
        else if ((param = TRY_P("--suffix")))
        {
            if (strlen(param) > SUFFIX_LEN)
            {
                fprintf(stderr, "--suffix's argument is too long!\n");
                print_help();
            }
            strcpy(suffix, param);
        }
        else
        {
            break;
        }
    }
    if (!dir[0])
    {
        fprintf(stderr, "--dir must be specified!\n");
        print_help();
    }
    populate_deals_from_argv(argc, argv, arg);
#define MAX_NUM_DIGITS 30
#define MARGIN 5
    char filename[DIR_LEN + SUFFIX_LEN + MAX_NUM_DIGITS + MARGIN];
    char *const fn_suffix = filename + sprintf(filename, "%s/", dir);
    fcs_state_string s;
    get_board__setup_string(s);

    DEALS_ITERATE__START(board_num)
    sprintf(fn_suffix, "%lu%s", board_num, suffix);
    get_board_l__without_setup(board_num, s);
    output_file(filename, s);
    DEALS_ITERATE__END()
    deals_ranges__free();

    return 0;
}
