/*
 * fcs.h - header file of the preset management functions for Freecell Solver.
 *
 * Written by Shlomi Fish (shlomif@vipe.technion.ac.il), 2000
 *
 * This file is in the public domain (it's uncopyrighted).
 */

#ifndef __PRESET_H
#define __PRESET_H

#ifdef __cplusplus
extern "C" {
#endif

#include "fcs.h"

struct fcs_preset_struct
{
    int preset_id;
    int freecells_num;
    int stacks_num;
    int decks_num;

    int sequences_are_built_by;
    int unlimited_sequence_move;
    int empty_stacks_fill;

    char tests_order[FCS_TESTS_NUM*3+1];
    char allowed_tests[FCS_TESTS_NUM*3+1];
};

typedef struct fcs_preset_struct fcs_preset_t;

extern int freecell_solver_apply_preset_by_name(
    freecell_solver_instance_t * instance,
    const char * name
    );

extern int freecell_solver_apply_tests_order(
    fcs_tests_order_t * tests_order,
    const char * string,
    char * * error_string
    );

extern int freecell_solver_get_preset_by_name(
    const char * name,
    const fcs_preset_t * * preset_ptr
    );

#ifdef __cplusplus
}
#endif

#endif
