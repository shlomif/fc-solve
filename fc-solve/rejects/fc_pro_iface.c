#if 0
static char * cmd_line_known_parameters[] = { NULL };

int Free2Solver(Position * orig, int NoFcs, int limit, int cmd_line_argc, char * * cmd_line_argv,
    int (*signal_step)(int step_limit))
{
    char * state_string;
    void * instance;
    int verdict;
    int num_iters;
    int ret;
    int current_limit = 1000;
    char * err_str;
    int ret_arg, parser_ret;
    char * moves_string;


    state_string = fc_solve_fc_pro_position_to_string(orig, NoFcs);

    instance = freecell_solver_user_alloc();

    NumFCs = NoFcs ;

    parser_ret = freecell_solver_user_cmd_line_parse_args(
            instance,
            cmd_line_argc,
            cmd_line_argv,
            0,
            cmd_line_known_parameters,
            NULL,
            NULL,
            &err_str,
            &ret_arg
            );

    if (parser_ret == FCS_CMD_LINE_UNRECOGNIZED_OPTION)
    {
        freecell_solver_user_free(instance);
        return 0;
    }


    freecell_solver_user_set_game(
        instance,

        NoFcs,
        8,
        1,
        FCS_SEQ_BUILT_BY_ALTERNATE_COLOR,
        0,
        FCS_ES_FILLED_BY_ANY_CARD
        );

    freecell_solver_user_limit_iterations(instance, current_limit);

    verdict = freecell_solver_user_solve_board(instance, state_string);

    free(state_string);

    while ((verdict == FCS_STATE_SUSPEND_PROCESS) &&
           (
                (limit > 0) ?
                    (current_limit < limit) :
                    1
           )
          )
    {
        if (signal_step(current_limit) != 0)
        {
            break;
        }
        current_limit += 1000;
        freecell_solver_user_limit_iterations(instance, current_limit);
        verdict = freecell_solver_user_resume_solution(instance);
    }

    num_iters = freecell_solver_user_get_num_times(instance);

    if (verdict == FCS_STATE_WAS_SOLVED)
    {
        int num_moves;
        int a;
        fcs_extended_move_t move;
        char * str, * moves_string_proto;
        int len;
        moves_processed_t * moves_processed;

        ret = num_iters;

        moves_processed = moves_processed_gen(orig, NoFcs, instance);
        num_moves = moves_processed_get_moves_left(moves_processed);
        moves_string_proto = (char *)malloc(moves_processed->num_moves*4+1);

        /* a = num_moves-1; */
        str = moves_string_proto;

        while (! moves_processed_get_next_move(moves_processed, &move))
        {
            str = moves_processed_render_move(move, str);
        }
        moves_processed_free(moves_processed);
        len = str-moves_string_proto;
        moves_string = malloc(len+1);
        for(a=0;a<len;a++)
        {
            moves_string[a] = moves_string_proto[len-1-a];
        }
        moves_string[a] = '\0';
        free(moves_string_proto);

    }
    else if (verdict == FCS_STATE_IS_NOT_SOLVEABLE)
    {
        if (num_iters == 0)
            num_iters++ ;
        ret = -num_iters;
    }
    else if ((verdict == FCS_STATE_SUSPEND_PROCESS) || (verdict == FCS_STATE_BEGIN_SUSPEND_PROCESS))
    {
        ret = 0;
    }

    freecell_solver_user_free(instance);

    return ret;
}
#endif

#if 0
            {
                /* Check the intermediate position validity */
                char exists[4*13];

                memset(exists, '\0', sizeof(exists));
                for (int suit = 0 ; suit < 4 ; suit++)
                {
                    for (int rank = 1 ; rank <= fcs_foundation_value(pos, suit) ;
                        rank++)
                    {
                        exists[rank-1+suit*13] = 1;
                    }
                }
                for (int col = 0 ; col < 8 ; col++)
                {
                    const fcs_const_cards_column_t col_col = fcs_state_get_col(pos, col);
                    const_AUTO(count, fcs_col_len(col_col));
                    for (i = 0 ; i < count ; i++)
                    {
                        const_AUTO(card, fcs_col_get_card(col_col, i));
                        exists[fcs_card_rank(card)-1+fcs_card_suit(card)*13] = 1;
                    }
                }
                for (int fc = 0 ; fc < num_freecells ; fc++)
                {
                    const_AUTO(card, fcs_freecell_card(pos, fc));
                    if (fcs_card_is_valid(card))
                    {
                        exists[fcs_card_rank(card)-1+fcs_card_suit(card)*13] = 1;
                    }
                }
                for (int i = 0 ; i < 52 ; i++)
                {
                    if (exists[i] != 1)
                    {
                        printf("Invalid position!!!!!!!!!!!\n");
                        exit(-1);
                    }
                }
            }
#endif

#if 0
    fcs_state_locs_struct_t locs;
    fc_solve_init_locs(&locs);
            printf("STATE=<<<\n%s\n>>>\n",
                fc_solve_state_as_string(
                    &pos,
                    &pos_proto.info,
                    &locs,
                    4,
                    8,
                    1,
                    TRUE,
                    FALSE,
                    TRUE
                )
            );
#endif
