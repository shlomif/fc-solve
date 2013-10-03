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

#define BUILDING_DLL 1

#include <string.h>
#include <stdlib.h>
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
#include "inline.h"
#include "bool.h"

#include "cmd_line_enum.h"

static GCC_INLINE int read_preset(const char * preset_name, args_man_t * * args_man, char * * opened_files_dir_to_assign, const char * user_preset_dir)
{
    int ret_code = 1;
    char * home_dir_presetrc = NULL, * env_var_presetrc = NULL;
    const char * global_presetrc = NULL;
    const char * * presetrc_pathes[5];
    FILE * f = NULL;
    char * opened_files_dir = NULL;


    {
        register int idx = 0;
        presetrc_pathes[idx++] = (const char * *)(void *)&env_var_presetrc;
        presetrc_pathes[idx++] = (const char * *)(void *)&home_dir_presetrc;
        presetrc_pathes[idx++] = (const char * *)&global_presetrc;
        presetrc_pathes[idx++] = &user_preset_dir;
        presetrc_pathes[idx++] = NULL;
    }

    {
        char * home_dir;
        home_dir = getenv("HOME");
        if (home_dir)
        {
            home_dir_presetrc = SMALLOC(
                home_dir_presetrc,
                strlen(home_dir) + 50
            );
            sprintf(home_dir_presetrc,
                "%s/.freecell-solver/presetrc", home_dir
                );
        }
    }
    env_var_presetrc = getenv("FREECELL_SOLVER_PRESETRC");

#ifdef _WIN32
    char windows_exe_dir[MAX_PATH] = "";
    char windows_global_presetc[MAX_PATH + 100] = "";

    /* Will contain exe path */
    const HMODULE hModule = GetModuleHandle(NULL);
    if (hModule != NULL)
    {

        /* When passing NULL to GetModuleHandle, it returns handle of exe itself */
        GetModuleFileName(hModule, windows_exe_dir, (sizeof(windows_exe_dir)));
        {
            /* Remove the basename and keep only the dirname. */
            char * s = windows_exe_dir + strlen(windows_exe_dir) - 1;
            while ((s > windows_exe_dir) && (!(*s == '/') || (*s == '\\')))
            {
                s--;
            }
            if (s > windows_exe_dir)
            {
                *s = '\0';
            }
         }
         sprintf(
             windows_global_presetc,
             "%s/../share/freecell-solver/presetrc",
             windows_exe_dir
         );
         global_presetrc = windows_global_presetc;
    }
    else
    {
        global_presetrc = NULL;
    }

#else
    global_presetrc = (FREECELL_SOLVER_PKG_DATA_DIR "/presetrc");
#endif
    const char * path;
    fcs_bool_t read_next_preset = FALSE;
    for (int path_idx=0;(presetrc_pathes[path_idx] != NULL) ; path_idx++)
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
            char line[8192];
            if (fgets(line, sizeof(line), f) == NULL)
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
#ifdef _WIN32
                {
                    char * s = line+4;
                    const char * const var_prefix = "${EXE_DIRNAME}";
                    const int var_prefix_len = strlen(var_prefix);
                    if (! strncmp(s, var_prefix, var_prefix_len))
                    {
                        opened_files_dir = malloc(strlen(s) + strlen(windows_exe_dir) + 100);
                        sprintf(
                            opened_files_dir, "%s%s",
                            windows_exe_dir, s+var_prefix_len
                        );
                    }
                    else
                    {
                        opened_files_dir = strdup(s);
                    }
                }
#else
                opened_files_dir = strdup(line+4);
#endif
            }
            else if (!strncmp(line, "name=", 5))
            {
                nullify_newline();
                if (!strcmp(line+5, preset_name))
                {
                    read_next_preset = TRUE;
                }
            }
            else if (!strncmp(line, "command=", 8))
            {
                if (read_next_preset)
                {
                    *args_man = fc_solve_args_man_alloc();
                    fc_solve_args_man_chop(*args_man, line+8);
#if 0
                    fprintf(stderr, "man_chop for <<<%s>>>\n", line);
                    fflush(stderr);
#endif
                    ret_code = 0;
                    goto have_preset;
                }
            }
        }
        fclose(f);
        f = NULL;
#undef nullify_newline
    }
have_preset:

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

DLLEXPORT int freecell_solver_user_cmd_line_read_cmd_line_preset(
    void * instance,
    const char * const preset_name,
    freecell_solver_str_t * known_parameters,
    char * * error_string,
    int file_nesting_count,
    freecell_solver_str_t opened_files_dir
)
{
    int status;
    args_man_t * preset_args;
    char * dir = NULL;

    status = read_preset(preset_name, &preset_args, &dir, NULL);
    if (status != 0)
    {
        *error_string = strdup("Could not read preset.");
        return FCS_CMD_LINE_ERROR_IN_ARG;
    }
    else
    {
        int ret;
        int last_arg = 0;

        ret = freecell_solver_user_cmd_line_parse_args_with_file_nesting_count(
            instance,
            preset_args->argc,
            (freecell_solver_str_t *)(void *)(preset_args->argv),
            0,
            known_parameters,
            NULL,
            NULL,
            error_string,
            &(last_arg),
            ((file_nesting_count < 0) ? file_nesting_count : (file_nesting_count-1)),
            dir ? dir : opened_files_dir
        );

        if (dir)
        {
            free(dir);
        }
        fc_solve_args_man_free(preset_args);

        return ret;

    }
}

DLLEXPORT int freecell_solver_user_cmd_line_parse_args_with_file_nesting_count(
    void * instance,
    int argc,
    freecell_solver_str_t argv[],
    int start_arg,
    freecell_solver_str_t * known_parameters,
    freecell_solver_user_cmd_line_known_commands_callback_t callback,
    void * callback_context,
    char * * error_string,
    int * last_arg,
    int file_nesting_count,
    freecell_solver_str_t opened_files_dir
    )
{
    int num_to_skip;
    const char * p;

    *error_string = NULL;

    freecell_solver_str_t * arg_argc = &(argv[argc]);

    freecell_solver_str_t * arg;
    freecell_solver_str_t * known_param;
    for (arg = &(argv[start_arg]) ; arg < arg_argc ; arg++)
    {
        /* First check for the parameters that the user recognizes */
        if (known_parameters)
        {
            known_param = known_parameters;
            while((*known_param) && strcmp(*known_param, (*arg)))
            {
                known_param++;
            }
            if ((*known_param) != NULL )
            {
                int ret;

                const int callback_ret = callback(instance, argc, argv, arg-&(argv[0]), &num_to_skip, &ret, callback_context);
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
        }

        int opt;
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
if (!strcmp(p, "-star-weights")) {
opt = FCS_OPT_BEFS_WEIGHTS;

}
}

break;

case 'c':

{
if (*(p++) == 'a')
{
{ switch(*(p++)) {
case 'c':
{
if (!strcmp(p, "he-limit")) {
opt = FCS_OPT_CACHE_LIMIT;

}
}

break;

case 'l':
{
if (!strcmp(p, "c-real-depth")) {
opt = FCS_OPT_CALC_REAL_DEPTH;

}
}

break;

}
}

}

}

break;

case 'd':

{
if (*(p++) == 'e')
{
{ switch(*(p++)) {
case 'c':
{
if (!strcmp(p, "ks-num")) {
opt = FCS_OPT_DECKS_NUM;

}
}

break;

case 'p':
{
if (!strcmp(p, "th-tests-order")) {
opt = FCS_OPT_DEPTH_TESTS_ORDER;

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
if (!strcmp(p, "mpty-stacks-filled-by")) {
opt = FCS_OPT_EMPTY_STACKS_FILLED_BY;

}
}

break;

case 'f':
{ switch(*(p++)) {
case 'l':
{
if (!strncmp(p, "are", 3)) {
p += 3;
{ switch(*(p++)) {
case '-':
{
if (!strcmp(p, "name")) {
opt = FCS_OPT_FLARE_NAME;

}
}

break;

case 's':

{
if (*(p++) == '-')
{
{ switch(*(p++)) {
case 'c':
{
if (!strcmp(p, "hoice")) {
opt = FCS_OPT_FLARES_CHOICE;

}
}

break;

case 'i':
{
if (!strcmp(p, "ters-factor")) {
opt = FCS_OPT_FLARES_ITERS_FACTOR;

}
}

break;

case 'p':
{
if (!strcmp(p, "lan")) {
opt = FCS_OPT_FLARES_PLAN;

}
}

break;

}
}

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
if (!strcmp(p, "eecells-num")) {
opt = FCS_OPT_FREECELLS_NUM;

}
}

break;

}
}

break;

case 'g':
{
if (!strcmp(p, "ame")) {
opt = FCS_OPT_GAME;

}
}

break;

case 'l':
{
if (!strcmp(p, "oad-config")) {
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
if (!strcmp(p, "epth")) {
opt = FCS_OPT_MAX_DEPTH;

}
}

break;

case 'i':
{
if (!strcmp(p, "ters")) {
opt = FCS_OPT_MAX_ITERS;

}
}

break;

case 's':
{
if (!strcmp(p, "tored-states")) {
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
if (!strcmp(p, "thod")) {
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
case 'f':
{
if (!strcmp(p, "lare")) {
opt = FCS_OPT_NEXT_FLARE;

}
}

break;

case 'h':
{
if (!strcmp(p, "ard-thread")) {
opt = FCS_OPT_NEXT_HARD_THREAD;

}
}

break;

case 'i':
{
if (!strcmp(p, "nstance")) {
opt = FCS_OPT_NEXT_INSTANCE;

}
}

break;

case 's':
{
if (!strcmp(p, "oft-thread")) {
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
if (!strcmp(p, "tion-tests-order")) {
opt = FCS_OPT_OPTIMIZATION_TESTS_ORDER;

}
}

break;

case 'e':
{
if (!strcmp(p, "-solution")) {
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
if (!strcmp(p, "ude")) {
opt = FCS_OPT_PRELUDE;

}
}

break;

case 's':
{
if (!strcmp(p, "et")) {
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
if (!strcmp(p, "d-from-file")) {
opt = FCS_OPT_READ_FROM_FILE;

}
}

break;

case 'p':
{
if (!strcmp(p, "arent-states")) {
opt = FCS_OPT_REPARENT_STATES;

}
}

break;

case 's':
{
if (!strcmp(p, "et")) {
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
if (!strcmp(p, "ans-synergy")) {
opt = FCS_OPT_SCANS_SYNERGY;

}
}

break;

case 'e':
{ switch(*(p++)) {
case 'q':
{
if (!strncmp(p, "uence", 5)) {
p += 5;
{ switch(*(p++)) {
case '-':
{
if (!strcmp(p, "move")) {
opt = FCS_OPT_SEQUENCE_MOVE;

}
}

break;

case 's':
{
if (!strcmp(p, "-are-built-by")) {
opt = FCS_OPT_SEQUENCES_ARE_BUILT_BY;

}
}

break;

}
}

}
}

break;

case 't':
{
if (!strcmp(p, "-pruning")) {
opt = FCS_OPT_SET_PRUNING;

}
}

break;

}
}

break;

case 'o':
{
if (!strcmp(p, "ft-thread-step")) {
opt = FCS_OPT_SOFT_THREAD_STEP;

}
}

break;

case 't':
{ switch(*(p++)) {
case '-':
{
if (!strcmp(p, "name")) {
opt = FCS_OPT_ST_NAME;

}
}

break;

case 'a':
{
if (!strcmp(p, "cks-num")) {
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
{ switch(*(p++)) {
case 'e':
{
if (!strcmp(p, "sts-order")) {
opt = FCS_OPT_TESTS_ORDER;

}
}

break;

case 'r':
{
if (!strcmp(p, "im-max-stored-states")) {
opt = FCS_OPT_TRIM_MAX_STORED_STATES;

}
}

break;

}
}

break;

}
}

break;

case 'a':
{
if (!strcmp(p, "sw")) {
opt = FCS_OPT_BEFS_WEIGHTS;

}
}

break;

case 'd':
{
if (!strcmp(p, "to")) {
opt = FCS_OPT_DEPTH_TESTS_ORDER;

}
}

break;

case 'f':
{
if (!strcmp(p, "if")) {
opt = FCS_OPT_FLARES_ITERS_FACTOR;

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
if (!strcmp(p, "s")) {
opt = FCS_OPT_MAX_STORED_STATES;

}
}

break;

}
}

break;

case 'n':
{ switch(*(p++)) {
case 'f':

{
if (*p == '\0')
{

opt = FCS_OPT_NEXT_FLARE;
}
}

break;

case 'h':
{
if (!strcmp(p, "t")) {
opt = FCS_OPT_NEXT_HARD_THREAD;

}
}

break;

case 'i':

{
if (*p == '\0')
{

opt = FCS_OPT_NEXT_INSTANCE;
}
}

break;

case 's':
{
if (!strcmp(p, "t")) {
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
if (!strcmp(p, "to")) {
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
if (!strcmp(p, "ed")) {
opt = FCS_OPT_SEED;

}
}

break;

case 'p':

{
if (*p == '\0')
{

opt = FCS_OPT_SET_PRUNING;
}
}

break;

case 't':
{
if (!strcmp(p, "ep")) {
opt = FCS_OPT_SOFT_THREAD_STEP;

}
}

break;

}
}

break;

case 't':
{ switch(*(p++)) {
case 'm':
{
if (!strcmp(p, "ss")) {
opt = FCS_OPT_TRIM_MAX_STORED_STATES;

}
}

break;

case 'o':

{
if (*p == '\0')
{

opt = FCS_OPT_TESTS_ORDER;
}
}

break;

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

            freecell_solver_user_limit_iterations_long(instance, atol((*arg)));
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
                char * errstr = SMALLOC(errstr, strlen(fcs_user_errstr)+500);
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

                errstr = SMALLOC(errstr, 500);
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

                errstr = SMALLOC(errstr, 500);
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

                errstr = SMALLOC(errstr, 500);
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
            PROCESS_OPT_ARG() ;

            freecell_solver_user_set_sequence_move(
                instance,
                !strcmp((*arg), "unlimited")
            );
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

                errstr = SMALLOC(errstr, strlen((*arg))+500);

                sprintf(errstr, "Unknown game \"%s\"!\n\n", (*arg));
                *error_string = errstr;

                RET_ERROR_IN_ARG() ;
            }
            else if (ret == FCS_PRESET_CODE_FREECELLS_EXCEED_MAX)
            {
                char * errstr;

                errstr = SMALLOC(errstr, strlen((*arg))+500);
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

                errstr = SMALLOC(errstr, strlen((*arg))+500);

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

                errstr = SMALLOC(errstr, strlen((*arg))+500);

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

                errstr = SMALLOC(errstr, strlen((*arg))+500);

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

        case FCS_OPT_BEFS_WEIGHTS: /* STRINGS=-asw|--a-star-weights; */
        {
            PROCESS_OPT_ARG() ;

            {
                int i;
                /* Initialize all the Best Frist Search weights at first
                 * to 0 so
                 * we won't have partial initialization.
                 * */
                double befs_weights[FCS_NUM_BEFS_WEIGHTS];

                fc_solve_set_weights(*arg, befs_weights);

                for (i=0 ; i<FCS_NUM_BEFS_WEIGHTS ; i++)
                {
                    freecell_solver_user_set_a_star_weight(
                        instance,
                        i,
                        befs_weights[i]
                    );
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

            freecell_solver_user_limit_num_states_in_collection_long(
                instance,
                atol((*arg))
            );
        }
        break;

        case FCS_OPT_TRIM_MAX_STORED_STATES: /* STRINGS=-tmss|--trim-max-stored-states; */
        {
            PROCESS_OPT_ARG() ;

            freecell_solver_set_stored_states_trimming_limit(
                instance,
                atol((*arg))
            );
        }
        break;

        case FCS_OPT_NEXT_INSTANCE: /* STRINGS=-ni|--next-instance; */
        {
            freecell_solver_user_next_instance(instance);
        }
        break;

        case FCS_OPT_NEXT_FLARE: /* STRINGS=-nf|--next-flare; */
        {
            freecell_solver_user_next_flare(instance);
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

        case FCS_OPT_FLARE_NAME: /* STRINGS=--flare-name; */
        {
            PROCESS_OPT_ARG() ;

            freecell_solver_user_set_flare_name(instance, (*arg));
        }
        break;

        case FCS_OPT_PRELUDE: /* STRINGS=--prelude; */
        {
            PROCESS_OPT_ARG() ;

            freecell_solver_user_set_hard_thread_prelude(instance, (*arg));
        }
        break;

        case FCS_OPT_FLARES_PLAN: /* STRINGS=--flares-plan; */
        {
            PROCESS_OPT_ARG() ;

            freecell_solver_user_set_flares_plan(instance, (*arg));
        }
        break;

        case FCS_OPT_FLARES_ITERS_FACTOR: /* STRINGS=-fif|--flares-iters-factor; */
        {
            PROCESS_OPT_ARG() ;

            freecell_solver_user_set_flares_iters_factor(instance, atof(*arg));
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
                char * errstr = SMALLOC(errstr, strlen(fcs_user_errstr)+500);
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

                errstr = SMALLOC(errstr, strlen((*arg))+500);

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
                freecell_solver_str_t s;
                char * buffer;
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

                    complete_path = SMALLOC(complete_path, len);
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
                    char * err_str = SMALLOC(err_str, strlen(s)+100);
                    sprintf(err_str,
                            "Could not open file \"%s\"!\nQuitting.\n",
                            s);

                    *error_string = err_str;

                    RET_ERROR_IN_ARG() ;
                }
                fseek(f, 0, SEEK_END);
                file_len = ftell(f);
                buffer= SMALLOC(buffer, file_len+1);
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
                        (freecell_solver_str_t *)(void *)(args_man->argv + num_file_args_to_skip),
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

            int ret = freecell_solver_user_cmd_line_read_cmd_line_preset(
                instance,
                (*arg),
                known_parameters,
                error_string,
                file_nesting_count,
                opened_files_dir
            );

            if (ret == FCS_CMD_LINE_ERROR_IN_ARG)
            {
                char * err_str = SMALLOC(err_str, strlen((*arg)) + 100);
                sprintf(err_str, "Unable to load the \"%s\" configuration!\n", (*arg));
                *error_string = err_str;

                RET_ERROR_IN_ARG() ;
            }
            else if (ret == FCS_CMD_LINE_UNRECOGNIZED_OPTION)
            {
                /* Do nothing - continue */
            }
            else if (ret != FCS_CMD_LINE_OK)
            {
                return ret;
            }
        }
        break;

        case FCS_OPT_DEPTH_TESTS_ORDER: /* STRINGS=-dto|--depth-tests-order; */
        {
            PROCESS_OPT_ARG() ;

            {
                char * fcs_user_errstr;
                int min_depth;
                int ret;
                const char * s;

                min_depth = 0;

                s = (*arg);
                while(isdigit(*s))
                {
                    s++;
                }
                if (*s == ',')
                {
                    min_depth = atoi((*arg));
                    s++;
                }

                ret = freecell_solver_user_set_depth_tests_order(
                        instance,
                        min_depth,
                        (*arg),
                        &fcs_user_errstr
                        );

                if (ret != 0)
                {
                    char * errstr = SMALLOC(errstr, strlen(fcs_user_errstr)+500);
                    sprintf(
                            errstr,
                            "Error in depth tests' order!\n%s\n",
                            fcs_user_errstr
                           );
                    free(fcs_user_errstr);

                    *error_string = errstr;

                    RET_ERROR_IN_ARG() ;
                }
            }
        }
        break;

        case FCS_OPT_SET_PRUNING: /* STRINGS=-sp|--set-pruning; */
        {
            int ret;
            char * fcs_user_errstr;

            PROCESS_OPT_ARG() ;

            ret = freecell_solver_user_set_pruning(
                    instance,
                    (*arg),
                    &fcs_user_errstr
                    );

            if (ret != 0)
            {
                char * errstr = SMALLOC(errstr, strlen(fcs_user_errstr)+500);
                sprintf(
                    errstr,
                    "Error in the optimization scan's pruning!\n%s\n",
                    fcs_user_errstr
                    );
                free(fcs_user_errstr);

                *error_string = errstr;

                RET_ERROR_IN_ARG() ;
            }
        }
        break;

        case FCS_OPT_CACHE_LIMIT: /* STRINGS=--cache-limit; */
        {
            PROCESS_OPT_ARG() ;

            freecell_solver_user_set_cache_limit(
                instance,
                atol(*arg)
            );
        }
        break;

        case FCS_OPT_FLARES_CHOICE: /* STRINGS=--flares-choice; */
        {
            PROCESS_OPT_ARG() ;

            if (freecell_solver_user_set_flares_choice(
                instance,
                (*arg)
            ) != 0)
            {
                char * errstr = SMALLOC(errstr, strlen(*arg)+500);
                sprintf(
                    errstr,
                    "Unknown flares choice argument '%s'.\n",
                    (*arg)
                );
                *error_string = errstr;

                RET_ERROR_IN_ARG() ;
            }
        }
        break;

        }
        /* OPT-PARSE-END */
    }

    *last_arg = arg-&(argv[0]);
    return FCS_CMD_LINE_OK;
}

DLLEXPORT int freecell_solver_user_cmd_line_parse_args(
    void * instance,
    int argc,
    freecell_solver_str_t argv[],
    int start_arg,
    freecell_solver_str_t * known_parameters,
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

