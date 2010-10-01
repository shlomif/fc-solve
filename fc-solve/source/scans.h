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

extern int fc_solve_soft_dfs_do_solve(fc_solve_soft_thread_t * soft_thread);

extern void fc_solve_soft_thread_init_befs_or_bfs(
    fc_solve_soft_thread_t * soft_thread
    );

extern int fc_solve_befs_or_bfs_do_solve(
    fc_solve_soft_thread_t * soft_thread
    );

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
#ifdef FCS_RCS_STATES
        fcs_state_t * ptr_state_key,
#endif
        fcs_collectible_state_t * ptr_state
        );

extern int fc_solve_sfs_check_state_begin(
    fc_solve_hard_thread_t * hard_thread,
#ifdef FCS_RCS_STATES
    fcs_state_t * out_new_state_key,
#endif
    fcs_collectible_state_t * *  out_ptr_new_state,
#ifdef FCS_RCS_STATES
    fcs_state_t * ptr_state_key_ptr,
#endif
    fcs_collectible_state_t * ptr_state_val,
    fcs_move_stack_t * moves
    );

extern void fc_solve_sfs_check_state_end(
    fc_solve_soft_thread_t * soft_thread,
#ifdef FCS_RCS_STATES
    fcs_state_t * ptr_state_key,
#endif
    fcs_collectible_state_t * ptr_state,
#ifdef FCS_RCS_STATES
    fcs_state_t * ptr_new_state_key,
#endif
    fcs_collectible_state_t * ptr_new_state,
    int state_context_value,
    fcs_move_stack_t * moves,
    fcs_derived_states_list_t * derived_states_list
    );

extern void fc_solve_increase_dfs_max_depth(
    fc_solve_soft_thread_t * soft_thread
    );

#ifdef __cplusplus
}
#endif

#endif /* FC_SOLVE__SCANS_H */
