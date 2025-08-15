// dbm_rawdump_parent_lookup.h
// Copyright (C) 2025 Shlomi Fish <shlomif@cpan.org>
//
// Distributed under terms of the MIT license.
#pragma once

#include <assert.h>
#include <stdio.h>

// Setting FCS_DBM__ENABLE_PARENT_LOOKUP to 0, makes the depth-dbm-fc-solver
// run faster, but prevents calculating the solution leading
// to the end state in case the deal turns out to be winnable.
//
// Disable at your own risk.
#define FCS_DBM__ENABLE_PARENT_LOOKUP 1

typedef struct
{
    FILE *writer;
#if FCS_DBM__ENABLE_PARENT_LOOKUP
    char filename[2000];
#endif
} fcs_dbm__rawdump__parent_lookup__type;

static void parent_lookup__create(
    fcs_dbm__rawdump__parent_lookup__type *const obj, char *filename)
{
#if FCS_DBM__ENABLE_PARENT_LOOKUP
    strncpy(obj->filename, filename, sizeof(obj->filename));
    LAST(obj->filename) = '\0';
    FILE *const writer = fopen(obj->filename, "wb");
    assert(writer);
    obj->writer = writer;
#else
    obj->writer = NULL;
#endif
}

static void parent_lookup__add(fcs_dbm__rawdump__parent_lookup__type *const obj,
    const fcs_dbm_record *const rec)
{
#if FCS_DBM__ENABLE_PARENT_LOOKUP
    fwrite(rec, sizeof(*rec), 1, obj->writer);
#endif
}

static void parent_lookup__finish_writing(
    fcs_dbm__rawdump__parent_lookup__type *const obj)
{
#if FCS_DBM__ENABLE_PARENT_LOOKUP
    fclose(obj->writer);
    obj->writer = NULL;
#endif
}

static bool rawdump_parent_lookup__lookup_parent(
    fcs_dbm__rawdump__parent_lookup__type *const obj,
    const fcs_encoded_state_buffer key, fcs_encoded_state_buffer *const parent)
{
#if FCS_DBM__ENABLE_PARENT_LOOKUP
    FILE *const reader = fopen(obj->filename, "rb");
    assert(reader);
#define MAX_NUM_ITEMS 1024
    fcs_dbm_record page[MAX_NUM_ITEMS];
    size_t num_items;
    do
    {
        num_items = fread(page, sizeof(page[0]), MAX_NUM_ITEMS, reader);
        for (size_t i = 0; i < num_items; ++i)
        {
            if (!memcmp(&page[i].key, &key, sizeof(key)))
            {
                *parent = page[i].parent;
                fclose(reader);
                return true;
            }
        }
    } while (num_items > 0);
#undef MAX_NUM_ITEMS
    fclose(reader);
    return false;
#else
    fprintf(
        stdout, "%s() was CALLED!\n", "rawdump_parent_lookup__lookup_parent");
    fflush(stdout);
    return false;
#endif
}
