/* Copyright (c) 2010 Shlomi Fish
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
 * google_hash.cpp - module file for Google's dense_hash_map as adapted
 * for Freecell Solver.
 */

#include "google_hash.h"

#include <google/dense_hash_map>

#include "state.h"

using google::dense_hash_map;      // namespace where class lives by default


typedef  unsigned long  int  ub4;   /* unsigned 4-byte quantities */
typedef  unsigned       char ub1;

static GCC_INLINE ub4 perl_hash_function(
    register ub1 *s_ptr,        /* the key */
    register ub4  length        /* the length of the key */
    )
{
    register ub4  hash_value_int = 0;
    register ub1 * s_end = s_ptr+length;

    while (s_ptr < s_end)
    {
        hash_value_int += (hash_value_int << 5) + *(s_ptr++);
    }
    hash_value_int += (hash_value_int>>5);

    return hash_value_int;
}

#if (FCS_STATE_STORAGE == FCS_STATE_STORAGE_GOOGLE_DENSE_HASH)

struct state_equality
{
  bool operator()(const char* s1, const char* s2) const
  {
      return (s1 == s2) || (s1 && s2 && (fc_solve_state_compare(s1, s2) == 0));
  }
};


struct state_hash
{
  int operator()(const char* s1) const
  {
      return perl_hash_function((ub1 *)s1, sizeof(fcs_state_t));
  }
};

typedef dense_hash_map<char*, char *, state_hash, state_equality> StatesGoogleHash;

extern "C" fcs_states_google_hash_handle_t fc_solve_states_google_hash_new()
{
    StatesGoogleHash * ret = new StatesGoogleHash;

    ret->set_empty_key(NULL);

    return (fcs_states_google_hash_handle_t)(ret);
}

/*
 * Returns 0 if the key is new and the key/val pair was inserted.
 *      - in that case *existing_key / *existing_val will be set to key
 *      and val respectively.
 * Returns 1 if the key is not new and *existing_key / *existing_val
 * was set to it.
 */
extern "C" int fc_solve_states_google_hash_insert(
    fcs_states_google_hash_handle_t void_hash,
    void * key,
    void * val,
    void * * existing_key,
    void * * existing_val)
{
    StatesGoogleHash * hash = (StatesGoogleHash *)void_hash;
    std::pair<StatesGoogleHash::iterator, bool> result =
        hash->insert(std::make_pair((char *)key, (char *)val))
        ;

    /* If an insertion took place. */
    if (result.second)
    {
        *existing_key = NULL;
        *existing_val = NULL;
        return 0;
    }
    else
    {
        *existing_key = (*(result.first)).first;
        *existing_val = (*(result.first)).second;

        return 1;
    }
}

extern "C" void fc_solve_states_google_hash_free(
        fcs_states_google_hash_handle_t void_hash
        )
{
    StatesGoogleHash * hash = (StatesGoogleHash *)void_hash;

    delete hash;

    return;
}

#endif

#if (FCS_STACK_STORAGE == FCS_STACK_STORAGE_GOOGLE_DENSE_HASH)

struct column_equality
{
  bool operator()(const char* s1, const char* s2) const
  {
      return (s1 == s2) || (s1 && s2 && (fc_solve_stack_compare_for_comparison(s1, s2) == 0));
  }
};

struct column_hash
{
  int operator()(const char* s1) const
  {
      return perl_hash_function((ub1 *)s1, fcs_col_len(s1)+1);
  }
};

typedef dense_hash_map<char*, char *, column_hash, column_equality> ColumnsGoogleHash;

extern "C" fcs_columns_google_hash_handle_t fc_solve_columns_google_hash_new()
{
    ColumnsGoogleHash * ret = new ColumnsGoogleHash;

    ret->set_empty_key(NULL);

    return (fcs_columns_google_hash_handle_t)(ret);
}

/*
 * Returns 0 if the key is new and the key/val pair was inserted.
 *      - in that case *existing_key / *existing_val will be set to key
 *      and val respectively.
 * Returns 1 if the key is not new and *existing_key / *existing_val
 * was set to it.
 */
extern "C" int fc_solve_columns_google_hash_insert(
    fcs_columns_google_hash_handle_t void_hash,
    void * key,
    void * val,
    void * * existing_key,
    void * * existing_val)
{
    ColumnsGoogleHash * hash = (ColumnsGoogleHash *)void_hash;
    std::pair<ColumnsGoogleHash::iterator, bool> result =
        hash->insert(std::make_pair((char *)key, (char *)val))
        ;

    /* If an insertion took place. */
    if (result.second)
    {
        *existing_key = NULL;
        *existing_val = NULL;
        return 0;
    }
    else
    {
        *existing_key = (*(result.first)).first;
        *existing_val = (*(result.first)).second;

        return 1;
    }
}

extern "C" void fc_solve_columns_google_hash_free(
        fcs_columns_google_hash_handle_t void_hash
        )
{
    ColumnsGoogleHash * hash = (ColumnsGoogleHash *)void_hash;

    delete hash;

    return;
}

#endif
