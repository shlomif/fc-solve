
#ifndef __FCS_CL_H
#define __FCS_CL_H

#include "fcs_user.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef int (*freecell_solver_user_cmd_line_known_commands_callback_t)
    (
     void * instance,
     int argc,
     char * argv[],
     int arg_index,
     int * num_to_skip,
     int * ret,
     void * context
     );

enum FCS_CMD_LINE_CALLBACK_RET_VALUES
{
    FCS_CMD_LINE_OK,
    FCS_CMD_LINE_SKIP,
    FCS_CMD_LINE_STOP,
    FCS_CMD_LINE_UNRECOGNIZED_OPTION,
    FCS_CMD_LINE_PARAM_WITH_NO_ARG,
    FCS_CMD_LINE_ERROR_IN_ARG,

    FCS_CMD_LINE_USER = 0x10000,
};

extern int freecell_solver_user_cmd_line_parse_args(
    void * instance,
    int argc,
    char * argv[],
    int start_arg,
    char * * known_parameters,
    freecell_solver_user_cmd_line_known_commands_callback_t callback,
    void * callback_context,
    char * * error_string,
    int * last_arg
    );

extern int freecell_solver_user_cmd_line_parse_args_with_file_nesting_count(
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
    );


#ifdef __cplusplus
};
#endif

#endif /* #ifndef __FCS_CL_H */
