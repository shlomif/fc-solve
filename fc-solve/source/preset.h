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

extern int freecell_solver_apply_preset_by_name(
    freecell_solver_instance_t * instance,
    const char * name
    );

extern int freecell_solver_apply_tests_order(
    fcs_tests_order_t * tests_order,
    const char * string,
    char * * error_string
    );

#ifdef __cplusplus
}
#endif

#endif
