#ifndef DEBUG_STATES
                const fcs_card_t top_card = fcs_col_get_card(col, 0);
#else
                const fcs_card_t top_card_proto = fcs_col_get_card(col, 0);
                const unsigned char top_card =
                    (top_card_proto.suit | (top_card_proto.rank << 2));
#endif
