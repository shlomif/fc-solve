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

