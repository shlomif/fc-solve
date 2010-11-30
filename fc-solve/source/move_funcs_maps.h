
/*
    This file is generated from gen_move_funcs.pl.

    Do not edit by hand!!!
*/

#ifndef FC_SOLVE__MOVE_FUNCS_MAPS_H
#define FC_SOLVE__MOVE_FUNCS_MAPS_H

#include "config.h"

#define FCS_TESTS_NUM 24

#define FCS_TESTS_ALIASES_NUM 24

typedef struct
{
    const char * alias;
    int test_num;
} fcs_test_aliases_mapping_t;

extern fc_solve_solve_for_state_test_t fc_solve_sfs_tests[FCS_TESTS_NUM];
extern fcs_test_aliases_mapping_t fc_solve_sfs_tests_aliases[FCS_TESTS_ALIASES_NUM];

#endif
