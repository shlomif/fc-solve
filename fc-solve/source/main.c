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
 * main.c - the main() and related functions of the fc-solve command line
 * executable.
 *
 * It is documented in the documents "README", "USAGE", etc. in the
 * Freecell Solver distribution from http://fc-solve.shlomifish.org/ .
 */
#include "rinutils.h"
#include "preset.h"

int main(int argc, char *argv[])
{
    const fcs_preset_t *new_preset_ptr;
    const_AUTO(
        status1, fc_solve_get_preset_by_name("notexist", &new_preset_ptr));
    if (status1 != FCS_PRESET_CODE_OK)
    {
        return status1;
    }
    return 0;
}
