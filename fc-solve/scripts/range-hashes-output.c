/*
 * This file is part of Freecell Solver. It is subject to the license terms in
 * the COPYING.txt file found in the top-level directory of this distribution
 * and at http://fc-solve.shlomifish.org/docs/distro/COPYING.html . No part of
 * Freecell Solver, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the COPYING file.
 *
 * Copyright (c) 2000 Shlomi Fish
 */
// summarizing_solver.c - solves several indices of deals and prints a summary
// of the solutions of each one.
#include "freecell-solver/fcs_conf.h"
#include "freecell-solver/fcs_cl.h"
#include "range_solvers_gen_ms_boards.h"
#include "handle_parsing.h"
#define XXH_PRIVATE_API
#include "xxhash.h"

typedef struct
{
    long start, end;
} deals_range;
static deals_range *mydeals = NULL;
static size_t num_deals = 0, max_num_deals = 0;

static inline bool is_valid(const deals_range r) { return (r.start <= r.end); }
static inline void append(const long start, const long end)
{
    if (num_deals == max_num_deals)
    {
        mydeals = SREALLOC(mydeals, max_num_deals += 1000);
        if (!mydeals)
        {
            exit_error("Number of deals exceeded %ld!\n", (long)max_num_deals);
        }
    }
    const deals_range new_r = (deals_range){.start = start, .end = end};
    if ((num_deals == 0) ||
        (!(is_valid(new_r) && is_valid(mydeals[num_deals - 1]) &&
            (start == mydeals[num_deals - 1].end + 1))))
    {
        mydeals[num_deals++] = new_r;
    }
    else
    {
        mydeals[num_deals - 1].end = end;
    }
}

int main(int argc, char *argv[])
{
    int arg = 1;

    while (arg < argc && (strcmp(argv[arg], "--")))
    {
        if (!strcmp(argv[arg], "seq"))
        {
            if (++arg >= argc)
            {
                exit_error("seq without args!\n");
            }
            const long start = atol(argv[arg]);
            if (++arg >= argc)
            {
                exit_error("seq without args!\n");
            }
            const long end = atol(argv[arg++]);
            append(start, end);
        }
        else if (!strcmp(argv[arg], "slurp"))
        {
            if (++arg >= argc)
            {
                exit_error("slurp without arg!\n");
            }
            FILE *const f = fopen(argv[arg++], "rt");
            if (!f)
            {
                exit_error("Cannot slurp file!\n");
            }
            while (!feof(f))
            {
                long deal;
                if (fscanf(f, "%ld", &deal) == 1)
                {
                    append(deal, deal);
                }
            }
            fclose(f);
        }
        else
        {
            const long deal = atol(argv[arg++]);
            append(deal, deal);
        }
    }

    char buffer[2000];
    FILE * o[256];
    for (size_t i = 0; i < 256; ++i)
    {
        char fn[100];
        sprintf(fn, "parts/%02x.txt", (int)i);
        if (!(o[i] = fopen(fn, "at")))
        {
            exit_error("cannot open %s\n", fn);
        }
    }
    get_board_l(1, buffer);
    LAST(buffer) = '\0';
    const_AUTO(len, strlen(buffer));

    for (size_t deal_idx = 0; deal_idx < num_deals; ++deal_idx)
    {
        const_AUTO(board_range, mydeals[deal_idx]);
        const unsigned long long end = board_range.end;
        for (unsigned long long board_num = board_range.start; board_num <= end;
             ++board_num)
        {
            get_board_l(board_num, buffer);
            LAST(buffer) = '\0';
            XXH64_hash_t hash = XXH64(buffer, len, 0);
            fprintf(o[hash&0xFF], "%014lx\t%llu\n", (long)(hash >> 8), board_num);
            if ((board_num & 0xFFFFF) == 0)
            {
                fprintf(stderr, "Reached %llu\n", board_num);
                for (size_t i = 0; i < 256; ++i)
                {
                    fflush(o[i]);
                }
            }
        }
    }
    for (size_t i = 0; i < 256; ++i)
    {
        fclose(o[i]);
    }
    free(mydeals);

    return 0;
}
