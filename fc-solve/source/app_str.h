#include <string.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>

#ifndef FC_SOLVE__APP_STR_H
#define FC_SOLVE__APP_STR_H

#ifdef __cplusplus
extern "C" {
#endif

struct freecell_solver_append_string_struct
{
    char * buffer;
    char * end_of_buffer;
    int max_size;
    int size_of_margin;
};

typedef struct freecell_solver_append_string_struct freecell_solver_append_string_t;

extern freecell_solver_append_string_t * freecell_solver_append_string_alloc(int size_margin);

extern int freecell_solver_append_string_sprintf(
    freecell_solver_append_string_t * app_str,
    char * format,
    ...
    );

extern char * freecell_solver_append_string_finalize(
    freecell_solver_append_string_t * app_str
    );

#ifdef __cplusplus
}
#endif

#endif /* #ifndef FC_SOLVE__APP_STR_H */
