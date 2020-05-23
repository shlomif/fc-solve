#pragma once
static inline int_fast32_t __attribute__((pure)) calc_foundation_to_put_card_on(
    const fcs_dbm_variant_type local_variant GCC_UNUSED,
    fcs_state *const ptr_state, const fcs_card card)
{
    FCS_ON_NOT_FC_ONLY(
        const int sequences_are_built_by = CALC_SEQUENCES_ARE_BUILT_BY());
    const fcs_card rank = fcs_card_rank(card);
    const fcs_card suit = fcs_card_suit(card);
    const int_fast32_t rank_min_1 = rank - 1;
    const int_fast32_t rank_min_2 = rank - 2;

    for (stack_i deck = 0; deck < INSTANCE_DECKS_NUM; deck++)
    {
        const stack_i ret_val = ((deck << 2) | suit);
        if (fcs_foundation_value(*ptr_state, ret_val) == rank_min_1)
        {
/* Always put on the foundation if it is built-by-suit */
#ifndef FCS_FREECELL_ONLY
            if (sequences_are_built_by == FCS_SEQ_BUILT_BY_SUIT)
            {
                return (int_fast32_t)ret_val;
            }
#endif
            const size_t lim_ = ((size_t)INSTANCE_DECKS_NUM << 2U);
            for (size_t f = 0; f < lim_; ++f)
            {
                if (fcs_foundation_value(*ptr_state, f) <
                    rank_min_2 -
                        (FCS__SEQS_ARE_BUILT_BY_RANK()
                                ? 0
                                : ((f & 0x1) == (fcs_card_suit(card) & 0x1))))
                {
                    goto next_iter;
                }
            }
            return (int_fast32_t)ret_val;
        next_iter:;
        }
    }

    return -1;
}
