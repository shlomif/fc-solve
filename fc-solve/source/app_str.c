#include <string.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>

#define GROW_BY 4000

struct freecell_solver_append_string_struct
{
    char * buffer;
    char * end_of_buffer;
    int max_size;
    int size_of_margin;
};

typedef struct freecell_solver_append_string_struct freecell_solver_append_string_t;

freecell_solver_append_string_t * freecell_solver_append_string_alloc(int size_margin)
{
    freecell_solver_append_string_t * app_str;

    if (size_margin > GROW_BY)
    {
        return NULL;
    }

    app_str = malloc(sizeof(freecell_solver_append_string_t));
    app_str->max_size = GROW_BY;
    app_str->end_of_buffer = app_str->buffer = malloc(app_str->max_size);
    app_str->size_of_margin = size_margin;

    return app_str;
}

int freecell_solver_append_string_sprintf(
    freecell_solver_append_string_t * app_str,
    char * format,
    ...
    )
{
    int num_chars_written;
    va_list my_va_list;

    va_start(my_va_list, format);
    num_chars_written = vsprintf(app_str->end_of_buffer, format, my_va_list);
    app_str->end_of_buffer += num_chars_written;
    /*
     * Check to see if we don't have enough space in which case we should
     * resize
     * */
    if (app_str->buffer + app_str->max_size - app_str->end_of_buffer < app_str->size_of_margin)
    {
        char * old_buffer = app_str->buffer;
        app_str->max_size += GROW_BY;
        app_str->buffer = realloc(app_str->buffer, app_str->max_size);
        /*
         * Adjust end_of_buffer to the new buffer start
         * */
        app_str->end_of_buffer = app_str->buffer + (app_str->end_of_buffer - old_buffer);
    }

    return num_chars_written;
}

char * freecell_solver_append_string_finalize(
    freecell_solver_append_string_t * app_str
    )
{
    char * ret;
    ret = strdup(app_str->buffer);
    free(app_str->buffer);
    free(app_str);
    return ret;
}
