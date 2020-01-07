// This file is part of Freecell Solver. It is subject to the license terms in
// the COPYING.txt file found in the top-level directory of this distribution
// and at http://fc-solve.shlomifish.org/docs/distro/COPYING.html . No part of
// Freecell Solver, including this file, may be copied, modified, propagated,
// or distributed except according to the terms contained in the COPYING file.
//
// Copyright (c) 2000 Shlomi Fish
// deals_populator.h
#pragma once

typedef struct
{
    long start, end;
} deals_range;
static deals_range *deals_ranges_array = NULL;
static size_t num_deals = 0, max_num_deals = 0;

static inline bool deals_range__is_valid(const deals_range r)
{
    return (r.start <= r.end);
}
static inline void deals_range__append(const long start, const long end)
{
    if (num_deals == max_num_deals)
    {
        deals_ranges_array =
            SREALLOC(deals_ranges_array, max_num_deals += 1000);
        if (!deals_ranges_array)
        {
            exit_error("Number of deals exceeded %ld!\n", (long)max_num_deals);
        }
    }
    const deals_range new_r = (deals_range){.start = start, .end = end};
    if ((num_deals == 0) ||
        (!(deals_range__is_valid(new_r) &&
            deals_range__is_valid(deals_ranges_array[num_deals - 1]) &&
            (start == deals_ranges_array[num_deals - 1].end + 1))))
    {
        deals_ranges_array[num_deals++] = new_r;
    }
    else
    {
        deals_ranges_array[num_deals - 1].end = end;
    }
}

static inline int populate_deals_from_argv(
    const int argc, char *argv[], int arg)
{
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
            deals_range__append(start, end);
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
                    deals_range__append(deal, deal);
                }
            }
            fclose(f);
        }
        else
        {
            const long deal = atol(argv[arg++]);
            deals_range__append(deal, deal);
        }
    }
    return arg;
}

static inline void deals_ranges__free(void) { free(deals_ranges_array); }

#define DEALS_ITERATE__START(var)                                              \
    for (size_t deal_idx = 0; deal_idx < num_deals; ++deal_idx)                \
    {                                                                          \
        const_AUTO(board_range, deals_ranges_array[deal_idx]);                 \
        for (long var = board_range.start; var <= board_range.end; ++var)      \
        {

#define DEALS_ITERATE__END()                                                   \
    }                                                                          \
    }
