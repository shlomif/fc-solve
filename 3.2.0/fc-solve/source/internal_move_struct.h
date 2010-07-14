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
 * fcs_move.h - header file for the move structure and enums of
 * Freecell Solver. This file is common to the main code and to the
 * library headers.
 *
 */

#ifndef FC_SOLVE__INTERNAL_MOVE_STRUCT_H
#define FC_SOLVE__INTERNAL_MOVE_STRUCT_H

#include "fcs_move.h"

#ifdef __cplusplus
extern "C" {
#endif

#include "config.h"

#ifdef FCS_USE_COMPACT_MOVE_TOKENS
typedef struct
{
    /*  TODO : Change to log_2 ( 
     *      max(MAX_NUM_FREECELLS, MAX_NUM_STACKS, MAX_NUM_DECKS * 4) 
     *  ) */
    unsigned int type : 4;
    unsigned int src : 4;
    unsigned int dest : 4;
    unsigned int num_cards_in_seq : 4;
} fcs_internal_move_t;
#else
typedef fcs_move_t fcs_internal_move_t;
#endif

#ifdef FCS_USE_COMPACT_MOVE_TOKENS
#define fcs_int_move_set_src_stack(move,value)        (move).src = ((unsigned int)(value));
#define fcs_int_move_set_src_freecell(move,value)     (move).src = ((unsigned int)(value));
#define fcs_int_move_set_dest_stack(move,value)       (move).dest = ((unsigned int)(value));
#define fcs_int_move_set_dest_freecell(move,value)    (move).dest = ((unsigned int)(value));
#define fcs_int_move_set_foundation(move,value)       (move).dest = ((unsigned int)(value));
#define fcs_int_move_set_type(move,value)             (move).type = ((unsigned int)(value));
#define fcs_int_move_set_num_cards_in_seq(move,value) (move).num_cards_in_seq = ((unsigned int)(value));
#define fcs_int_move_set_num_cards_flipped(move,value) (move).num_cards_in_seq = ((unsigned int)(value));

#define fcs_int_move_get_src_stack(move)              ((move).src)
#define fcs_int_move_get_src_freecell(move)           ((move).src)
#define fcs_int_move_get_dest_stack(move)             ((move).dest)
#define fcs_int_move_get_dest_freecell(move)          ((move).dest)
#define fcs_int_move_get_foundation(move)             ((move).dest)
#define fcs_int_move_get_type(move)                   ((move).type)
#define fcs_int_move_get_num_cards_in_seq(move)       ((move).num_cards_in_seq)

#else /* Not FCS_USE_COMPACT_MOVE_TOKENS */

#define fcs_int_move_set_src_stack(move,value) fcs_move_set_src_stack((move),(value))
#define fcs_int_move_set_src_freecell(move,value) fcs_move_set_src_freecell((move),(value))
#define fcs_int_move_set_dest_stack(move,value) fcs_move_set_dest_stack((move),(value))
#define fcs_int_move_set_dest_freecell(move,value) fcs_move_set_dest_freecell((move),(value))
#define fcs_int_move_set_foundation(move,value) fcs_move_set_foundation((move),(value))
#define fcs_int_move_set_type(move,value) fcs_move_set_type((move),(value))
#define fcs_int_move_set_num_cards_in_seq(move,value) fcs_move_set_num_cards_in_seq((move),(value))
#define fcs_int_move_set_num_cards_flipped(move,value) fcs_move_set_num_cards_flipped((move),(value))

#define fcs_int_move_get_src_stack(move) fcs_move_get_src_stack((move))
#define fcs_int_move_get_src_freecell(move) fcs_move_get_src_freecell((move))
#define fcs_int_move_get_dest_stack(move) fcs_move_get_dest_stack((move))
#define fcs_int_move_get_dest_freecell(move) fcs_move_get_dest_freecell((move))
#define fcs_int_move_get_foundation(move) fcs_move_get_foundation((move))
#define fcs_int_move_get_type(move) fcs_move_get_type((move))
#define fcs_int_move_get_num_cards_in_seq(move) fcs_move_get_num_cards_in_seq((move))

#endif

typedef struct
{
    fcs_internal_move_t * moves;
    int num_moves;
} fcs_move_stack_t;

#ifdef __cplusplus
}
#endif

#endif  /* FC_SOLVE__INTERNAL_MOVE_STRUCT_H */

