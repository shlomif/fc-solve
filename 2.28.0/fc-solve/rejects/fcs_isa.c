/*
 * =====================================================================================
 *
 *       Filename:  fcs_isa.c
 *
 *    Description:  rejects from fcs_isa.c
 *
 *        Version:  1.0
 *        Created:  16/05/09 09:11:32
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), 
 *        Company:  
 *
 * =====================================================================================
 */

#if 0
fcs_state_keyval_pair_t * fcs_state_ia_alloc(fc_solve_hard_thread_t * hard_thread)
{
    if (hard_thread->num_states_in_last_pack == hard_thread->state_pack_len)
    {
        if (hard_thread->num_state_packs == hard_thread->max_num_state_packs)
        {
            hard_thread->max_num_state_packs += IA_STATE_PACKS_GROW_BY;
            hard_thread->state_packs = (fcs_state_keyval_pair_t * *)realloc(hard_thread->state_packs, sizeof(fcs_state_keyval_pair_t *) * hard_thread->max_num_state_packs);
        }
        hard_thread->state_packs[hard_thread->num_state_packs] = malloc(hard_thread->state_pack_len * sizeof(fcs_state_keyval_pair_t));
        hard_thread->num_state_packs++;
        hard_thread->num_states_in_last_pack = 0;
    }
    return &(hard_thread->state_packs[hard_thread->num_state_packs-1][hard_thread->num_states_in_last_pack++]);
}
#endif

#if 0
void fcs_state_ia_release(fc_solve_hard_thread_t * hard_thread)
{
    hard_thread->num_states_in_last_pack--;
}
#endif

