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
 * range_solvers_binary_output.h - header file for range solvers binary
 * output.
 */
#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>

#include "rinutils.h"
#include "fcs_cl.h"

typedef struct
{
    FILE * fh;
    char * buffer;
    char * buffer_end;
    char * ptr;
    const char * filename;
} binary_output_t;

#define BINARY_OUTPUT_NUM_INTS 16
#define SIZE_INT 4
static void print_int(binary_output_t * const bin, int val)
{
    if (! bin->fh)
    {
        return;
    }
    unsigned char * const buffer = (unsigned char * const)bin->ptr;
    for (int p=0 ; p < SIZE_INT ; p++)
    {
        buffer[p] = (unsigned char)(val & 0xFF);
        val >>= 8;
    }
    bin->ptr += SIZE_INT;
    if (bin->ptr == bin->buffer_end)
    {
        fwrite(bin->buffer, 1, bin->ptr - bin->buffer, bin->fh);
        fflush(bin->fh);
        /* Reset ptr to the beginning */
        bin->ptr = bin->buffer;
    }
}

static GCC_INLINE void bin_close(binary_output_t * bin)
{
    if (bin->filename)
    {
        fwrite(bin->buffer, 1, bin->ptr - bin->buffer, bin->fh);
        fflush(bin->fh);
        fclose(bin->fh);
        bin->fh = NULL;
        bin->filename = NULL;
    }
}

static GCC_INLINE fcs_bool_t read_int(FILE * const f, int * const dest)
{
    unsigned char buffer[SIZE_INT];
    if (fread(buffer, 1, SIZE_INT, f) != SIZE_INT)
    {
        return TRUE;
    }
    *dest = (buffer[0]+((buffer[1]+((buffer[2]+((buffer[3])<<8))<<8))<<8));

    return FALSE;
}

static void read_int_wrapper(FILE * const in, int * const var)
{
    if (read_int(in, var))
    {
        fprintf(stderr, "%s",
            "Output file is too short to deduce the configuration!\n"
        );
        exit(-1);
    }
}

static GCC_INLINE void bin_init(binary_output_t * const bin,
    int * const start_board_ptr,
    int * const end_board_ptr,
    fcs_int_limit_t * const total_iterations_limit_per_board_ptr
    )
{
    if (bin->filename)
    {
        FILE * in;

        bin->buffer = malloc(sizeof(int) * BINARY_OUTPUT_NUM_INTS);
        bin->ptr = bin->buffer;
        bin->buffer_end = bin->buffer + sizeof(int)*BINARY_OUTPUT_NUM_INTS;


        in = fopen(bin->filename, "rb");
        if (in == NULL)
        {
            bin->fh = fopen(bin->filename, "wb");
            if (! bin->fh)
            {
                fprintf(stderr, "Could not open \"%s\" for writing!\n", bin->filename);
                exit(-1);
            }

            print_int(bin, *start_board_ptr);
            print_int(bin, *end_board_ptr);
            print_int(bin, (int)(*total_iterations_limit_per_board_ptr));
        }
        else
        {
            read_int_wrapper(in, start_board_ptr);
            read_int_wrapper(in, end_board_ptr);
            {
                int val;
                read_int_wrapper(in, &val);
                *total_iterations_limit_per_board_ptr = (fcs_int_limit_t)val;
            }

            fseek(in, 0, SEEK_END);
            const long file_len = ftell(in);
            if (file_len % 4 != 0)
            {
                fprintf(stderr, "%s", "Output file has an invalid length. Terminating.\n");
                exit(-1);
            }
            *start_board_ptr += (file_len-12)/4;
            if (*start_board_ptr >= *end_board_ptr)
            {
                fprintf(stderr, "%s", "Output file was already finished being generated.\n");
                exit(-1);
            }
            fclose(in);
            bin->fh = fopen(bin->filename, "ab");
            if (! bin->fh)
            {
                fprintf(stderr, "Could not open \"%s\" for writing!\n", bin->filename);
                exit(-1);
            }
        }
    }
    else
    {
        bin->fh = NULL;
        bin->buffer
            = bin->ptr
            = bin->buffer_end
            = NULL;
    }
}

#ifdef __cplusplus
}
#endif
