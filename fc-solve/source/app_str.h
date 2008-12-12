#include <string.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>

#ifndef FC_SOLVE__APP_STR_H
#define FC_SOLVE__APP_STR_H

#ifdef __cplusplus
extern "C" {
#endif

struct fc_solve_append_string_struct
{
    char * buffer;
    char * end_of_buffer;
    int max_size;
    int size_of_margin;
};

typedef struct fc_solve_append_string_struct fc_solve_append_string_t;

extern fc_solve_append_string_t * fc_solve_append_string_alloc(int size_margin);

extern int fc_solve_append_string_sprintf(
    fc_solve_append_string_t * app_str,
    char * format,
    ...
    );

extern char * fc_solve_append_string_finalize(
    fc_solve_append_string_t * app_str
    );

#ifdef __cplusplus
}
#endif

#endif /* #ifndef FC_SOLVE__APP_STR_H */
