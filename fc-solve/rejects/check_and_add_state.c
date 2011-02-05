/*
 * =====================================================================================
 *
 *       Filename:  check_and_add_state.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  02/05/2011 11:17:24 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), 
 *        Company:  
 *
 * =====================================================================================
 */

#ifdef FCS_WITH_TALONS
static GCC_INLINE void fc_solve_cache_talon(
    fc_solve_instance_t * instance,
    fcs_state_t * new_state_key,
    fcs_state_extra_info_t * new_state_val
    )
{
    void * cached_talon;
    int hash_value_int;

    new_state_key->talon = realloc(new_state_key->talon, fcs_klondike_talon_len(new_state_key)+1);
    hash_value_int = *(SFO_hash_value_t*)instance->hash_value;
    if (hash_value_int < 0)
    {
        /*
         * This is a bit mask that nullifies the sign bit of the
         * number so it will always be positive
         * */
        hash_value_int &= (~(1<<((sizeof(hash_value_int)<<3)-1)));
    }

    cached_talon = (void *)fc_solve_hash_insert(
        instance->talons_hash,
        new_state_key->talon,
        hash_value_int,
        1
        );

    if (cached_talon != NULL)
    {
        free(new_state_key->talon);
        new_state_key->talon = cached_talon;
    }
}
#endif
