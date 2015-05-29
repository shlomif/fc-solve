
#define fcs_int_move_set_num_cards_flipped(move,value) (move).num_cards_in_seq = ((unsigned int)(value));
#define fcs_int_move_set_num_cards_flipped(move,value) fcs_move_set_num_cards_flipped((move),(value))
