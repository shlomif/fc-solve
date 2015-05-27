#define DC_LOOP_START(offset, filter) \
    for (int dc = dest_cards_num-offset ; dc >= 0 ; dc--) \
    {   \
        if (!filter(fcs_col_get_card(dest_col, dc), card)) \
        { \
            continue; \
        } \



#define DC_LOOP_END() \
    }

#define DS_DC_LOOP_START(min_num_cards, offset, filter) \
    STACK_DEST_LOOP_START(min_num_cards) \
        DC_LOOP_START(offset, filter)

#define DS_DC_LOOP_END() \
    DC_LOOP_END() \
    STACK_DEST_LOOP_END()
