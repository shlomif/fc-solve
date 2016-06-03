/* Copyright (c) 2011 Shlomi Fish
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
 * delta_states_debondt.c - "delta states" are an encoding of states,
 * where the states are encoded and decoded based on a compact delta from the
 * initial state.
 *
 * This encoding improves upon the original delta_states.c .
 */

#define BUILDING_DLL 1

#include "delta_states_debondt_impl.h"

#ifdef FCS_COMPILE_DEBUG_FUNCTIONS
#include "prepare_state_str.h"
/*
 * The char * returned is malloc()ed and should be free()ed.
 */
DLLEXPORT char * fc_solve_user_INTERNAL_debondt_delta_states_enc_and_dec(
        const enum fcs_dbm_variant_type_t local_variant,
        const char * const init_state_str_proto,
        const char * const derived_state_str_proto
)
{
    char * init_state_s, * derived_state_s;
    fcs_state_keyval_pair_t init_state, derived_state, new_derived_state;
    fc_solve_debondt_delta_stater_t delta;
    fcs_uchar_t enc_state[24];
    fcs_state_locs_struct_t locs;

    DECLARE_IND_BUF_T(indirect_stacks_buffer)
    DECLARE_IND_BUF_T(derived_stacks_buffer)
    DECLARE_IND_BUF_T(new_derived_indirect_stacks_buffer)

    init_state_s = prepare_state_str(init_state_str_proto);
    derived_state_s = prepare_state_str(derived_state_str_proto);

    fc_solve_initial_user_state_to_c(
            init_state_s,
            &init_state,
            FREECELLS_NUM,
            STACKS_NUM,
            DECKS_NUM,
            indirect_stacks_buffer
            );

    fc_solve_initial_user_state_to_c(
            derived_state_s,
            &derived_state,
            FREECELLS_NUM,
            STACKS_NUM,
            DECKS_NUM,
            derived_stacks_buffer
            );

    fc_solve_debondt_delta_stater_init(
            &delta,
            local_variant,
            &(init_state.s),
            STACKS_NUM,
            FREECELLS_NUM
#ifndef FCS_FREECELL_ONLY
            , FCS_SEQ_BUILT_BY_ALTERNATE_COLOR
#endif
            );

    fc_solve_debondt_delta_stater_set_derived(&delta, &(derived_state.s));

    fc_solve_state_init(
        &new_derived_state,
        STACKS_NUM,
        new_derived_indirect_stacks_buffer
    );

    fc_solve_var_base_writer_start(&(delta.w));
    fc_solve_debondt_delta_stater_encode_composite(&delta, local_variant, &(delta.w));
    memset(enc_state, '\0', sizeof(enc_state));
    fc_solve_var_base_writer_get_data(&(delta.w), enc_state);

    fc_solve_var_base_reader_start(&(delta.r), enc_state, sizeof(enc_state));
    fc_solve_debondt_delta_stater_decode(&delta, local_variant, &(delta.r),
                                         &(new_derived_state.s));

    fc_solve_init_locs(&locs);

    char * new_derived_as_str = SMALLOC(new_derived_as_str, 1000);
    fc_solve_state_as_string(
        new_derived_as_str,
        &(new_derived_state.s),
        &locs
        PASS_FREECELLS(FREECELLS_NUM)
        PASS_STACKS(STACKS_NUM)
        PASS_DECKS(DECKS_NUM)
        FC_SOLVE__PASS_PARSABLE(TRUE)
        , FALSE
        FC_SOLVE__PASS_T(TRUE)
    );

    free(init_state_s);
    free(derived_state_s);

    fc_solve_debondt_delta_stater_release (&delta);

    return new_derived_as_str;
}


#endif
