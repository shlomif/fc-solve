#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>

#include "fcs_user.h"
#include "fcs_cl.h"
#include "cl_chop.h"

#include "prefix.h"

#ifdef DMALLOC
#include "dmalloc.h"
#endif

static int read_preset(const char * preset_name, args_man_t * * args, char * * opened_files_dir_to_assign, const char * user_preset_dir)
{
    int ret_code = 1;
    const char * global_presetrc = NULL, * env_var_presetrc = NULL;
    char * home_dir_presetrc = NULL;
    const char * path;
    const char * * presetrc_pathes[5];
    int path_idx;
    char line[8192];
    FILE * f = NULL;
    char * fgets_ret;
    char * opened_files_dir = NULL;
    int read_next_preset = 0;
    int i=0;

    presetrc_pathes[i++] = &env_var_presetrc;
    presetrc_pathes[i++] = (const char * *)&home_dir_presetrc;
    presetrc_pathes[i++] = &global_presetrc;
    presetrc_pathes[i++] = &user_preset_dir;
    presetrc_pathes[i++] = NULL;

    {
        char * home_dir;
        home_dir = getenv("HOME");
        if (home_dir)
        {
            home_dir_presetrc = malloc(strlen(home_dir) + 50);
            sprintf(home_dir_presetrc,
                "%s/.freecell-solver/presetrc", home_dir
                );
        }
    }
    env_var_presetrc = getenv("FREECELL_SOLVER_PRESETRC");

    global_presetrc = (FREECELL_SOLVER_PKG_DATA_DIR "/presetrc");

    for(path_idx=0;(presetrc_pathes[path_idx] != NULL) ; path_idx++)
    {
        path = (*presetrc_pathes[path_idx]);
        if (path == NULL)
        {
            continue;
        }
        f = fopen(path, "rt");
        if (f == NULL)
        {
            continue;
        }
        while(1)
        {
            fgets_ret = fgets(line, sizeof(line), f);
            if (fgets_ret == NULL)
            {
                break;
            }
            if (!strncmp(line, "dir=", 4))
            {
#define nullify_newline() \
                {     \
                char * s;   \
   \
                s = strchr(line, '\n');      \
                if (s != NULL)     \
                {       \
                    *s = '\0';      \
                }        \
                }
                nullify_newline();

                if (opened_files_dir != NULL)
                {
                    free(opened_files_dir);
                }
                opened_files_dir = strdup(line+4);
            }
            else if (!strncmp(line, "name=", 5))
            {
                nullify_newline();
                if (!strcmp(line+5, preset_name))
                {
                    read_next_preset = 1;
                }
            }
            else if (!strncmp(line, "command=", 8))
            {
                if (read_next_preset)
                {
                    *args = freecell_solver_args_man_alloc();
                    freecell_solver_args_man_chop(*args, line+8);
                    ret_code = 0;
                    goto HAVE_PRESET;
                }
            }
        }
        fclose(f);
        f = NULL;
#undef nullify_newline
    }
HAVE_PRESET:

    if (f)
    {
        fclose(f);
    }

    if (home_dir_presetrc)
    {
        free(home_dir_presetrc);
    }

    if (ret_code == 0)
    {
        *opened_files_dir_to_assign = opened_files_dir;
    }
    else
    {
        if (opened_files_dir)
        {
            free(opened_files_dir);
        }
    }

    return ret_code;
}


int freecell_solver_user_cmd_line_parse_args_with_file_nesting_count(
    void * instance,
    int argc,
    const char * argv[],
    int start_arg,
    const char * * known_parameters,
    freecell_solver_user_cmd_line_known_commands_callback_t callback,
    void * callback_context,
    char * * error_string,
    int * last_arg,
    int file_nesting_count,
    char * opened_files_dir
    )
{
    int arg;
    const char * * known_param;
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
                *last_arg = arg-1;
                return FCS_CMD_LINE_PARAM_WITH_NO_ARG;
            }
            freecell_solver_user_limit_depth(instance, atoi(argv[arg]));
        }
        else if ((!strcmp(argv[arg], "-mi")) || (!strcmp(argv[arg], "--max-iters")))
        {
            arg++;
            if (arg == argc)
            {
                *last_arg = arg-1;
                return FCS_CMD_LINE_PARAM_WITH_NO_ARG;
            }

            freecell_solver_user_limit_current_instance_iterations(instance, atoi(argv[arg]));
        }
        else if ((!strcmp(argv[arg], "-to")) || (!strcmp(argv[arg], "--tests-order")))
        {
            char * fcs_user_errstr;
            arg++;
            if (arg == argc)
            {
                *last_arg = arg-1;
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
                *last_arg = arg-1;
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
                *last_arg = arg-1;
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
                *last_arg = arg-1;
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
                *last_arg = arg-1;
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
                *last_arg = arg-1;
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
                *last_arg = arg-1;
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
                *last_arg = arg-1;
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
                *last_arg = arg-1;
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
                *last_arg = arg-1;
                return FCS_CMD_LINE_PARAM_WITH_NO_ARG;
            }
            {
                int a;
                const char * start_num;
                const char * end_num;
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
                *last_arg = arg-1;
                return FCS_CMD_LINE_PARAM_WITH_NO_ARG;
            }

            freecell_solver_user_set_random_seed(instance, atoi(argv[arg]));
        }
        else if ((!strcmp(argv[arg], "-mss")) || (!strcmp(argv[arg], "--max-stored-states")))
        {
            arg++;
            if (arg == argc)
            {
                *last_arg = arg-1;
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
                *last_arg = arg-1;
                return FCS_CMD_LINE_PARAM_WITH_NO_ARG;
            }

            freecell_solver_user_set_soft_thread_step(
                instance,
                atoi(argv[arg])
            );
        }
        else if ((!strcmp(argv[arg], "--reparent-states")))
        {
            freecell_solver_user_set_reparent_states(
                instance,
                1
                );
        }
        else if ((!strcmp(argv[arg], "--calc-real-depth")))
        {
            freecell_solver_user_set_calc_real_depth(
                instance,
                1);
        }
        else if ((!strcmp(argv[arg], "--st-name")))
        {
            arg++;
            if (arg == argc)
            {
                *last_arg = arg-1;
                return FCS_CMD_LINE_PARAM_WITH_NO_ARG;
            }
            freecell_solver_user_set_soft_thread_name(instance, argv[arg]);
        }
        else if ((!strcmp(argv[arg], "--prelude")))
        {
            arg++;
            if (arg == argc)
            {
                *last_arg = arg-1;
                return FCS_CMD_LINE_PARAM_WITH_NO_ARG;
            }
            freecell_solver_user_set_hard_thread_prelude(instance, argv[arg]);
        }
        else if ((!strcmp(argv[arg], "-opt-to")) || (!strcmp(argv[arg], "--optimization-tests-order")))
        {
            char * fcs_user_errstr;
            arg++;
            if (arg == argc)
            {
                *last_arg = arg-1;
                return FCS_CMD_LINE_PARAM_WITH_NO_ARG;
            }

            ret = freecell_solver_user_set_optimization_scan_tests_order(
                    instance,
                    argv[arg],
                    &fcs_user_errstr
                    );

            if (ret != 0)
            {
                char * errstr = malloc(strlen(fcs_user_errstr)+500);
                sprintf(
                    errstr,
                    "Error in the optimization scan's tests' order!\n%s\n",
                    fcs_user_errstr
                    );
                free(fcs_user_errstr);

                *error_string = errstr;

                *last_arg = arg;
                return FCS_CMD_LINE_ERROR_IN_ARG;
            }
        }
        else if ((!strcmp(argv[arg], "--scans-synergy")))
        {
            int value;

            arg++;
            if (arg == argc)
            {
                *last_arg = arg-1;
                return FCS_CMD_LINE_PARAM_WITH_NO_ARG;
            }

            if (!strcmp(argv[arg], "none"))
            {
                value = 0;
            }
            else if (!strcmp(argv[arg], "dead-end-marks"))
            {
                value = 1;
            }
            else
            {
                char * errstr;

                errstr = malloc(strlen(argv[arg])+500);

                sprintf(errstr, "Unknown scans' synergy type \"%s\"!\n", argv[arg]);
                *last_arg = arg;
                *error_string = errstr;
                return FCS_CMD_LINE_ERROR_IN_ARG;
            }

            freecell_solver_user_set_scans_synergy(
                instance,
                value
                );
        }
        else if ((!strcmp(argv[arg], "-ni")) ||
                 (!strcmp(argv[arg], "--next-instance")))
        {
            freecell_solver_user_next_instance(instance);
        }
        else if (!strcmp(argv[arg], "--reset"))
        {
            freecell_solver_user_reset(instance);
        }
        else if (!strcmp(argv[arg], "--read-from-file"))
        {
            arg++;
            if (arg == argc)
            {
                *last_arg = arg-1;
                return FCS_CMD_LINE_PARAM_WITH_NO_ARG;
            }
            if (file_nesting_count == 0)
            {
                /* do nothing */
            }
            else
            {
                int num_to_skip = 0;
                char * s, * buffer;
                FILE * f;
                long file_len;
                int ret;
                size_t num_read;
                args_man_t * args_man;

                s = argv[arg];
                while(isdigit(*s))
                {
                    s++;
                }
                if (*s == ',')
                {
                    num_to_skip = atoi(argv[arg]);
                    s++;
                }
                if (opened_files_dir)
                {
                    char * complete_path;

                    complete_path = malloc(strlen(opened_files_dir)+strlen(s)+1);
                    sprintf(complete_path, "%s%s", opened_files_dir, s);
                    f = fopen(complete_path, "rt");
                    free(complete_path);
                }
                if (f == NULL)
                {
                    f = fopen(s, "rt");
                }
                if (f == NULL)
                {
                    char * err_str;

                    err_str = malloc(strlen(s)+100);
                    sprintf(err_str,
                            "Could not open file \"%s\"!\nQuitting.\n",
                            s);

                    *error_string = err_str;
                    *last_arg = arg;

                    return FCS_CMD_LINE_ERROR_IN_ARG;
                }
                fseek(f, 0, SEEK_END);
                file_len = ftell(f);
                buffer=malloc(file_len+1);
                if (buffer == NULL)
                {
                    *error_string = strdup("Could not allocate enough memory to parse the file. Quitting.\n");
                    fclose(f);

                    *last_arg = arg;

                    return FCS_CMD_LINE_ERROR_IN_ARG;
                }
                fseek(f,0,SEEK_SET);
                num_read = fread(buffer, 1, file_len, f);
                fclose(f);
                buffer[num_read] = '\0';

                args_man = freecell_solver_args_man_alloc();
                ret = freecell_solver_args_man_chop(args_man, buffer);
                free(buffer);
                if (ret != 0)
                {
                    *error_string =
                        strdup("Could not parse the file. Quitting\n");
                    freecell_solver_args_man_free(args_man);

                    *last_arg = arg;

                    return FCS_CMD_LINE_ERROR_IN_ARG;
                }

                if (num_to_skip >= args_man->argc)
                {
                    /* Do nothing */
                }
                else
                {
                    ret = freecell_solver_user_cmd_line_parse_args_with_file_nesting_count(
                        instance,
                        args_man->argc - num_to_skip,
                        args_man->argv + num_to_skip,
                        0,
                        known_parameters,
                        callback,
                        callback_context,
                        error_string,
                        last_arg,
                        ((file_nesting_count < 0) ? file_nesting_count : (file_nesting_count-1)),
                        opened_files_dir
                        );

                    if (ret == FCS_CMD_LINE_UNRECOGNIZED_OPTION)
                    {
                        /* Do nothing - continue */
                    }
                    else if (ret != FCS_CMD_LINE_OK)
                    {
                        freecell_solver_args_man_free(args_man);
                        return ret;
                    }
                }
                freecell_solver_args_man_free(args_man);
            }
        }
        else if ((!strcmp(argv[arg], "-l")) || (!strcmp(argv[arg], "--load-config")))
        {
            arg++;
            if (arg == argc)
            {
                *last_arg = arg-1;
                return FCS_CMD_LINE_PARAM_WITH_NO_ARG;
            }
            {
                int status;
                args_man_t * preset_args;
                char * dir = NULL;

                status = read_preset(argv[arg], &preset_args, &dir, NULL);
                if (status != 0)
                {
                    char * err_str;
                    err_str = malloc(strlen(argv[arg]) + 100);
                    sprintf(err_str, "Unable to load the \"%s\" configuration!\n", argv[arg]);
                    *error_string = err_str;

                    *last_arg = arg;

                    return FCS_CMD_LINE_ERROR_IN_ARG;
                }
                else
                {
                    ret = freecell_solver_user_cmd_line_parse_args_with_file_nesting_count(
                        instance,
                        preset_args->argc,
                        preset_args->argv,
                        0,
                        known_parameters,
                        callback,
                        callback_context,
                        error_string,
                        last_arg,
                        ((file_nesting_count < 0) ? file_nesting_count : (file_nesting_count-1)),
                        dir ? dir : opened_files_dir
                        );

                    if (dir)
                    {
                        free(dir);
                    }
                    freecell_solver_args_man_free(preset_args);

                    if (ret == FCS_CMD_LINE_UNRECOGNIZED_OPTION)
                    {
                        /* Do nothing - continue */
                    }
                    else if (ret != FCS_CMD_LINE_OK)
                    {
                        return ret;
                    }
                }
            }
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
    return freecell_solver_user_cmd_line_parse_args_with_file_nesting_count(
        instance,
        argc,
        argv,
        start_arg,
        known_parameters,
        callback,
        callback_context,
        error_string,
        last_arg,
        -1,
        NULL
        );
}

