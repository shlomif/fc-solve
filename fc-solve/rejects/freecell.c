#ifdef FCS_WITH_TALONS
/*
    Let's try to deal the Gypsy-type Talon.

  */
DECLARE_MOVE_FUNCTION(fc_solve_sfs_deal_gypsy_talon)
{
    tests_declare_accessors()


    fcs_card_t temp_card;
    int a;

    fcs_internal_move_t temp_move;

    tests_define_accessors();

    if (instance->talon_type != FCS_TALON_GYPSY)
    {
        return;
    }

    if (fcs_talon_pos(state) < fcs_talon_len(state))
    {
        sfs_check_state_begin()
        for(a=0;a<LOCAL_STACKS_NUM;a++)
        {
            fcs_cards_column_t new_a_col;

            temp_card = fcs_get_talon_card(new_state, fcs_talon_pos(new_state)+a);
            my_copy_stack(a);

            new_a_col = fcs_state_get_col(new_state, a);

            fcs_col_push_card(new_a_col,temp_card);
        }
        fcs_talon_pos(new_state) += LOCAL_STACKS_NUM;
        fcs_int_move_set_type(temp_move, FCS_MOVE_TYPE_DEAL_GYPSY_TALON);
        fcs_move_stack_push(moves, temp_move);

        sfs_check_state_end()
    }

    return;
}


DECLARE_MOVE_FUNCTION(fc_solve_sfs_get_card_from_klondike_talon)
{
    tests_declare_accessors()

    fcs_internal_move_t temp_move;

    int num_redeals_left, num_redeals_done, num_cards_moved[2];
    int first_iter;
    fcs_card_t card_to_check, top_card;
    int s;
    int cards_num;
    int a;
    fcs_cards_column_t col;

    fcs_state_keyval_pair_t talon_temp;

    tests_define_accessors();

    if (instance->talon_type != FCS_TALON_KLONDIKE)
    {
        return;
    }

    /* Duplicate the talon and its parameters into talon_temp */
    talon_temp->s.talon = malloc(fcs_klondike_talon_len(state)+1);
    memcpy(
        talon_temp->s.talon,
        ptr_state_key->talon,
        fcs_klondike_talon_len(state)+1
        );
    memcpy(
        talon_temp->s.talon_params,
        ptr_state_key->talon_params,
        sizeof(ptr_state_key->talon_params)
        );

    /* Make sure we redeal the talon only once */
    num_redeals_left = fcs_klondike_talon_num_redeals_left(state);
    if ((num_redeals_left > 0) || (num_redeals_left < 0))
    {
        num_redeals_left = 1;
    }
    num_redeals_done = 0;
    num_cards_moved[0] = 0;
    num_cards_moved[1] = 0;

    first_iter = 1;
    while (num_redeals_left >= 0)
    {
        if ((fcs_klondike_talon_stack_pos(talon_temp->s) == -1) &&
            (fcs_klondike_talon_queue_pos(talon_temp->s) == fcs_klondike_talon_len(talon_temp->s)))
        {
            break;
        }
        if ((!first_iter) || (fcs_klondike_talon_stack_pos(talon_temp->s) == -1))
        {
            if (fcs_klondike_talon_queue_pos(talon_temp->s) == fcs_klondike_talon_len(talon_temp->s))
            {
                if (num_redeals_left > 0)
                {
                    fcs_klondike_talon_len(talon_temp->s) = fcs_klondike_talon_stack_pos(talon_temp->s);
                    fcs_klondike_talon_redeal_bare(talon_temp->s);

                    num_redeals_left--;
                    num_redeals_done++;
                }
                else
                {
                    break;
                }
            }
            fcs_klondike_talon_queue_to_stack(talon_temp->s);
            num_cards_moved[num_redeals_done]++;
        }
        first_iter = 0;

        card_to_check = fcs_klondike_talon_get_top_card(talon_temp->s);
        for(s=0 ; s<LOCAL_STACKS_NUM ; s++)
        {
            col = fcs_state_get_col(state, s);
            cards_num = fcs_col_len(col);
            top_card = fcs_col_get_card(col, cards_num-1);
            if (fcs_is_parent_card(card_to_check, top_card))
            {
                fcs_cards_column_t new_s_col;
                /*
                 * We have a card in the talon that we can move
                 * to the column, so let's move it
                 * */
                sfs_check_state_begin()

                my_copy_stack(s);

                new_s_col = fcs_state_get_col(new_state, s);

                new_state.talon = malloc(fcs_klondike_talon_len(talon_temp->s)+1);
                memcpy(
                    new_state.talon,
                    talon_temp->s.talon,
                    fcs_klondike_talon_len(talon_temp->s)+1
                    );

                memcpy(
                    ptr_new_state_key->talon_params,
                    talon_temp->s.talon_params,
                    sizeof(ptr_state_key->talon_params)
                );

                for(a=0;a<=num_redeals_done;a++)
                {
                    fcs_int_move_set_type(temp_move, FCS_MOVE_TYPE_KLONDIKE_FLIP_TALON);
                    fcs_int_move_set_num_cards_flipped(temp_move, num_cards_moved[a]);
                    fcs_move_stack_push(moves, temp_move);
                    if (a != num_redeals_done)
                    {
                        fcs_int_move_set_type(temp_move, FCS_MOVE_TYPE_KLONDIKE_REDEAL_TALON);
                        fcs_move_stack_push(moves,temp_move);
                    }
                }
                fcs_col_push_card(new_s_col, fcs_klondike_talon_get_top_card(new_state));
                fcs_int_move_set_type(temp_move, FCS_MOVE_TYPE_KLONDIKE_TALON_TO_STACK);
                fcs_int_move_set_dest_stack(temp_move, s);
                fcs_klondike_talon_decrement_stack(new_state);

                sfs_check_state_end()
            }
        }
    }

    free(talon_temp->s.talon);
    free(talon_temp);

    return;

}

#endif

/* Disabling Yukon solving for the time being. */
#if 0
DECLARE_MOVE_FUNCTION(fc_solve_sfs_yukon_move_card_to_parent)
{
    int stack_idx, cards_num, c, ds;
    int dest_cards_num;
    fcs_card_t card;
    fcs_card_t dest_card;
    fcs_cards_column_t dest_col, col;

#ifndef HARD_CODED_NUM_STACKS
    const int stacks_num = INSTANCE_STACKS_NUM;
#endif
    tests_define_accessors();

    for( ds=0 ; ds < LOCAL_STACKS_NUM ; ds++ )
    {
        dest_col = fcs_state_get_col(state, ds);
        dest_cards_num = fcs_col_len(dest_col);
        if (dest_cards_num > 0)
        {
            dest_card = fcs_col_get_card(dest_col, dest_cards_num-1);
            for( stack_idx=0 ; stack_idx < LOCAL_STACKS_NUM ; stack_idx++)
            {
                if (stack_idx == ds)
                {
                    continue;
                }
                col = fcs_state_get_col(state, stack_idx);
                cards_num = fcs_col_len(col);
                for( c=cards_num-1 ; c >= 0 ; c--)
                {
                    card = fcs_col_get_card(col, c);
                    if (fcs_is_parent_card(card, dest_card))
                    {

                        /* We can move it there - now let's check to see
                         * if it is already above a suitable parent. */
                        if ((c == 0) ||
                            (! fcs_is_parent_card(card, fcs_col_get_card(col, c-1))))
                        {
                            /* Let's move it */
                            sfs_check_state_begin();
                            copy_two_stacks(stack_idx, ds);
                            fcs_move_sequence(ds, stack_idx, c, cards_num-1);
                            sfs_check_state_end();
                        }

                    }
                }
            }
        }
    }
}

DECLARE_MOVE_FUNCTION(fc_solve_sfs_yukon_move_kings_to_empty_stack)
{
    int stack_idx, cards_num, c, ds;
    fcs_card_t card;
    fcs_cards_column_t col;

#ifndef HARD_CODED_NUM_STACKS
    const int stacks_num = INSTANCE_STACKS_NUM;
#endif

    fcs_game_limit_t num_vacant_stacks;
    tests_define_accessors();

    num_vacant_stacks = soft_thread->num_vacant_stacks;
    if (num_vacant_stacks == 0)
    {
        return;
    }

    for(ds=0;ds<LOCAL_STACKS_NUM;ds++)
    {
        if (fcs_col_len(
            fcs_state_get_col(state, ds)
            ) == 0)
        {
            break;
        }
    }

    for( stack_idx=0 ; stack_idx < LOCAL_STACKS_NUM ; stack_idx++)
    {
        col = fcs_state_get_col(state, stack_idx);
        cards_num = fcs_col_len(col);
        for( c=cards_num-1 ; c >= 1 ; c--)
        {
            card = fcs_col_get_card(col, c);
            if (fcs_card_rank(card) == 13)
            {
                /* It's a King - so let's move it */
                sfs_check_state_begin();
                copy_two_stacks(stack_idx, ds);
                fcs_move_sequence(ds, stack_idx, c, cards_num-1);
                sfs_check_state_end();
            }
        }
    }
}
#endif
