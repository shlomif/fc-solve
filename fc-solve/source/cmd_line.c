#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "fcs_user.h"
#include "fcs_cl.h"

#ifdef DMALLOC
#include "dmalloc.h"
#endif


int freecell_solver_user_cmd_line_parse_args(
    void * instance,
    int argc,
    char * argv[],
    int start_arg,
    char * * known_parameters,
    freecell_solver_user_cmd_line_known_commands_callback_t callback,
    void * callback_context,
    char * * error_string,
    int * last_arg
    )
{
    int arg;
    char * * known_param;
    int num_to_skip;
    int callback_ret;
    int ret;

    *error_string = NULL;

    for(arg=start_arg;arg<argc;arg++)
    {
        /* First check for the parameters that the user recognizes */
        known_param = known_parameters;
        while((*known_param) && strcmp(*known_param, argv[arg]))
        {
            known_param++;
        }
        if ((*known_param) != NULL )
        {
            callback_ret = callback(instance, argc, argv, arg, &num_to_skip, &ret, callback_context);
            if (callback_ret == FCS_CMD_LINE_SKIP)
            {
                arg += num_to_skip-1;
                continue;
            }
            else if (callback_ret == FCS_CMD_LINE_STOP)
            {
                *last_arg = arg;
                return ret;
            }
        }

        if (0)
        {
        }
        else if ((!strcmp(argv[arg], "-md")) || (!strcmp(argv[arg], "--max-depth")))
        {
            arg++;
            if (arg == argc)
            {
                *last_arg = arg;
                return FCS_CMD_LINE_PARAM_WITH_NO_ARG;
            }
            freecell_solver_user_limit_depth(instance, atoi(argv[arg]));
        }
        else if ((!strcmp(argv[arg], "-mi")) || (!strcmp(argv[arg], "--max-iters")))
        {
            arg++;
            if (arg == argc)
            {
                *last_arg = arg;
                return FCS_CMD_LINE_PARAM_WITH_NO_ARG;
            }

            freecell_solver_user_limit_iterations(instance, atoi(argv[arg]));
        }
        else if ((!strcmp(argv[arg], "-to")) || (!strcmp(argv[arg], "--tests-order")))
        {
            char * fcs_user_errstr;
            arg++;
            if (arg == argc)
            {
                *last_arg = arg;
                return FCS_CMD_LINE_PARAM_WITH_NO_ARG;
            }
            ret = freecell_solver_user_set_tests_order(instance, argv[arg], &fcs_user_errstr);
            if (ret != 0)
            {
                char * errstr = malloc(strlen(fcs_user_errstr)+500);
                sprintf(
                    errstr,
                    "Error in tests' order!\n%s\n",
                    fcs_user_errstr
                    );
                free(fcs_user_errstr);

                *error_string = errstr;

                *last_arg = arg;
                return FCS_CMD_LINE_ERROR_IN_ARG;
            }
        }
        else if ((!strcmp(argv[arg], "--freecells-num")))
        {
            arg++;
            if (arg == argc)
            {
                *last_arg = arg;
                return FCS_CMD_LINE_PARAM_WITH_NO_ARG;
            }
            if (freecell_solver_user_set_num_freecells(instance, atoi(argv[arg])) != 0)
            {
                char * errstr;

                errstr = malloc(500);
                sprintf(errstr,
                    "Error! The freecells\' number "
                    "exceeds the maximum of %i.\n"
                    "Recompile the program if you wish to have more.\n",
                    freecell_solver_user_get_max_num_freecells()
                    );

                *error_string = errstr;

                *last_arg = arg;
                return FCS_CMD_LINE_ERROR_IN_ARG;
            }
        }
        else if ((!strcmp(argv[arg], "--stacks-num")))
        {
            arg++;
            if (arg == argc)
            {
                *last_arg = arg;
                return FCS_CMD_LINE_PARAM_WITH_NO_ARG;
            }
            if (freecell_solver_user_set_num_stacks(instance, atoi(argv[arg])) != 0)
            {
                char * errstr;

                errstr = malloc(500);
                sprintf(errstr,
                    "Error! The stacks\' number "
                    "exceeds the maximum of %i.\n"
                    "Recompile the program if you wish to have more.\n",
                    freecell_solver_user_get_max_num_stacks()
                    );

                *error_string = errstr;

                *last_arg = arg;
                return FCS_CMD_LINE_ERROR_IN_ARG;
            }
        }
        else if ((!strcmp(argv[arg], "--decks-num")))
        {
            arg++;
            if (arg == argc)
            {
                *last_arg = arg;
                return FCS_CMD_LINE_PARAM_WITH_NO_ARG;
            }
            if (freecell_solver_user_set_num_decks(instance, atoi(argv[arg])) != 0)
            {
                char * errstr;

                errstr = malloc(500);
                sprintf(errstr,
                    "Error! The decks\' number "
                    "exceeds the maximum of %i.\n"
                    "Recopmile the program if you wish to have more.\n",
                    freecell_solver_user_get_max_num_decks()
                    );

                *error_string = errstr;

                *last_arg = arg;
                return FCS_CMD_LINE_ERROR_IN_ARG;
            }
        }
        else if ((!strcmp(argv[arg], "--sequences-are-built-by")))
        {
            int sbb;

            arg++;
            if (arg == argc)
            {
                *last_arg = arg;
                return FCS_CMD_LINE_PARAM_WITH_NO_ARG;
            }

            if (!strcmp(argv[arg], "suit"))
            {
                sbb = FCS_SEQ_BUILT_BY_SUIT;
            }
            else if (!strcmp(argv[arg], "rank"))
            {
                sbb = FCS_SEQ_BUILT_BY_RANK;
            }
            else
            {
                sbb = FCS_SEQ_BUILT_BY_ALTERNATE_COLOR;
            }
            freecell_solver_user_set_sequences_are_built_by_type(instance, sbb);
        }
        else if ((!strcmp(argv[arg], "--sequence-move")))
        {
            int unlimited;

            arg++;
            if (arg == argc)
            {
                *last_arg = arg;
                return FCS_CMD_LINE_PARAM_WITH_NO_ARG;
            }

            if (!strcmp(argv[arg], "unlimited"))
            {
                unlimited = 1;
            }
            else
            {
                unlimited = 0;
            }
            freecell_solver_user_set_sequence_move(instance, unlimited);
        }
        else if (!strcmp(argv[arg], "--empty-stacks-filled-by"))
        {
            int es_fill;

            arg++;
            if (arg == argc)
            {
                *last_arg = arg;
                return FCS_CMD_LINE_PARAM_WITH_NO_ARG;
            }

            if (!strcmp(argv[arg], "kings"))
            {
                es_fill = FCS_ES_FILLED_BY_KINGS_ONLY;
            }
            else if (!strcmp(argv[arg], "none"))
            {
                es_fill = FCS_ES_FILLED_BY_NONE;
            }
            else
            {
                es_fill = FCS_ES_FILLED_BY_ANY_CARD;
            }
            freecell_solver_user_set_empty_stacks_filled_by(
                instance,
                es_fill
                );
        }
        else if (
            (!strcmp(argv[arg], "--game")) ||
            (!strcmp(argv[arg], "--preset")) ||
            (!strcmp(argv[arg], "-g"))
            )
        {
            arg++;
            if (arg == argc)
            {
                *last_arg = arg;
                return FCS_CMD_LINE_PARAM_WITH_NO_ARG;
            }

            ret = freecell_solver_user_apply_preset(instance, argv[arg]);
            if (ret == FCS_PRESET_CODE_NOT_FOUND)
            {
                char * errstr;

                errstr = malloc(strlen(argv[arg])+500);

                sprintf(errstr, "Unknown game \"%s\"!\n\n", argv[arg]);
                *error_string = errstr;

                *last_arg = arg;
                return FCS_CMD_LINE_ERROR_IN_ARG;
            }
            else if (ret == FCS_PRESET_CODE_FREECELLS_EXCEED_MAX)
            {
                char * errstr;

                errstr = malloc(strlen(argv[arg])+500);
                sprintf(errstr, "The game \"%s\" exceeds the maximal number "
                        "of freecells in the program.\n"
                        "Modify the file \"config.h\" and recompile, "
                        "if you wish to solve one of its boards.\n",
                        argv[arg]
                        );

                *error_string = errstr;

                *last_arg = arg;
                return FCS_CMD_LINE_ERROR_IN_ARG;
            }
            else if (ret == FCS_PRESET_CODE_STACKS_EXCEED_MAX)
            {
                char * errstr;

                errstr = malloc(strlen(argv[arg])+500);

                sprintf(errstr, "The game \"%s\" exceeds the maximal number "
                        "of stacks in the program.\n"
                        "Modify the file \"config.h\" and recompile, "
                        "if you wish to solve one of its boards.\n",
                        argv[arg]
                        );

                *error_string = errstr;

                *last_arg = arg;
                return FCS_CMD_LINE_ERROR_IN_ARG;
            }
            else if (ret != FCS_PRESET_CODE_OK)
            {
                char * errstr;

                errstr = malloc(strlen(argv[arg])+500);

                sprintf(errstr,
                    "The game \"%s\" exceeds the limits of the program.\n"
                    "Modify the file \"config.h\" and recompile, if you wish to solve one of its boards.\n",
                    argv[arg]
                );

                *error_string = errstr;

                *last_arg = arg;
                return FCS_CMD_LINE_ERROR_IN_ARG;
            }
        }
        else if ((!strcmp(argv[arg], "-me")) || (!strcmp(argv[arg], "--method")))
        {
            int method;

            arg++;
            if (arg == argc)
            {
                *last_arg = arg;
                return FCS_CMD_LINE_PARAM_WITH_NO_ARG;
            }
            if (!strcmp(argv[arg], "dfs"))
            {
                method = FCS_METHOD_HARD_DFS;
            }
            else if (!strcmp(argv[arg], "soft-dfs"))
            {
                method = FCS_METHOD_SOFT_DFS;
            }
            else if (!strcmp(argv[arg], "bfs"))
            {
                method = FCS_METHOD_BFS;
            }
            else if (!strcmp(argv[arg], "a-star"))
            {
                method = FCS_METHOD_A_STAR;
            }
            else if (!strcmp(argv[arg], "random-dfs"))
            {
                method = FCS_METHOD_RANDOM_DFS;
            }
            else
            {
                char * errstr;

                errstr = malloc(strlen(argv[arg])+500);

                sprintf(
                    errstr,
                    "Unknown solving method \"%s\".\n",
                    argv[arg]
                    );

                *error_string = errstr;

                *last_arg = arg;
                return FCS_CMD_LINE_ERROR_IN_ARG;
            }

            freecell_solver_user_set_solving_method(instance, method);
        }
        else if ((!strcmp(argv[arg], "-asw")) || (!strcmp(argv[arg], "--a-star-weights")))
        {
            arg++;
            if (arg == argc)
            {
                *last_arg = arg;
                return FCS_CMD_LINE_PARAM_WITH_NO_ARG;
            }
            {
                int a;
                char * start_num;
                char * end_num;
                char save;
                start_num = argv[arg];
                for(a=0;a<5;a++)
                {
                    while ((*start_num > '9') && (*start_num < '0') && (*start_num != '\0'))
                    {
                        start_num++;
                    }
                    if (*start_num == '\0')
                    {
                        break;
                    }
                    end_num = start_num+1;
                    while ((((*end_num >= '0') && (*end_num <= '9')) || (*end_num == '.')) && (*end_num != '\0'))
                    {
                        end_num++;
                    }
                    save = *end_num;
                    *end_num = '\0';
                    freecell_solver_user_set_a_star_weight(
                        instance,
                        a,
                        atof(start_num)
                        );
                    *end_num = save;
                    start_num=end_num+1;
                }
            }
        }
        else if ((!strcmp(argv[arg], "-opt")) || (!strcmp(argv[arg], "--optimize-solution")))
        {
            freecell_solver_user_set_solution_optimization(instance, 1);
        }
        else if ((!strcmp(argv[arg], "-seed")))
        {
            arg++;
            if (arg == argc)
            {
                *last_arg = arg;
                return FCS_CMD_LINE_PARAM_WITH_NO_ARG;
            }

            freecell_solver_user_set_random_seed(instance, atoi(argv[arg]));
        }
        else if ((!strcmp(argv[arg], "-mss")) || (!strcmp(argv[arg], "--max-stored-states")))
        {
            arg++;
            if (arg == argc)
            {
                *last_arg = arg;
                return FCS_CMD_LINE_PARAM_WITH_NO_ARG;
            }

            freecell_solver_user_limit_num_states_in_collection(
                instance,
                atoi(argv[arg])
            );
        }
        else if (
            (!strcmp(argv[arg], "-nst")) ||
            (!strcmp(argv[arg], "--next-soft-thread")) ||
            (!strcmp(argv[arg], "-nht")) ||
            (!strcmp(argv[arg], "--next-hard-thread"))
        )
        {
            int ret;
            int is_st = ((!strcmp(argv[arg], "-nst")) || (!strcmp(argv[arg], "--next-soft-thread")));

            ret =
                is_st ?
                    freecell_solver_user_next_soft_thread(instance) :
                    freecell_solver_user_next_hard_thread(instance)
                    ;

            if (ret)
            {
                char * errstr;

                errstr = strdup("The maximal number of soft threads has been exceeded\n");

                *error_string = errstr;

                *last_arg = arg;

                return FCS_CMD_LINE_ERROR_IN_ARG;
            }
        }
        else if ((!strcmp(argv[arg], "-step")) || (!strcmp(argv[arg], "--soft-thread-step")))
        {
            arg++;
            if (arg == argc)
            {
                *last_arg = arg;
                return FCS_CMD_LINE_PARAM_WITH_NO_ARG;
            }

            freecell_solver_user_set_soft_thread_step(
                instance,
                atoi(argv[arg])
            );
        }
        else if ((!strcmp(argv[arg], "--calc-real-depth")))
        {
            freecell_solver_user_set_calc_real_depth(
                instance,
                1);
        }
        else
        {
            *last_arg = arg;
            return FCS_CMD_LINE_UNRECOGNIZED_OPTION;
        }
    }

    *last_arg = arg;
    return FCS_CMD_LINE_OK;
}
