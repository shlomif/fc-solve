/* Copyright (c) 2000 Shlomi Fish
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use,
 * copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following
 * conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 */
/*
 * cmd_line.c - the Freecell Solver command line arguments-like parsing 
 * routines. Useful for more easily configuring a Freecell Solver instance.
 */
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>

#include "fcs_user.h"
#include "fcs_cl.h"
#include "split_cmd_line.h"

#include "prefix.h"

#ifdef DMALLOC
#include "dmalloc.h"
#endif

#include "cmd_line_enum.h"

static int read_preset(char * preset_name, args_man_t * * args, char * * opened_files_dir_to_assign, char * user_preset_dir)
{
    int ret_code = 1;
    char * home_dir_presetrc = NULL, * global_presetrc = NULL, * env_var_presetrc = NULL;
    char * path;
    char * * presetrc_pathes[5];
    int path_idx;
    char line[8192];
    FILE * f = NULL;
    char * fgets_ret;
    char * opened_files_dir = NULL;
    int read_next_preset = 0;
   

    {
        register int idx = 0;
        presetrc_pathes[idx++] = &env_var_presetrc;
        presetrc_pathes[idx++] = &home_dir_presetrc;
        presetrc_pathes[idx++] = &global_presetrc;
        presetrc_pathes[idx++] = &user_preset_dir;
        presetrc_pathes[idx++] = NULL;
    }

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
                    *args = fc_solve_args_man_alloc();
                    fc_solve_args_man_chop(*args, line+8);
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
    char * argv[],
    int start_arg,
    char * * known_parameters,
    freecell_solver_user_cmd_line_known_commands_callback_t callback,
    void * callback_context,
    char * * error_string,
    int * last_arg,
    int file_nesting_count,
    char * opened_files_dir
    )
{
    char * * arg, * * arg_argc;
    char * * known_param;
    int num_to_skip;
    int callback_ret;
    int opt;
    const char * p;

    *error_string = NULL;

    arg_argc = &(argv[argc]);

    for(arg=&(argv[start_arg]);arg<arg_argc;arg++)
    {
        /* First check for the parameters that the user recognizes */
        known_param = known_parameters;
        while((*known_param) && strcmp(*known_param, (*arg)))
        {
            known_param++;
        }
        if ((*known_param) != NULL )
        {
            int ret;

            callback_ret = callback(instance, argc, argv, arg-&(argv[0]), &num_to_skip, &ret, callback_context);
            if (callback_ret == FCS_CMD_LINE_SKIP)
            {
                arg += num_to_skip-1;
                continue;
            }
            else if (callback_ret == FCS_CMD_LINE_STOP)
            {
                *last_arg = arg-&(argv[0]);
                return ret;
            }
        }

        /* OPT-PARSE-START */
p = (*arg);
opt = FCS_OPT_UNRECOGNIZED;

{
if (*(p++) == '-')
{
{ switch(*(p++)) {
case '-':
{ switch(*(p++)) {
case 'a':
{
if (!strncmp(p, "-star-weights", 13)) {
p += 13;
opt = FCS_OPT_A_STAR_WEIGHTS;

}
}

break;

case 'c':
{
if (!strncmp(p, "alc-real-depth", 14)) {
p += 14;
opt = FCS_OPT_CALC_REAL_DEPTH;

}
}

break;

case 'd':
{
if (!strncmp(p, "ecks-num", 8)) {
p += 8;
opt = FCS_OPT_DECKS_NUM;

}
}

break;

case 'e':
{
if (!strncmp(p, "mpty-stacks-filled-by", 21)) {
p += 21;
opt = FCS_OPT_EMPTY_STACKS_FILLED_BY;

}
}

break;

case 'f':
{
if (!strncmp(p, "reecells-num", 12)) {
p += 12;
opt = FCS_OPT_FREECELLS_NUM;

}
}

break;

case 'g':
{
if (!strncmp(p, "ame", 3)) {
p += 3;
opt = FCS_OPT_GAME;

}
}

break;

case 'l':
{
if (!strncmp(p, "oad-config", 10)) {
p += 10;
opt = FCS_OPT_LOAD_CONFIG;

}
}

break;

case 'm':
{ switch(*(p++)) {
case 'a':
{
if (!strncmp(p, "x-", 2)) {
p += 2;
{ switch(*(p++)) {
case 'd':
{
if (!strncmp(p, "epth", 4)) {
p += 4;
opt = FCS_OPT_MAX_DEPTH;

}
}

break;

case 'i':
{
if (!strncmp(p, "ters", 4)) {
p += 4;
opt = FCS_OPT_MAX_ITERS;

}
}

break;

case 's':
{
if (!strncmp(p, "tored-states", 12)) {
p += 12;
opt = FCS_OPT_MAX_STORED_STATES;

}
}

break;

}
}

}
}

break;

case 'e':
{
if (!strncmp(p, "thod", 4)) {
p += 4;
opt = FCS_OPT_METHOD;

}
}

break;

}
}

break;

case 'n':
{
if (!strncmp(p, "ext-", 4)) {
p += 4;
{ switch(*(p++)) {
case 'h':
{
if (!strncmp(p, "ard-thread", 10)) {
p += 10;
opt = FCS_OPT_NEXT_HARD_THREAD;

}
}

break;

case 's':
{
if (!strncmp(p, "oft-thread", 10)) {
p += 10;
opt = FCS_OPT_NEXT_SOFT_THREAD;

}
}

break;

}
}

}
}

break;

case 'o':
{
if (!strncmp(p, "ptimiz", 6)) {
p += 6;
{ switch(*(p++)) {
case 'a':
{
if (!strncmp(p, "tion-tests-order", 16)) {
p += 16;
opt = FCS_OPT_OPTIMIZATION_TESTS_ORDER;

}
}

break;

case 'e':
{
if (!strncmp(p, "-solution", 9)) {
p += 9;
opt = FCS_OPT_OPTIMIZE_SOLUTION;

}
}

break;

}
}

}
}

break;

case 'p':
{
if (!strncmp(p, "re", 2)) {
p += 2;
{ switch(*(p++)) {
case 'l':
{
if (!strncmp(p, "ude", 3)) {
p += 3;
opt = FCS_OPT_PRELUDE;

}
}

break;

case 's':
{
if (!strncmp(p, "et", 2)) {
p += 2;
opt = FCS_OPT_GAME;

}
}

break;

}
}

}
}

break;

case 'r':

{
if (*(p++) == 'e')
{
{ switch(*(p++)) {
case 'a':
{
if (!strncmp(p, "d-from-file", 11)) {
p += 11;
opt = FCS_OPT_READ_FROM_FILE;

}
}

break;

case 'p':
{
if (!strncmp(p, "arent-states", 12)) {
p += 12;
opt = FCS_OPT_REPARENT_STATES;

}
}

break;

case 's':
{
if (!strncmp(p, "et", 2)) {
p += 2;
opt = FCS_OPT_RESET;

}
}

break;

}
}

}

}

break;

case 's':
{ switch(*(p++)) {
case 'c':
{
if (!strncmp(p, "ans-synergy", 11)) {
p += 11;
opt = FCS_OPT_SCANS_SYNERGY;

}
}

break;

case 'e':
{
if (!strncmp(p, "quence", 6)) {
p += 6;
{ switch(*(p++)) {
case '-':
{
if (!strncmp(p, "move", 4)) {
p += 4;
opt = FCS_OPT_SEQUENCE_MOVE;

}
}

break;

case 's':
{
if (!strncmp(p, "-are-built-by", 13)) {
p += 13;
opt = FCS_OPT_SEQUENCES_ARE_BUILT_BY;

}
}

break;

}
}

}
}

break;

case 'o':
{
if (!strncmp(p, "ft-thread-step", 14)) {
p += 14;
opt = FCS_OPT_SOFT_THREAD_STEP;

}
}

break;

case 't':
{ switch(*(p++)) {
case '-':
{
if (!strncmp(p, "name", 4)) {
p += 4;
opt = FCS_OPT_ST_NAME;

}
}

break;

case 'a':
{
if (!strncmp(p, "cks-num", 7)) {
p += 7;
opt = FCS_OPT_STACKS_NUM;

}
}

break;

}
}

break;

}
}

break;

case 't':
{
if (!strncmp(p, "ests-order", 10)) {
p += 10;
opt = FCS_OPT_TESTS_ORDER;

}
}

break;

}
}

break;

case 'a':
{
if (!strncmp(p, "sw", 2)) {
p += 2;
opt = FCS_OPT_A_STAR_WEIGHTS;

}
}

break;

case 'g':

{
if (*p == '\0')
{

opt = FCS_OPT_GAME;
}
}

break;

case 'l':

{
if (*p == '\0')
{

opt = FCS_OPT_LOAD_CONFIG;
}
}

break;

case 'm':
{ switch(*(p++)) {
case 'd':

{
if (*p == '\0')
{

opt = FCS_OPT_MAX_DEPTH;
}
}

break;

case 'e':

{
if (*p == '\0')
{

opt = FCS_OPT_METHOD;
}
}

break;

case 'i':

{
if (*p == '\0')
{

opt = FCS_OPT_MAX_ITERS;
}
}

break;

case 's':
{
if (!strncmp(p, "s", 1)) {
p += 1;
opt = FCS_OPT_MAX_STORED_STATES;

}
}

break;

}
}

break;

case 'n':
{ switch(*(p++)) {
case 'h':
{
if (!strncmp(p, "t", 1)) {
p += 1;
opt = FCS_OPT_NEXT_HARD_THREAD;

}
}

break;

case 's':
{
if (!strncmp(p, "t", 1)) {
p += 1;
opt = FCS_OPT_NEXT_SOFT_THREAD;

}
}

break;

}
}

break;

case 'o':
{
if (!strncmp(p, "pt", 2)) {
p += 2;
{ switch(*(p++)) {
case '\0':
{
opt = FCS_OPT_OPTIMIZE_SOLUTION;
}

break;

case '-':
{
if (!strncmp(p, "to", 2)) {
p += 2;
opt = FCS_OPT_OPTIMIZATION_TESTS_ORDER;

}
}

break;

}
}

}
}

break;

case 's':
{ switch(*(p++)) {
case 'e':
{
if (!strncmp(p, "ed", 2)) {
p += 2;
opt = FCS_OPT_SEED;

}
}

break;

case 't':
{
if (!strncmp(p, "ep", 2)) {
p += 2;
opt = FCS_OPT_SOFT_THREAD_STEP;

}
}

break;

}
}

break;

case 't':
{
if (!strncmp(p, "o", 1)) {
p += 1;
opt = FCS_OPT_TESTS_ORDER;

}
}

break;

}
}

}

}
        switch (opt)
        {
        case FCS_OPT_UNRECOGNIZED:
        {
            *last_arg = arg-&(argv[0]);
            return FCS_CMD_LINE_UNRECOGNIZED_OPTION;
        }
        break;

#define PROCESS_OPT_ARG() \
        { \
            arg++; \
            if (arg == arg_argc) \
            { \
                *last_arg = arg-&(argv[0]); \
                return FCS_CMD_LINE_PARAM_WITH_NO_ARG; \
            } \
        }

#define RET_ERROR_IN_ARG() \
        *last_arg = arg-&(argv[0]); \
        return FCS_CMD_LINE_ERROR_IN_ARG;

        case FCS_OPT_MAX_DEPTH: /* STRINGS=-md|--max-depth; */
        {
            PROCESS_OPT_ARG() ;
            freecell_solver_user_limit_depth(instance, atoi((*arg)));
        }
        break;

        case FCS_OPT_MAX_ITERS: /* STRINGS=-mi|--max-iters; */
        {
            PROCESS_OPT_ARG() ;

            freecell_solver_user_limit_current_instance_iterations(instance, atoi((*arg)));
        }
        break;

        case FCS_OPT_TESTS_ORDER: /* STRINGS=-to|--tests-order; */
        {
            int ret;
            char * fcs_user_errstr;

            PROCESS_OPT_ARG() ;

            ret = freecell_solver_user_set_tests_order(instance, (*arg), &fcs_user_errstr);
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

                RET_ERROR_IN_ARG() ;
            }
        }
        break;

        case FCS_OPT_FREECELLS_NUM: /* STRINGS=--freecells-num; */
        {
            PROCESS_OPT_ARG() ;
            if (freecell_solver_user_set_num_freecells(instance, atoi((*arg))) != 0)
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

                RET_ERROR_IN_ARG() ;
            }
        }
        break;

        case FCS_OPT_STACKS_NUM: /* STRINGS=--stacks-num; */
        {
            PROCESS_OPT_ARG() ;
            if (freecell_solver_user_set_num_stacks(instance, atoi((*arg))) != 0)
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

                RET_ERROR_IN_ARG() ;
            }
        }
        break;

        case FCS_OPT_DECKS_NUM: /* STRINGS=--decks-num; */
        {
            PROCESS_OPT_ARG() ;
            if (freecell_solver_user_set_num_decks(instance, atoi((*arg))) != 0)
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

                RET_ERROR_IN_ARG() ;
            }
        }
        break;

        case FCS_OPT_SEQUENCES_ARE_BUILT_BY: /* STRINGS=--sequences-are-built-by; */
        {
            int sbb;

            PROCESS_OPT_ARG() ;

            if (!strcmp((*arg), "suit"))
            {
                sbb = FCS_SEQ_BUILT_BY_SUIT;
            }
            else if (!strcmp((*arg), "rank"))
            {
                sbb = FCS_SEQ_BUILT_BY_RANK;
            }
            else
            {
                sbb = FCS_SEQ_BUILT_BY_ALTERNATE_COLOR;
            }
            freecell_solver_user_set_sequences_are_built_by_type(instance, sbb);
        }
        break;

        case FCS_OPT_SEQUENCE_MOVE: /* STRINGS=--sequence-move; */
        {
            int unlimited;

            PROCESS_OPT_ARG() ;

            if (!strcmp((*arg), "unlimited"))
            {
                unlimited = 1;
            }
            else
            {
                unlimited = 0;
            }
            freecell_solver_user_set_sequence_move(instance, unlimited);
        }
        break;

        case FCS_OPT_EMPTY_STACKS_FILLED_BY: /* STRINGS=--empty-stacks-filled-by; */
        {
            int es_fill;

            PROCESS_OPT_ARG() ;

            if (!strcmp((*arg), "kings"))
            {
                es_fill = FCS_ES_FILLED_BY_KINGS_ONLY;
            }
            else if (!strcmp((*arg), "none"))
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
        break;

        case FCS_OPT_GAME: /* STRINGS=--game|--preset|-g; */
        {
            int ret;

            PROCESS_OPT_ARG() ;

            ret = freecell_solver_user_apply_preset(instance, (*arg));
            if (ret == FCS_PRESET_CODE_NOT_FOUND)
            {
                char * errstr;

                errstr = malloc(strlen((*arg))+500);

                sprintf(errstr, "Unknown game \"%s\"!\n\n", (*arg));
                *error_string = errstr;

                RET_ERROR_IN_ARG() ;
            }
            else if (ret == FCS_PRESET_CODE_FREECELLS_EXCEED_MAX)
            {
                char * errstr;

                errstr = malloc(strlen((*arg))+500);
                sprintf(errstr, "The game \"%s\" exceeds the maximal number "
                        "of freecells in the program.\n"
                        "Modify the file \"config.h\" and recompile, "
                        "if you wish to solve one of its boards.\n",
                        (*arg)
                        );

                *error_string = errstr;

                RET_ERROR_IN_ARG() ;
            }
            else if (ret == FCS_PRESET_CODE_STACKS_EXCEED_MAX)
            {
                char * errstr;

                errstr = malloc(strlen((*arg))+500);

                sprintf(errstr, "The game \"%s\" exceeds the maximal number "
                        "of stacks in the program.\n"
                        "Modify the file \"config.h\" and recompile, "
                        "if you wish to solve one of its boards.\n",
                        (*arg)
                        );

                *error_string = errstr;

                RET_ERROR_IN_ARG() ;
            }
            else if (ret != FCS_PRESET_CODE_OK)
            {
                char * errstr;

                errstr = malloc(strlen((*arg))+500);

                sprintf(errstr,
                    "The game \"%s\" exceeds the limits of the program.\n"
                    "Modify the file \"config.h\" and recompile, if you wish to solve one of its boards.\n",
                    (*arg)
                );

                *error_string = errstr;

                RET_ERROR_IN_ARG() ;
            }
        }
        break;

        case FCS_OPT_METHOD: /* STRINGS=-me|--method; */
        {
            int method;

            PROCESS_OPT_ARG() ;

            if (!strcmp((*arg), "soft-dfs") ||
                !strcmp((*arg), "dfs"))
            {
                method = FCS_METHOD_SOFT_DFS;
            }
            else if (!strcmp((*arg), "bfs"))
            {
                method = FCS_METHOD_BFS;
            }
            else if (!strcmp((*arg), "a-star"))
            {
                method = FCS_METHOD_A_STAR;
            }
            else if (!strcmp((*arg), "random-dfs"))
            {
                method = FCS_METHOD_RANDOM_DFS;
            }
            else
            {
                char * errstr;

                errstr = malloc(strlen((*arg))+500);

                sprintf(
                    errstr,
                    "Unknown solving method \"%s\".\n",
                    (*arg)
                    );

                *error_string = errstr;

                RET_ERROR_IN_ARG() ;
            }

            freecell_solver_user_set_solving_method(instance, method);
        }
        break;

        case FCS_OPT_A_STAR_WEIGHTS: /* STRINGS=-asw|--a-star-weights; */
        {
            PROCESS_OPT_ARG() ;

            {
                int a;
                char * start_num;
                char * end_num;
                char save;
                start_num = (*arg);
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
        break;

        case FCS_OPT_OPTIMIZE_SOLUTION: /* STRINGS=-opt|--optimize-solution; */
        {
            freecell_solver_user_set_solution_optimization(instance, 1);
        }
        break;

        case FCS_OPT_SEED: /* STRINGS=-seed; */
        {
            PROCESS_OPT_ARG() ;

            freecell_solver_user_set_random_seed(instance, atoi((*arg)));
        }
        break;

        case FCS_OPT_MAX_STORED_STATES: /* STRINGS=-mss|--max-stored-states; */
        {
            PROCESS_OPT_ARG() ;

            freecell_solver_user_limit_num_states_in_collection(
                instance,
                atoi((*arg))
            );
        }
        break;

        case FCS_OPT_NEXT_SOFT_THREAD: /* STRINGS=-nst|--next-soft-thread; */
        case FCS_OPT_NEXT_HARD_THREAD: /* STRINGS=-nht|--next-hard-thread; */
        {
            int ret;

            ret =
                (opt == FCS_OPT_NEXT_SOFT_THREAD)
                    ? freecell_solver_user_next_soft_thread(instance)
                    : freecell_solver_user_next_hard_thread(instance)
                    ;

            if (ret)
            {
                char * errstr;

                errstr = strdup("The maximal number of soft threads has been exceeded\n");

                *error_string = errstr;

                RET_ERROR_IN_ARG() ;
            }
        }
        break;

        case FCS_OPT_SOFT_THREAD_STEP: /* STRINGS=-step|--soft-thread-step; */
        {
            PROCESS_OPT_ARG() ;

            freecell_solver_user_set_soft_thread_step(
                instance,
                atoi((*arg))
            );
        }
        break;

        case FCS_OPT_REPARENT_STATES: /* STRINGS=--reparent-states; */
        {
            freecell_solver_user_set_reparent_states(
                instance,
                1
                );
        }
        break;

        case FCS_OPT_CALC_REAL_DEPTH: /* STRINGS=--calc-real-depth; */
        {
            freecell_solver_user_set_calc_real_depth(
                instance,
                1);
        }
        break;

        case FCS_OPT_ST_NAME: /* STRINGS=--st-name; */
        {
            PROCESS_OPT_ARG() ;

            freecell_solver_user_set_soft_thread_name(instance, (*arg));
        }
        break;

        case FCS_OPT_PRELUDE: /* STRINGS=--prelude; */
        {
            PROCESS_OPT_ARG() ;

            freecell_solver_user_set_hard_thread_prelude(instance, (*arg));
        }
        break;

        case FCS_OPT_OPTIMIZATION_TESTS_ORDER: /* STRINGS=-opt-to|--optimization-tests-order; */
        {
            int ret;
            char * fcs_user_errstr;

            PROCESS_OPT_ARG() ;

            ret = freecell_solver_user_set_optimization_scan_tests_order(
                    instance,
                    (*arg),
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

                RET_ERROR_IN_ARG() ;
            }
        }
        break;

        case FCS_OPT_SCANS_SYNERGY: /* STRINGS=--scans-synergy; */
        {
            int value;

            PROCESS_OPT_ARG() ;

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
                char * errstr;

                errstr = malloc(strlen((*arg))+500);

                sprintf(errstr, "Unknown scans' synergy type \"%s\"!\n", (*arg));
                *error_string = errstr;

                RET_ERROR_IN_ARG() ;
            }

            freecell_solver_user_set_scans_synergy(
                instance,
                value
                );
        }
        break;

        case FCS_OPT_RESET: /* STRINGS=--reset; */
        {
            freecell_solver_user_reset(instance);
        }
        break;

        case FCS_OPT_READ_FROM_FILE: /* STRINGS=--read-from-file; */
        {
            PROCESS_OPT_ARG() ;

            if (file_nesting_count == 0)
            {
                /* do nothing */
            }
            else
            {
                char * s, * buffer;
                FILE * f;
                long file_len;
                int ret;
                size_t num_read;
                args_man_t * args_man;
                int num_file_args_to_skip;

                num_file_args_to_skip = 0;

                s = (*arg);
                while(isdigit(*s))
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
                    char * complete_path;
                    register int len;

                    len = strlen(opened_files_dir);
                    len += strlen(s);
                    len++;

                    complete_path = malloc(len);
                    sprintf(complete_path, "%s%s", opened_files_dir, s);
                    f = fopen(complete_path, "rt");
                    free(complete_path);
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
                    char * err_str;

                    err_str = malloc(strlen(s)+100);
                    sprintf(err_str,
                            "Could not open file \"%s\"!\nQuitting.\n",
                            s);

                    *error_string = err_str;

                    RET_ERROR_IN_ARG() ;
                }
                fseek(f, 0, SEEK_END);
                file_len = ftell(f);
                buffer=malloc(file_len+1);
                if (buffer == NULL)
                {
                    *error_string = strdup("Could not allocate enough memory to parse the file. Quitting.\n");
                    fclose(f);

                    RET_ERROR_IN_ARG() ;
                }
                fseek(f,0,SEEK_SET);
                num_read = fread(buffer, 1, file_len, f);
                fclose(f);
                buffer[num_read] = '\0';

                args_man = fc_solve_args_man_alloc();
                ret = fc_solve_args_man_chop(args_man, buffer);
                free(buffer);
                if (ret != 0)
                {
                    *error_string =
                        strdup("Could not parse the file. Quitting\n");
                    fc_solve_args_man_free(args_man);

                    RET_ERROR_IN_ARG() ;
                }

                if (num_file_args_to_skip >= args_man->argc)
                {
                    /* Do nothing */
                }
                else
                {
                    ret = freecell_solver_user_cmd_line_parse_args_with_file_nesting_count(
                        instance,
                        args_man->argc - num_file_args_to_skip,
                        args_man->argv + num_file_args_to_skip,
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
                        fc_solve_args_man_free(args_man);
                        return ret;
                    }
                }
                fc_solve_args_man_free(args_man);
            }
        }
        break;

        case FCS_OPT_LOAD_CONFIG: /* STRINGS=-l|--load-config; */
        {
            PROCESS_OPT_ARG() ;

            {
                int status;
                args_man_t * preset_args;
                char * dir = NULL;

                status = read_preset((*arg), &preset_args, &dir, NULL);
                if (status != 0)
                {
                    char * err_str;
                    err_str = malloc(strlen((*arg)) + 100);
                    sprintf(err_str, "Unable to load the \"%s\" configuration!\n", (*arg));
                    *error_string = err_str;

                    RET_ERROR_IN_ARG() ;
                }
                else
                {
                    int ret;

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
                    fc_solve_args_man_free(preset_args);

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
        break;


        }
        /* OPT-PARSE-END */
    }

    *last_arg = arg-&(argv[0]);
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

