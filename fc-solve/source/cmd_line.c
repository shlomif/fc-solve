/*
 * This file is part of Freecell Solver. It is subject to the license terms in
 * the COPYING.txt file found in the top-level directory of this distribution
 * and at http://fc-solve.shlomifish.org/docs/distro/COPYING.html . No part of
 * Freecell Solver, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the COPYING file.
 *
 * Copyright (c) 2000 Shlomi Fish
 */
/*
 * cmd_line.c - the Freecell Solver command line arguments-like parsing
 * routines. Useful for more easily configuring a Freecell Solver instance.
 */

#include "dll_thunk.h"

#include "config.h"

#ifdef HAVE_VASPRINTF
#define _GNU_SOURCE
#else
#include "asprintf.h"
#endif

#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>
#include <ctype.h>
#ifdef _WIN32
#include <windows.h>
#endif

#include "set_weights.h"
#include "fcs_user.h"
#include "fcs_cl.h"
#include "split_cmd_line.h"

#include "prefix.h"
#include "cmd_line_enum.h"
#include "cmd_line_inc.h"

static GCC_INLINE void nullify_newline(char *const line)
{
    char *const s = strchr(line, '\n');

    if (s)
    {
        (*s) = '\0';
    }
}

#define MAX_PATH_LEN 4000
static GCC_INLINE fcs_bool_t read_preset(const char *const preset_name,
    args_man_t *const args_man, char *const opened_files_dir,
    const char *const user_preset_dir)
{
    char home_dir_presetrc[MAX_PATH_LEN];
    char *const home_dir = getenv("HOME");
    if (home_dir)
    {
        snprintf(home_dir_presetrc, MAX_PATH_LEN,
            "%s/.freecell-solver/presetrc", home_dir);
    }
    const char *global_presetrc;
    char *env_var_presetrc = getenv("FREECELL_SOLVER_PRESETRC");

#ifdef _WIN32

    char windows_exe_dir[MAX_PATH] = "";
    char windows_global_presetc[MAX_PATH + 100] = "";

    /* Will contain exe path */
    const HMODULE hModule = GetModuleHandle(NULL);
    if (hModule != NULL)
    {
        /* When passing NULL to GetModuleHandle, it returns handle of exe itself
         */
        GetModuleFileName(hModule, windows_exe_dir, (sizeof(windows_exe_dir)));
        {
            /* Remove the basename and keep only the dirname. */
            char *s = windows_exe_dir + strlen(windows_exe_dir) - 1;
            while ((s > windows_exe_dir) && (!((*s == '/') || (*s == '\\'))))
            {
                s--;
            }
            if (s > windows_exe_dir)
            {
                *s = '\0';
            }
        }
        sprintf(windows_global_presetc, "%s/../share/freecell-solver/presetrc",
            windows_exe_dir);
        global_presetrc = windows_global_presetc;
    }
    else
    {
        global_presetrc = NULL;
    }

#else
    global_presetrc = (FREECELL_SOLVER_PKG_DATA_DIR "/presetrc");
#endif
    const char *const presetrc_pathes[4] = {
        (const char *)env_var_presetrc,
        (home_dir ? ((const char *)home_dir_presetrc) : NULL),
        (const char *)global_presetrc, user_preset_dir,
    };
    fcs_bool_t read_next_preset = FALSE;
    for (size_t path_idx = 0; path_idx < COUNT(presetrc_pathes); path_idx++)
    {
        const char *const path = presetrc_pathes[path_idx];
        if (path == NULL)
        {
            continue;
        }
        FILE *f = fopen(path, "rt");
        if (f == NULL)
        {
            continue;
        }
        while (1)
        {
            char line[8192];
            if (fgets(line, sizeof(line), f) == NULL)
            {
                break;
            }
            const char *s;
            if ((s = try_str_prefix(line, "dir=")))
            {
                nullify_newline(line);

#ifdef _WIN32
                const char *const after_prefix =
                    try_str_prefix(s, "${EXE_DIRNAME}");
                if (after_prefix)
                {
                    snprintf(opened_files_dir, MAX_PATH_LEN, "%s%s",
                        windows_exe_dir, after_prefix);
                }
                else
                {
                    strncpy(opened_files_dir, s, MAX_PATH_LEN);
                    opened_files_dir[MAX_PATH_LEN - 1] = '\0';
                }
#else
                strncpy(opened_files_dir, s, MAX_PATH_LEN);
                opened_files_dir[MAX_PATH_LEN - 1] = '\0';
#endif
            }
            else if ((s = try_str_prefix(line, "name=")))
            {
                nullify_newline(line);
                if (!strcmp(s, preset_name))
                {
                    read_next_preset = TRUE;
                }
            }
            else if ((s = try_str_prefix(line, "command=")))
            {
                if (read_next_preset)
                {
                    *args_man = fc_solve_args_man_chop(s);
                    fclose(f);
                    return FALSE;
                }
            }
        }
        fclose(f);
    }
    return TRUE;
}

static GCC_INLINE char *calc_errstr_s(const char *const format, ...)
{
    va_list my_va_list;

    va_start(my_va_list, format);

    char *errstr;

    vasprintf(&errstr, format, my_va_list);

    va_end(my_va_list);

    return errstr;
}

#ifdef FCS_WITH_ERROR_STRS
#define ASSIGN_ERR_STR(error_string, format, ...)                              \
    *(error_string) = calc_errstr_s(format, __VA_ARGS__)
#define ASSIGN_ERR_STR_AND_FREE(fcs_user_errstr, error_string, format, ...)    \
    {                                                                          \
        ASSIGN_ERR_STR(error_string, format, __VA_ARGS__);                     \
        free(fcs_user_errstr);                                                 \
    }
#else
#define ASSIGN_ERR_STR(error_string, format, ...)
#define ASSIGN_ERR_STR_AND_FREE(fcs_user_errstr, error_string, format, ...)
#endif

DLLEXPORT int freecell_solver_user_cmd_line_read_cmd_line_preset(
    void *const instance, const char *const preset_name,
    freecell_solver_str_t *const known_parameters FCS__PASS_ERR_STR(
        char **const error_string),
    const int file_nesting_count, freecell_solver_str_t opened_files_dir)
{
    args_man_t preset_args;
    char dir[MAX_PATH_LEN];

    dir[0] = '\0';

    if (read_preset(preset_name, &preset_args, dir, NULL))
    {
        ASSIGN_ERR_STR(error_string, "%s", "Could not read preset.");
        return FCS_CMD_LINE_ERROR_IN_ARG;
    }
    else
    {
        int last_arg = 0;

        const int ret =
            freecell_solver_user_cmd_line_parse_args_with_file_nesting_count(
                instance, preset_args.argc,
                (freecell_solver_str_t *)(void *)(preset_args.argv), 0,
                known_parameters, NULL, NULL FCS__PASS_ERR_STR(error_string),
                &(last_arg),
                ((file_nesting_count < 0) ? file_nesting_count
                                          : (file_nesting_count - 1)),
                dir[0] ? dir : opened_files_dir);

        fc_solve_args_man_free(&preset_args);

        return ret;
    }
}

DLLEXPORT int freecell_solver_user_cmd_line_parse_args_with_file_nesting_count(
    void *instance, int argc, freecell_solver_str_t argv[], const int start_arg,
    freecell_solver_str_t *known_parameters,
    freecell_solver_user_cmd_line_known_commands_callback_t callback,
    void *const callback_context FCS__PASS_ERR_STR(char **error_string),
    int *last_arg, const int file_nesting_count,
    freecell_solver_str_t opened_files_dir)
{
#ifdef FCS_WITH_ERROR_STRS
    char *fcs_user_errstr;
    *error_string = NULL;
#endif

    const freecell_solver_str_t *const arg_argc = &(argv[argc]);

    freecell_solver_str_t *arg = &(argv[start_arg]);
    for (; arg < arg_argc; arg++)
    {
        /* First check for the parameters that the user recognizes */
        if (known_parameters)
        {
            var_AUTO(known_param, known_parameters);
            while ((*known_param) && strcmp(*known_param, (*arg)))
            {
                known_param++;
            }
            if (*known_param)
            {
                int ret, num_to_skip;
                switch (callback(instance, argc, argv, arg - &(argv[0]),
                    &num_to_skip, &ret, callback_context))
                {
                case FCS_CMD_LINE_SKIP:
                    arg += num_to_skip - 1;
                    goto end_of_arg_loop;

                case FCS_CMD_LINE_STOP:
                    *last_arg = arg - &(argv[0]);
                    return ret;
                }
            }
        }

        /* OPT-PARSE-START */
        const int opt = ({
            const_AUTO(p, (*arg));
            const_AUTO(word, in_word_set(p, strlen(p)));
            word ? word->OptionCode : FCS_OPT_UNRECOGNIZED;
        });

        switch (opt)
        {
        case FCS_OPT_UNRECOGNIZED:
            *last_arg = arg - &(argv[0]);
            return FCS_CMD_LINE_UNRECOGNIZED_OPTION;

#define PROCESS_OPT_ARG()                                                      \
    {                                                                          \
        arg++;                                                                 \
        if (arg == arg_argc)                                                   \
        {                                                                      \
            *last_arg = arg - &(argv[0]);                                      \
            return FCS_CMD_LINE_PARAM_WITH_NO_ARG;                             \
        }                                                                      \
    }

#define RET_ERROR_IN_ARG()                                                     \
    *last_arg = arg - &(argv[0]);                                              \
    return FCS_CMD_LINE_ERROR_IN_ARG;

        case FCS_OPT_MAX_DEPTH: /* STRINGS=-md|--max-depth; */
        {
            PROCESS_OPT_ARG();
        }
        break;

        case FCS_OPT_MAX_ITERS: /* STRINGS=-mi|--max-iters; */
        {
            PROCESS_OPT_ARG();

            freecell_solver_user_limit_iterations_long(instance, atol((*arg)));
        }
        break;

        case FCS_OPT_TESTS_ORDER: /* STRINGS=-to|--tests-order; */
        {
            PROCESS_OPT_ARG();

            if (freecell_solver_user_set_tests_order(
                    instance, (*arg)FCS__PASS_ERR_STR(&fcs_user_errstr)) != 0)
            {
                ASSIGN_ERR_STR_AND_FREE(fcs_user_errstr, error_string,
                    "Error in tests' order!\n%s\n", fcs_user_errstr);
                RET_ERROR_IN_ARG();
            }
        }
        break;

        case FCS_OPT_FREECELLS_NUM: /* STRINGS=--freecells-num; */
        {
            PROCESS_OPT_ARG();
#ifndef HARD_CODED_NUM_FREECELLS
            if (freecell_solver_user_set_num_freecells(
                    instance, atoi((*arg))) != 0)
            {
                ASSIGN_ERR_STR(error_string,
                    "Error! The freecells\' number "
                    "exceeds the maximum of %i.\n"
                    "Recompile the program if you wish to have more.\n",
                    freecell_solver_user_get_max_num_freecells());
                RET_ERROR_IN_ARG();
            }
#endif
        }
        break;

        case FCS_OPT_STACKS_NUM: /* STRINGS=--stacks-num; */
        {
            PROCESS_OPT_ARG();
#ifndef HARD_CODED_NUM_STACKS
            if (freecell_solver_user_set_num_stacks(instance, atoi((*arg))) !=
                0)
            {
                ASSIGN_ERR_STR(error_string,
                    "Error! The stacks\' number "
                    "exceeds the maximum of %i.\n"
                    "Recompile the program if you wish to have more.\n",
                    freecell_solver_user_get_max_num_stacks());
                RET_ERROR_IN_ARG();
            }
#endif
        }
        break;

        case FCS_OPT_DECKS_NUM: /* STRINGS=--decks-num; */
        {
            PROCESS_OPT_ARG();
#ifndef HARD_CODED_NUM_DECKS
            if (freecell_solver_user_set_num_decks(instance, atoi((*arg))) != 0)
            {
                ASSIGN_ERR_STR(error_string,
                    "Error! The decks\' number "
                    "exceeds the maximum of %i.\n"
                    "Recopmile the program if you wish to have more.\n",
                    freecell_solver_user_get_max_num_decks());
                RET_ERROR_IN_ARG();
            }
#endif
        }
        break;

        case FCS_OPT_SEQUENCES_ARE_BUILT_BY: /* STRINGS=--sequences-are-built-by;
                                                */
        {
            PROCESS_OPT_ARG();
#ifndef FCS_FREECELL_ONLY
            const_AUTO(p, (*arg));
            freecell_solver_user_set_sequences_are_built_by_type(instance,
                (!strcmp(p, "suit"))
                    ? FCS_SEQ_BUILT_BY_SUIT
                    : (!strcmp(p, "rank")) ? FCS_SEQ_BUILT_BY_RANK
                                           : FCS_SEQ_BUILT_BY_ALTERNATE_COLOR);
#endif
        }
        break;

        case FCS_OPT_SEQUENCE_MOVE: /* STRINGS=--sequence-move; */
        {
            PROCESS_OPT_ARG();
#ifndef FCS_FREECELL_ONLY
            freecell_solver_user_set_sequence_move(
                instance, !strcmp((*arg), "unlimited"));
#endif
        }
        break;

        case FCS_OPT_EMPTY_STACKS_FILLED_BY: /* STRINGS=--empty-stacks-filled-by;
                                                */
        {

            PROCESS_OPT_ARG();
#ifndef FCS_FREECELL_ONLY
            const char *const s = (*arg);
            freecell_solver_user_set_empty_stacks_filled_by(instance,
                (!strcmp(s, "kings"))
                    ? FCS_ES_FILLED_BY_KINGS_ONLY
                    : (!strcmp(s, "none")) ? FCS_ES_FILLED_BY_NONE
                                           : FCS_ES_FILLED_BY_ANY_CARD);
#endif
        }
        break;

        case FCS_OPT_GAME: /* STRINGS=--game|--preset|-g; */
        {
            PROCESS_OPT_ARG();

#ifndef FCS_FREECELL_ONLY
            const char *const string_arg = (*arg);
            switch (freecell_solver_user_apply_preset(instance, string_arg))
            {
            case FCS_PRESET_CODE_OK:
                break;

            case FCS_PRESET_CODE_NOT_FOUND:
                ASSIGN_ERR_STR(
                    error_string, "Unknown game \"%s\"!\n\n", string_arg);
                RET_ERROR_IN_ARG();

            case FCS_PRESET_CODE_FREECELLS_EXCEED_MAX:
                ASSIGN_ERR_STR(error_string,
                    "The game \"%s\" exceeds the maximal number "
                    "of freecells in the program.\n"
                    "Modify the file \"config.h\" and recompile, "
                    "if you wish to solve one of its boards.\n",
                    string_arg);
                RET_ERROR_IN_ARG();

            case FCS_PRESET_CODE_STACKS_EXCEED_MAX:
                ASSIGN_ERR_STR(error_string,
                    "The game \"%s\" exceeds the maximal number "
                    "of stacks in the program.\n"
                    "Modify the file \"config.h\" and recompile, "
                    "if you wish to solve one of its boards.\n",
                    string_arg);
                RET_ERROR_IN_ARG();

            default:
                ASSIGN_ERR_STR(error_string,
                    "The game \"%s\" exceeds the limits of the program.\n"
                    "Modify the file \"config.h\" and recompile, if you wish "
                    "to solve one of its boards.\n",
                    string_arg);
                RET_ERROR_IN_ARG();
            }
#endif
        }
        break;

        case FCS_OPT_METHOD: /* STRINGS=-me|--method; */
        {
            int int_method;

            PROCESS_OPT_ARG();

            const_AUTO(s, (*arg));
            if (!strcmp(s, "soft-dfs") || !strcmp(s, "dfs"))
            {
                int_method = FCS_METHOD_SOFT_DFS;
            }
            else if (!strcmp(s, "bfs"))
            {
                int_method = FCS_METHOD_BFS;
            }
            else if (!strcmp(s, "a-star"))
            {
                int_method = FCS_METHOD_A_STAR;
            }
            else if (!strcmp(s, "random-dfs"))
            {
                int_method = FCS_METHOD_RANDOM_DFS;
            }
#ifndef FCS_DISABLE_PATSOLVE
            else if (!strcmp(s, "patsolve"))
            {
                int_method = FCS_METHOD_PATSOLVE;
            }
#endif
            else
            {
                ASSIGN_ERR_STR(
                    error_string, "Unknown solving method \"%s\".\n", s);

                RET_ERROR_IN_ARG();
            }

            freecell_solver_user_set_solving_method(instance, int_method);
        }
        break;

        case FCS_OPT_BEFS_WEIGHTS: /* STRINGS=-asw|--a-star-weights; */
        {
            PROCESS_OPT_ARG();

            {
                double befs_weights[FCS_NUM_BEFS_WEIGHTS];

                const char *const s = *arg;

                fc_solve_set_weights(s, strchr(s, '\0'), befs_weights);

                for (int i = 0; i < FCS_NUM_BEFS_WEIGHTS; i++)
                {
                    freecell_solver_user_set_a_star_weight(
                        instance, i, befs_weights[i]);
                }
            }
        }
        break;

        case FCS_OPT_OPTIMIZE_SOLUTION: /* STRINGS=-opt|--optimize-solution; */
        {
            freecell_solver_user_set_solution_optimization(instance, 1);
        }
        break;

        case FCS_OPT_SEED: /* STRINGS=-seed; */
        {
            PROCESS_OPT_ARG();

            freecell_solver_user_set_random_seed(instance, atoi((*arg)));
        }
        break;

        case FCS_OPT_MAX_STORED_STATES: /* STRINGS=-mss|--max-stored-states; */
        {
            PROCESS_OPT_ARG();

            freecell_solver_user_limit_num_states_in_collection_long(
                instance, atol((*arg)));
        }
        break;

        case FCS_OPT_TRIM_MAX_STORED_STATES: /* STRINGS=-tmss|--trim-max-stored-states;
                                                */
        {
            PROCESS_OPT_ARG();

#ifndef FCS_WITHOUT_TRIM_MAX_STORED_STATES
            freecell_solver_set_stored_states_trimming_limit(
                instance, atol((*arg)));
#endif
        }
        break;

        case FCS_OPT_NEXT_INSTANCE: /* STRINGS=-ni|--next-instance; */
        {
            freecell_solver_user_next_instance(instance);
        }
        break;

        case FCS_OPT_NEXT_FLARE: /* STRINGS=-nf|--next-flare; */
#ifdef FCS_WITH_FLARES
        {
            freecell_solver_user_next_flare(instance);
        }
#endif
        break;

        case FCS_OPT_NEXT_SOFT_THREAD: /* STRINGS=-nst|--next-soft-thread; */
        case FCS_OPT_NEXT_HARD_THREAD: /* STRINGS=-nht|--next-hard-thread; */
        {
            if ((opt == FCS_OPT_NEXT_SOFT_THREAD)
                    ? freecell_solver_user_next_soft_thread(instance)
                    : freecell_solver_user_next_hard_thread(instance))
            {
                ASSIGN_ERR_STR(error_string, "%s",
                    "The maximal number of soft threads has been exceeded\n");
                RET_ERROR_IN_ARG();
            }
        }
        break;

        case FCS_OPT_SOFT_THREAD_STEP: /* STRINGS=-step|--soft-thread-step; */
        {
            PROCESS_OPT_ARG();

            freecell_solver_user_set_soft_thread_step(instance, atoi((*arg)));
        }
        break;

        case FCS_OPT_REPARENT_STATES: /* STRINGS=--reparent-states; */
#ifndef FCS_HARD_CODE_REPARENT_STATES_AS_FALSE
            freecell_solver_user_set_reparent_states(instance, 1);
#endif
            break;

        case FCS_OPT_CALC_REAL_DEPTH: /* STRINGS=--calc-real-depth; */
#ifndef FCS_HARD_CODE_CALC_REAL_DEPTH_AS_FALSE
            freecell_solver_user_set_calc_real_depth(instance, 1);
#endif
            break;

        case FCS_OPT_ST_NAME: /* STRINGS=--st-name; */
        {
            PROCESS_OPT_ARG();

            freecell_solver_user_set_soft_thread_name(instance, (*arg));
        }
        break;

        case FCS_OPT_FLARE_NAME: /* STRINGS=--flare-name; */
        {
            PROCESS_OPT_ARG();
#ifdef FCS_WITH_FLARES
            freecell_solver_user_set_flare_name(instance, (*arg));
#endif
        }
        break;

        case FCS_OPT_PRELUDE: /* STRINGS=--prelude; */
        {
            PROCESS_OPT_ARG();

            freecell_solver_user_set_hard_thread_prelude(instance, (*arg));
        }
        break;

        case FCS_OPT_FLARES_PLAN: /* STRINGS=--flares-plan; */
        {
            PROCESS_OPT_ARG();
#ifdef FCS_WITH_FLARES
            freecell_solver_user_set_flares_plan(instance, (*arg));
#endif
        }
        break;

        case FCS_OPT_FLARES_ITERS_FACTOR: /* STRINGS=-fif|--flares-iters-factor;
                                             */
        {
            PROCESS_OPT_ARG();
#ifdef FCS_WITH_FLARES
            freecell_solver_user_set_flares_iters_factor(instance, atof(*arg));
#endif
        }
        break;

        case FCS_OPT_OPTIMIZATION_TESTS_ORDER: /* STRINGS=-opt-to|--optimization-tests-order;
                                                  */
        {
            PROCESS_OPT_ARG();
            if (freecell_solver_user_set_optimization_scan_tests_order(
                    instance, (*arg)FCS__PASS_ERR_STR(&fcs_user_errstr)) != 0)
            {
                ASSIGN_ERR_STR_AND_FREE(fcs_user_errstr, error_string,
                    "Error in the optimization scan's tests' order!\n%s\n",
                    fcs_user_errstr);
                RET_ERROR_IN_ARG();
            }
        }
        break;

        case FCS_OPT_SCANS_SYNERGY: /* STRINGS=--scans-synergy; */
        {
            PROCESS_OPT_ARG();
#ifndef FCS_HARD_CODE_SCANS_SYNERGY_AS_TRUE
            int value;

            if (!strcmp((*arg), "none"))
            {
                value = 0;
            }
            else if (!strcmp((*arg), "dead-end-marks"))
            {
                value = 1;
            }
            else
            {
                ASSIGN_ERR_STR(error_string,
                    "Unknown scans' synergy type \"%s\"!\n", (*arg));

                RET_ERROR_IN_ARG();
            }

            freecell_solver_user_set_scans_synergy(instance, value);
#endif
        }
        break;

        case FCS_OPT_RESET: /* STRINGS=--reset; */
        {
            freecell_solver_user_reset(instance);
        }
        break;

        case FCS_OPT_READ_FROM_FILE: /* STRINGS=--read-from-file; */
        {
            PROCESS_OPT_ARG();

            if (file_nesting_count == 0)
            {
                /* do nothing */
            }
            else
            {
                freecell_solver_str_t s;
                FILE *f;
                int num_file_args_to_skip = 0;
                s = (*arg);
                while (isdigit(*s))
                {
                    s++;
                }
                if (*s == ',')
                {
                    num_file_args_to_skip = atoi((*arg));
                    s++;
                }

                if (opened_files_dir)
                {
                    char
                        complete_path[strlen(opened_files_dir) + strlen(s) + 1];
                    sprintf(complete_path, "%s%s", opened_files_dir, s);
                    f = fopen(complete_path, "rt");
                }
                else
                {
                    /*
                     * Initialize f to NULL so it will be initialized
                     * */
                    f = NULL;
                }

                /* Try to open from the local path */
                if (f == NULL)
                {
                    f = fopen(s, "rt");
                }

                /* If we still could not open it return an error */
                if (f == NULL)
                {
                    ASSIGN_ERR_STR(error_string,
                        "Could not open file \"%s\"!\nQuitting.\n", s);
                    RET_ERROR_IN_ARG();
                }
                fseek(f, 0, SEEK_END);
                const long file_len = ftell(f);
                char *buffer = SMALLOC(buffer, file_len + 1);
                if (buffer == NULL)
                {
                    ASSIGN_ERR_STR(error_string, "%s",
                        "Could not allocate enough memory "
                        "to parse the file. Quitting.\n");
                    fclose(f);

                    RET_ERROR_IN_ARG();
                }
                fseek(f, 0, SEEK_SET);
                buffer[fread(buffer, 1, file_len, f)] = '\0';
                fclose(f);

                args_man_t args_man = fc_solve_args_man_chop(buffer);
                free(buffer);

                if (num_file_args_to_skip < args_man.argc)
                {
                    const int ret =
                        freecell_solver_user_cmd_line_parse_args_with_file_nesting_count(
                            instance, args_man.argc - num_file_args_to_skip,
                            (freecell_solver_str_t *)(void
                                    *)(args_man.argv + num_file_args_to_skip),
                            0, known_parameters, callback,
                            callback_context FCS__PASS_ERR_STR(error_string),
                            last_arg, ((file_nesting_count < 0)
                                              ? file_nesting_count
                                              : (file_nesting_count - 1)),
                            opened_files_dir);

                    if (!((ret == FCS_CMD_LINE_UNRECOGNIZED_OPTION) ||
                            (ret == FCS_CMD_LINE_OK)))
                    {
                        /* So we don't give a last_arg to the read file.*/
                        *last_arg = arg - &(argv[0]);
                        fc_solve_args_man_free(&args_man);
                        return ret;
                    }
                }
                fc_solve_args_man_free(&args_man);
            }
        }
        break;

        case FCS_OPT_LOAD_CONFIG: /* STRINGS=-l|--load-config; */
        {
            PROCESS_OPT_ARG();

            const int ret =
                freecell_solver_user_cmd_line_read_cmd_line_preset(instance,
                    (*arg), known_parameters FCS__PASS_ERR_STR(error_string),
                    file_nesting_count, opened_files_dir);

            switch (ret)
            {
            case FCS_CMD_LINE_ERROR_IN_ARG:
                ASSIGN_ERR_STR(error_string,
                    "Unable to load the \"%s\" configuration!\n", (*arg));
                RET_ERROR_IN_ARG();

            case FCS_CMD_LINE_UNRECOGNIZED_OPTION:
            case FCS_CMD_LINE_OK:
                break;

            default:
                return ret;
            }
        }
        break;

        case FCS_OPT_DEPTH_TESTS_ORDER: /* STRINGS=-dto|--depth-tests-order; */
        case FCS_OPT_DEPTH_TESTS_ORDER_2: /* STRINGS=-dto2|--depth-tests-order2;
                                             */
        {
            PROCESS_OPT_ARG();

            {
                int min_depth = 0;

                const char *s = (*arg);
                while (isdigit(*s))
                {
                    s++;
                }
                if (*s == ',')
                {
                    min_depth = atoi((*arg));
                    s++;
                }
                else
                {
                    s = (*arg);
                }

                /* Note: passing (*arg) instead of s to the
                 * set_depth_tests_order is likely wrong, but when doing
                 * it right, the tests fail.
                 * */
                if (freecell_solver_user_set_depth_tests_order(instance,
                        min_depth,
                        ((opt == FCS_OPT_DEPTH_TESTS_ORDER_2) ? s : (*arg))
                            FCS__PASS_ERR_STR(&fcs_user_errstr)))
                {
                    ASSIGN_ERR_STR_AND_FREE(fcs_user_errstr, error_string,
                        "Error in depth tests' order!\n%s\n", fcs_user_errstr);
                    RET_ERROR_IN_ARG();
                }
            }
        }
        break;

        case FCS_OPT_SET_PRUNING: /* STRINGS=-sp|--set-pruning; */
        {
            PROCESS_OPT_ARG();
            if (freecell_solver_user_set_pruning(
                    instance, (*arg)FCS__PASS_ERR_STR(&fcs_user_errstr)) != 0)
            {
                ASSIGN_ERR_STR_AND_FREE(fcs_user_errstr, error_string,
                    "Error in the optimization scan's pruning!\n%s\n",
                    fcs_user_errstr);
                RET_ERROR_IN_ARG();
            }
        }
        break;

        case FCS_OPT_CACHE_LIMIT: /* STRINGS=--cache-limit; */
        {
            PROCESS_OPT_ARG();

            freecell_solver_user_set_cache_limit(instance, atol(*arg));
        }
        break;

        case FCS_OPT_FLARES_CHOICE: /* STRINGS=--flares-choice; */
        {
            PROCESS_OPT_ARG();

#ifdef FCS_WITH_FLARES
#ifndef FCS_WITHOUT_FC_PRO_MOVES_COUNT
            if (freecell_solver_user_set_flares_choice(instance, (*arg)) != 0)
            {
                ASSIGN_ERR_STR(error_string,
                    "Unknown flares choice argument '%s'.\n", (*arg));
                RET_ERROR_IN_ARG();
            }
#endif
#endif
        }
        break;

        case FCS_OPT_PATSOLVE_X_PARAM: /* STRINGS=--patsolve-x-param; */
        {
            PROCESS_OPT_ARG();

#ifndef FCS_DISABLE_PATSOLVE
            {
                int position, x_param_val;
                if (sscanf((*arg), "%d,%d", &position, &x_param_val) != 2)
                {
                    ASSIGN_ERR_STR(error_string, "%s",
                        "Wrong format for --patsolve-x-param");
                    RET_ERROR_IN_ARG();
                }
                if (freecell_solver_user_set_patsolve_x_param(instance,
                        position,
                        x_param_val FCS__PASS_ERR_STR(&fcs_user_errstr)) != 0)
                {
                    ASSIGN_ERR_STR_AND_FREE(fcs_user_errstr, error_string,
                        "Error in patsolve X param setting!\n%s\n",
                        fcs_user_errstr);
                    RET_ERROR_IN_ARG();
                }
            }
#endif
        }
        break;

        case FCS_OPT_PATSOLVE_Y_PARAM: /* STRINGS=--patsolve-y-param; */
        {
            PROCESS_OPT_ARG();

#ifndef FCS_DISABLE_PATSOLVE
            {
                int position;
                double y_param_val;
                if (sscanf((*arg), "%d,%lf", &position, &y_param_val) != 2)
                {
                    ASSIGN_ERR_STR(error_string, "%s",
                        "Wrong format for --patsolve-y-param");
                    RET_ERROR_IN_ARG();
                }
                if (freecell_solver_user_set_patsolve_y_param(instance,
                        position,
                        y_param_val FCS__PASS_ERR_STR(&fcs_user_errstr)) != 0)
                {
                    ASSIGN_ERR_STR_AND_FREE(fcs_user_errstr, error_string,
                        "Error in patsolve Y param setting!\n%s\n",
                        fcs_user_errstr);
                    RET_ERROR_IN_ARG();
                }
            }
#endif
        }
        break;
        }

    /* OPT-PARSE-END */
    end_of_arg_loop:;
    }

    *last_arg = arg - &(argv[0]);
    return FCS_CMD_LINE_OK;
}

#ifndef FC_SOLVE__REMOVE_OLD_API_1
DLLEXPORT int freecell_solver_user_cmd_line_parse_args(void *instance, int argc,
    freecell_solver_str_t argv[], const int start_arg,
    freecell_solver_str_t *known_parameters,
    freecell_solver_user_cmd_line_known_commands_callback_t callback,
    void *const callback_context FCS__PASS_ERR_STR(char **error_string),
    int *last_arg)
{
    return freecell_solver_user_cmd_line_parse_args_with_file_nesting_count(
        instance, argc, argv, start_arg, known_parameters, callback,
        callback_context FCS__PASS_ERR_STR(error_string), last_arg, -1, NULL);
}
#endif
