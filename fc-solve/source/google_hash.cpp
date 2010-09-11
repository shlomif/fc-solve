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

#if ((FCS_WHICH_STATES_GOOGLE_HASH == FCS_WHICH_STATES_GOOGLE_HASH__DENSE) || (FCS_WHICH_COLUMNS_GOOGLE_HASH == FCS_WHICH_COLUMNS_GOOGLE_HASH__DENSE))
#include <google/dense_hash_set>
#endif

#if ((FCS_WHICH_STATES_GOOGLE_HASH == FCS_WHICH_STATES_GOOGLE_HASH__SPARSE) || (FCS_WHICH_COLUMNS_GOOGLE_HASH == FCS_WHICH_COLUMNS_GOOGLE_HASH__SPARSE))
#include <google/sparse_hash_set>
#endif

#include "state.h"

#if ((FCS_WHICH_STATES_GOOGLE_HASH == FCS_WHICH_STATES_GOOGLE_HASH__DENSE) || (FCS_WHICH_COLUMNS_GOOGLE_HASH == FCS_WHICH_COLUMNS_GOOGLE_HASH__DENSE))
using google::dense_hash_set;      // namespace where class lives by default
#endif

#if ((FCS_WHICH_STATES_GOOGLE_HASH == FCS_WHICH_STATES_GOOGLE_HASH__SPARSE) || (FCS_WHICH_COLUMNS_GOOGLE_HASH == FCS_WHICH_COLUMNS_GOOGLE_HASH__SPARSE))
using google::sparse_hash_set;      // namespace where class lives by default
#endif

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

#if (FCS_WHICH_STATES_GOOGLE_HASH == FCS_WHICH_STATES_GOOGLE_HASH__SPARSE)
typedef sparse_hash_set<char*, state_hash, state_equality> StatesGoogleHash;
#else
typedef dense_hash_set<char*, state_hash, state_equality> StatesGoogleHash;
#endif

extern "C" fcs_states_google_hash_handle_t fc_solve_states_google_hash_new()
{
    StatesGoogleHash * ret = new StatesGoogleHash;

#if (FCS_WHICH_STATES_GOOGLE_HASH == FCS_WHICH_STATES_GOOGLE_HASH__DENSE)
    ret->set_empty_key(NULL);
#endif

    return (fcs_states_google_hash_handle_t)(ret);
}

/*
 * Returns 0 if the key is new and the key/val pair was inserted.
 *      - in that case *existing_key / *existing_val will be set to key
 *      and val respectively.
 * Returns 1 if the key is not new and *existing_key / *existing_val
 * was set to it.
 */
extern "C" fcs_bool_t fc_solve_states_google_hash_insert(
    fcs_states_google_hash_handle_t void_hash,
    void * key,
    void * * existing_key)
{
    StatesGoogleHash * hash = (StatesGoogleHash *)void_hash;
    std::pair<StatesGoogleHash::iterator, bool> result =
        hash->insert((char *)key)
        ;

    /* If an insertion took place. */
    if (result.second)
    {
        *existing_key = NULL;
        return FALSE;
    }
    else
    {
        *existing_key = (*(result.first));

        return TRUE;
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

extern void fc_solve_states_google_hash_foreach(
    fcs_states_google_hash_handle_t void_hash,
    fcs_bool_t (*should_delete_ptr)(void * key, void * context),
    void * context
    )
{
    StatesGoogleHash * hash = (StatesGoogleHash *)void_hash;

    for(StatesGoogleHash::iterator it =  hash->begin(); it != hash->end(); ++it)
    {
        if (should_delete_ptr(*(it), context))
        {
            hash->erase(it);
        }
    }
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

#if (FCS_WHICH_COLUMNS_GOOGLE_HASH == FCS_WHICH_COLUMNS_GOOGLE_HASH__SPARSE)
typedef sparse_hash_set<char*, column_hash, column_equality> ColumnsGoogleHash;
#else
typedef dense_hash_set<char*, column_hash, column_equality> ColumnsGoogleHash;
#endif

extern "C" fcs_columns_google_hash_handle_t fc_solve_columns_google_hash_new()
{
    ColumnsGoogleHash * ret = new ColumnsGoogleHash;

#if (FCS_WHICH_STATES_GOOGLE_HASH == FCS_WHICH_COLUMNS_GOOGLE_HASH__DENSE)
    ret->set_empty_key(NULL);
#endif

    return (fcs_columns_google_hash_handle_t)(ret);
}

/*
 * Returns 0 if the key is new and the key/val pair was inserted.
 *      - in that case *existing_key / *existing_val will be set to key
 *      and val respectively.
 * Returns 1 if the key is not new and *existing_key / *existing_val
 * was set to it.
 */
extern "C" fcs_bool_t fc_solve_columns_google_hash_insert(
    fcs_columns_google_hash_handle_t void_hash,
    void * key,
    void * * existing_key)
{
    ColumnsGoogleHash * hash = (ColumnsGoogleHash *)void_hash;
    std::pair<ColumnsGoogleHash::iterator, bool> result =
        hash->insert((char *)key)
        ;

    /* If an insertion took place. */
    if (result.second)
    {
        *existing_key = NULL;
        return FALSE;
    }
    else
    {
        *existing_key = (*(result.first));

        return TRUE;
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
