/* Copyright (c) 2009 Shlomi Fish
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
 * scans.h - header file for the scans.c - Best-First-Search and Soft-DFS
 * scans.
 */

#ifndef FC_SOLVE__SCANS_H
#define FC_SOLVE__SCANS_H

#ifdef __cplusplus
extern "C" {
#endif

#include "config.h"
#include "instance.h"

/* We need 2 chars per card - one for the column_idx and one
 * for the card_idx.
 *
 * We also need it times 13 for each of the ranks.
 *
 * We need (4*LOCAL_DECKS_NUM+1) slots to hold the cards plus a
 * (-1,-1) (= end) padding.
 * */
#define FCS_POS_BY_RANK_WIDTH (LOCAL_DECKS_NUM << 3)

extern char * fc_solve_get_the_positions_by_rank_data(
        fc_solve_soft_thread_t * soft_thread,
        fcs_pass_state_t * ptr_state
        );

extern int fc_solve_sfs_check_state_begin(
    fc_solve_hard_thread_t * const hard_thread,
    fcs_lvalue_pass_state_t * const out_new_state_out,
    fcs_pass_state_t * const raw_state_raw,
    fcs_move_stack_t * const moves
    );

extern void fc_solve_sfs_check_state_end(
    fc_solve_soft_thread_t * const soft_thread,
    fcs_pass_state_t * const key_ptr_state_key,
    fcs_pass_state_t * const key_ptr_new_state_key,
    const int state_context_value,
    fcs_move_stack_t * const moves,
    fcs_derived_states_list_t * const derived_states_list
    );

#ifdef __cplusplus
}
#endif

#endif /* FC_SOLVE__SCANS_H */
