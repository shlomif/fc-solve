
/*
    This file is generated from gen_move_funcs.pl.

    Do not edit by hand!!!
*/

#ifndef FC_SOLVE__MOVE_FUNCS_MAPS_H
#define FC_SOLVE__MOVE_FUNCS_MAPS_H

#include "config.h"

#define FCS_MOVE_FUNCS_NUM 24

#define FCS_MOVE_FUNCS_ALIASES_NUM 24

typedef struct
{
    char alias[2];
    int move_func_num;
} fcs_move_func_aliases_mapping_t;

extern fc_solve_solve_for_state_move_func_t fc_solve_sfs_move_funcs[FCS_MOVE_FUNCS_NUM];
extern fcs_move_func_aliases_mapping_t fc_solve_sfs_move_funcs_aliases[FCS_MOVE_FUNCS_ALIASES_NUM];

#endif
