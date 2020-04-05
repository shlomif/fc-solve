#ifdef DEBUG_STATES
        if (fc_solve_card_compare(card, fc_solve_empty_card))
#else
        if (!(card == fc_solve_empty_card))
#endif
