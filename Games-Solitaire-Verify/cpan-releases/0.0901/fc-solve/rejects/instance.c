/*
 * =====================================================================================
 *
 *       Filename:  instance.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  07/02/11 18:42:16
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), 
 *        Company:  
 *
 * =====================================================================================
 */

#ifdef FCS_WITH_TALONS
    /* Initialize the Talon's Cache */
    if (instance->talon_type == FCS_TALON_KLONDIKE)
    {
        instance->talons_hash = fc_solve_hash_init(
            fcs_talon_compare_with_context,
            NULL
            );

        fc_solve_cache_talon(instance, instance->state_copy_ptr_val);
    }
#endif
