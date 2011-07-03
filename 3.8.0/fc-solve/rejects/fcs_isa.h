/*
 * =====================================================================================
 *
 *       Filename:  fcs_isa.h
 *
 *    Description:  rejects from fcs_isa.h
 *
 *        Version:  1.0
 *        Created:  09/06/09 22:13:41
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), 
 *        Company:  
 *
 * =====================================================================================
 */

extern void fc_solve_state_ia_foreach(
        fc_solve_hard_thread_t * hard_thread,
        void (*ptr_function)(fcs_state_t *, fcs_state_extra_info_t *, void *),
        void * context
        );
