// This file is part of Freecell Solver. It is subject to the license terms in
// the COPYING.txt file found in the top-level directory of this distribution
// and at http://fc-solve.shlomifish.org/docs/distro/COPYING.html . No part of
// Freecell Solver, including this file, may be copied, modified, propagated,
// or distributed except according to the terms contained in the COPYING file.
//
// Copyright (c) 2000 Shlomi Fish
#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "rinutils/rinutils.h"
#include "freecell-solver/fcs_cl.h"

typedef struct
{
    FILE *fh;
    char *buffer;
    char *buffer_end;
    char *ptr;
    const char *filename;
} fcs_binary_output;

static const fcs_binary_output INIT_BINARY_OUTPUT = {.filename = NULL};

#define BINARY_OUTPUT_BUF_SIZE (sizeof(int) * 16)
#define SIZE_INT 4

static inline void write_me(fcs_binary_output *const bin)
{
    fwrite(bin->buffer, 1, (size_t)(bin->ptr - bin->buffer), bin->fh);
    fflush(bin->fh);
}

static void print_int(fcs_binary_output *const bin, int val)
{
    if (!bin->fh)
    {
        return;
    }
    unsigned char *const buffer = (unsigned char *const)bin->ptr;
    for (int p = 0; p < SIZE_INT; p++)
    {
        buffer[p] = (unsigned char)(val & 0xFF);
        val >>= 8;
    }
    bin->ptr += SIZE_INT;
    if (bin->ptr == bin->buffer_end)
    {
        write_me(bin);
        bin->ptr = bin->buffer;
    }
}

static inline void bin_close(fcs_binary_output *const bin)
{
    if (bin->filename)
    {
        write_me(bin);
        fclose(bin->fh);
        bin->fh = NULL;
        bin->filename = NULL;
    }
}

static inline bool read_int(FILE *const f, long long *const dest)
{
    unsigned char buffer[SIZE_INT];
    if (fread(buffer, 1, SIZE_INT, f) != SIZE_INT)
    {
        return true;
    }
    *dest = (buffer[0] +
             ((buffer[1] + ((buffer[2] + ((buffer[3]) << 8)) << 8)) << 8));

    return false;
}

static void read_int_wrapper(FILE *const in, long long *const var)
{
    if (read_int(in, var))
    {
        exit_error(
            "%s", "Output file is too short to deduce the configuration!\n");
    }
}

static inline void bin_init(fcs_binary_output *const bin,
    fc_solve_ms_deal_idx_type *const start_board_ptr,
    fc_solve_ms_deal_idx_type *const end_board_ptr,
    fcs_int_limit_t *const total_iterations_limit_per_board_ptr)
{
    if (bin->filename)
    {
        bin->buffer = malloc(BINARY_OUTPUT_BUF_SIZE);
        bin->ptr = bin->buffer;
        bin->buffer_end = bin->buffer + BINARY_OUTPUT_BUF_SIZE;

        FILE *const in = fopen(bin->filename, "rb");
        if (!in)
        {
            if (!(bin->fh = fopen(bin->filename, "wb")))
            {
                exit_error(
                    "Could not open \"%s\" for writing!\n", bin->filename);
            }

            print_int(bin, (int)*start_board_ptr);
            print_int(bin, (int)*end_board_ptr);
            print_int(bin, (int)(*total_iterations_limit_per_board_ptr));
        }
        else
        {
            long long temp = 0;
            read_int_wrapper(in, &temp);
            *start_board_ptr = (fc_solve_ms_deal_idx_type)temp;
            temp = 0;
            read_int_wrapper(in, &temp);
            *end_board_ptr = (fc_solve_ms_deal_idx_type)temp;
            {
                long long val;
                read_int_wrapper(in, &val);
                *total_iterations_limit_per_board_ptr = (fcs_int_limit_t)val;
            }

            fseek(in, 0, SEEK_END);
            const long file_len = ftell(in);
            if (file_len % 4 != 0)
            {
                exit_error(
                    "%s", "Output file has an invalid length. Terminating.\n");
            }
            *start_board_ptr +=
                (fc_solve_ms_deal_idx_type)((file_len - 12) / 4);
            if (*start_board_ptr >= *end_board_ptr)
            {
                exit_error("%s",
                    "Output file was already finished being generated.\n");
            }
            fclose(in);
            if (!(bin->fh = fopen(bin->filename, "ab")))
            {
                exit_error(
                    "Could not open \"%s\" for writing!\n", bin->filename);
            }
        }
    }
    else
    {
        bin->fh = NULL;
        bin->buffer = bin->ptr = bin->buffer_end = NULL;
    }
}

#ifdef __cplusplus
}
#endif
