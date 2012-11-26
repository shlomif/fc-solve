/* Copyright 2009
 * Kaz Kylheku <kkylheku@gmail.com>
 * Vancouver, Canada
 * All rights reserved.
 *
 * BSD License:
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 *   1. Redistributions of source code must retain the above copyright
 *      notice, this list of conditions and the following disclaimer.
 *   2. Redistributions in binary form must reproduce the above copyright
 *      notice, this list of conditions and the following disclaimer in
 *      the documentation and/or other materials provided with the
 *      distribution.
 *   3. The name of the author may not be used to endorse or promote
 *      products derived from this software without specific prior
 *      written permission.
 *
 * THIS SOFTWARE IS PROVIDED ``AS IS'' AND WITHOUT ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 */
/*
 * Adapted from kazlib's dict.h by Shlomi Fish for Freecell Solver:
 *
 * http://git.savannah.gnu.org/cgit/kazlib.git/
 *
 * */
#ifndef FC_SOLVE__KAZ_TREE_H
#define FC_SOLVE__KAZ_TREE_H

#include <limits.h>
#ifdef KAZLIB_SIDEEFFECT_DEBUG
#include "sfx.h"
#endif

#include "meta_alloc.h"
/*
 * Blurb for inclusion into C++ translation units
 */

#ifdef __cplusplus
extern "C" {
#endif

typedef unsigned long dictcount_t;
#define DICTCOUNT_T_MAX ULONG_MAX

/*
 * The dictionary is implemented as a red-black tree
 */

typedef enum { dnode_red, dnode_black } dnode_color_t;

/*
 * This is set by dbm_kaztree.c to conserve space when used in the delta-states
 * nodes.
 */
#ifdef FCS_KAZ_TREE_USE_RECORD_DICT_KEY
typedef fcs_dbm_record_t dict_key_t;
typedef dict_key_t * dict_ret_key_t;
#else
typedef const void * dict_key_t;
typedef dict_key_t dict_ret_key_t;
#endif

#define DNODE_NEXT(node) ((node)->dict_left)

typedef struct dnode_t {
    #if defined(DICT_IMPLEMENTATION) || !defined(KAZLIB_OPAQUE_DEBUG)
    struct dnode_t *dict_left;
    struct dnode_t *dict_right;
    struct dnode_t *dict_parent;
    dnode_color_t dict_color;
    dict_key_t dict_key;
#ifdef NO_FC_SOLVE
    void *dict_data;
#endif
    #else
    int dict_dummy;
    #endif
} dnode_t;

typedef int (*dict_comp_t)(const void *, const void *, void *);
/* Removed from fc-solve */
#if 0
typedef dnode_t *(*dnode_alloc_t)(void *);
typedef void (*dnode_free_t)(dnode_t *, void *);
#endif

typedef struct dict_t {
    #if defined(DICT_IMPLEMENTATION) || !defined(KAZLIB_OPAQUE_DEBUG)
    dnode_t dict_nilnode;
#ifdef NO_FC_SOLVE
    dictcount_t dict_nodecount;
    dictcount_t dict_maxcount;
#endif
    dict_comp_t dict_compare;
    /* Removed from fc-solve */
#if 0
    dnode_alloc_t dict_allocnode;
    dnode_free_t dict_freenode;
#else
    fcs_compact_allocator_t dict_allocator;
    dnode_t * dict_recycle_bin;
#endif
    void *dict_context;
#ifdef NO_FC_SOLVE
    int dict_dupes;
#endif
    #else
    int dict_dummmy;
    #endif
} dict_t;

typedef void (*dnode_process_t)(dict_t *, dnode_t *, void *);

typedef struct dict_load_t {
    #if defined(DICT_IMPLEMENTATION) || !defined(KAZLIB_OPAQUE_DEBUG)
    dict_t *dict_dictptr;
    dnode_t dict_nilnode;
    #else
    int dict_dummmy;
    #endif
} dict_load_t;

#ifdef NO_FC_SOLVE
extern dict_t *dict_create(dictcount_t, dict_comp_t, void * context);
#else
extern dict_t * fc_solve_kaz_tree_create(dict_comp_t, void * context, fcs_meta_compact_allocator_t * meta_allocator);
#endif
#if 0
extern void dict_set_allocator(dict_t *, dnode_alloc_t, dnode_free_t, void *);
#endif
extern void fc_solve_kaz_tree_destroy(dict_t *);
extern void fc_solve_kaz_tree_free_nodes(dict_t *);
#ifdef NO_FC_SOLVE
extern void dict_free(dict_t *);
extern dict_t *dict_init(dict_t *, dictcount_t, dict_comp_t);
#else
extern dict_t *dict_init(dict_t *, dict_comp_t, fcs_meta_compact_allocator_t *);
#endif
#ifdef NO_FC_SOLVE
extern void dict_init_like(dict_t *, const dict_t *);
extern dict_t *dict_init_alloc(dict_t *, dictcount_t, dict_comp_t,
                               dnode_alloc_t, dnode_free_t, void *);
#endif
#ifdef NO_FC_SOLVE
extern int dict_verify(dict_t *);
extern int dict_similar(const dict_t *, const dict_t *);
extern dnode_t *dict_lower_bound(dict_t *, const void *);
extern dnode_t *dict_upper_bound(dict_t *, const void *);
extern dnode_t *dict_strict_lower_bound(dict_t *, const void *);
extern dnode_t *dict_strict_upper_bound(dict_t *, const void *);
#endif
extern dict_ret_key_t fc_solve_kaz_tree_insert(dict_t *, dnode_t *, dict_key_t);
#ifdef NO_FC_SOLVE
extern dnode_t *fc_solve_kaz_tree_delete(dict_t *, dnode_t *);
#endif
extern const dict_ret_key_t fc_solve_kaz_tree_alloc_insert(dict_t *, dict_key_t);
extern void fc_solve_kaz_tree_delete_free(dict_t *, dnode_t *);
extern dnode_t *fc_solve_kaz_tree_lookup(dict_t *, dict_key_t);
dict_key_t fc_solve_kaz_tree_lookup_value(dict_t *dict, dict_key_t key);

extern dnode_t *fc_solve_kaz_tree_first(dict_t *);
extern dnode_t *fc_solve_kaz_tree_next(dict_t *, dnode_t *);
#ifdef NO_FC_SOLVE
extern dnode_t *dict_last(dict_t *);
extern dnode_t *dict_prev(dict_t *, dnode_t *);
extern dictcount_t dict_count(dict_t *);
extern int dict_isempty(dict_t *);
extern int dict_isfull(dict_t *);
extern int dict_contains(dict_t *, dnode_t *);
#endif
#ifdef NO_FC_SOLVE
extern void dict_allow_dupes(dict_t *);
#endif
#ifdef NO_FC_SOLVE
extern int dnode_is_in_a_dict(dnode_t *);
extern dnode_t *dnode_create(void *);
extern dnode_t *dnode_init(dnode_t *, void *);
extern void dnode_destroy(dnode_t *);
extern void *dnode_get(dnode_t *);
extern dict_key_t *dnode_getkey(dnode_t *);
extern void dnode_put(dnode_t *, void *);
#endif
#ifdef NO_FC_SOLVE
extern void dict_process(dict_t *, void *, dnode_process_t);
extern void dict_load_begin(dict_load_t *, dict_t *);
extern void dict_load_next(dict_load_t *, dnode_t *, const void *);
extern void dict_load_end(dict_load_t *);
extern void dict_merge(dict_t *, dict_t *);
#endif

#if defined(DICT_IMPLEMENTATION) || !defined(KAZLIB_OPAQUE_DEBUG)
#ifdef KAZLIB_SIDEEFFECT_DEBUG
#define dict_isfull(D) (SFX_CHECK(D)->dict_nodecount == (D)->dict_maxcount)
#else
#define dict_isfull(D) ((D)->dict_nodecount == (D)->dict_maxcount)
#endif
#define dict_count(D) ((D)->dict_nodecount)
#define dict_isempty(D) ((D)->dict_nodecount == 0)
#define dnode_get(N) ((N)->dict_data)
#define dnode_getkey(N) ((N)->dict_key)
#define dnode_put(N, X) ((N)->dict_data = (X))
#endif

#ifdef __cplusplus
}

#if 0
#include <functional>

namespace kazlib
{
    class dnode : public dnode_t {
    public:
        dnode() { dnode_init(this, 0); }
        dnode(const dnode &) { dnode_init(this, 0); }
        dnode &operator = (const dnode &) { return *this; }
        bool is_in_a_dict()
        {
            return dnode_is_in_a_dict(this);
        }
    };

    template <typename KEY>
    int default_compare(const KEY &left, const KEY &right)
    {
        if (left < right)
            return -1;
        else if (left == right)
            return 0;
        else
            return 1;
    }

    template <typename KEY, int (*COMP)(const KEY &, const KEY &)>
    struct compare_with_function {
        static int compare(const void *left, const void *right)
        {
            return COMP(*(const KEY *) left, *(const KEY *) right);
        }
    };

    struct dupes_allowed {
        static const int allowed = 1;
    };

    struct dupes_disallowed {
        static const int allowed = 0;
    };

    template <class ITEM, typename KEY, KEY ITEM::* KEY_OFFSET>
    struct key_is_member {
        typedef KEY KEY_TYPE;
        static void *item2key(ITEM *item)
        {
            return &(item->*KEY_OFFSET);
        }
    };

    template <class ITEM>
    struct key_is_base {
        typedef ITEM KEY_TYPE;
        static void *item2key(ITEM *item)
        {
            return item;
        }
    };

    template <class ITEM, dnode ITEM::* DNODE_OFFSET>
    class dnode_is_member {
    private:
        dnode_is_member(const dnode_is_member &);
        void operator = (const dnode_is_member &);
    public:
        typedef ITEM ITEM_TYPE;
        static ITEM *dnode2item(dnode_t *node)
        {
            if (node == 0)
                return 0;
            const ptrdiff_t offset = (char *) &(((ITEM *) 0)->*DNODE_OFFSET)
                                     - ((char *) 0);
            return (ITEM *) (((char *) node) - offset);
        }
        static dnode_t *item2dnode(ITEM *item)
        {
            return &(item->*DNODE_OFFSET);
        }
    };

    template <class ITEM>
    class dnode_is_base {
    private:
        dnode_is_base(const dnode_is_base &);
        void operator = (const dnode_is_base &);
    public:
        typedef ITEM ITEM_TYPE;
        static ITEM *dnode2item(dnode_t *node)
        {
            return static_cast<ITEM *>(node);
        }
        static dnode_t *item2dnode(ITEM *item)
        {
            return item;
        }
    };

    struct static_items {
        template <class CONTAINER>
        static void delete_all(CONTAINER &)
        {
        }
        template <class ITEM>
        static void delete_item(ITEM *)
        {
        }
    };

    struct dynamic_items {
        template <class CONTAINER>
        static void delete_all(CONTAINER &c)
        {
            c.delete_all();
        }
        template <class ITEM>
        static void delete_item(ITEM *item)
        {
            delete item;
        }
    };

    struct placement_items {
        template <class CONTAINER>
        static void delete_all(CONTAINER &c)
        {
            c.delete_all();
        }
        template <class ITEM>
        static void delete_item(ITEM *item)
        {
            item->~ITEM();
        }
    };

    struct dict_dfl_feat {
        typedef dupes_disallowed dupe_feature;
        typedef static_items alloc_feature;
    };

    template <typename FIRST, typename REST>
    struct trait_combinator : public FIRST, public REST {
    };

    template <typename REST, typename KEY, int (*F)(const KEY&, const KEY &)>
    struct trait_combinator<compare_with_function<KEY, F>, REST>
    : public REST {
        typedef compare_with_function<KEY, F> compare_feature;
    };

    template <typename REST>
    struct trait_combinator<dupes_allowed, REST> : public REST {
        typedef dupes_allowed dupe_feature;
    };

    template <typename REST>
    struct trait_combinator<dupes_disallowed, REST> : public REST {
        typedef dupes_disallowed dupe_feature;
    };

    template <typename REST, class ITEM, typename KEY, KEY ITEM::* KO>
    struct trait_combinator<key_is_member<ITEM, KEY, KO>, REST> : public REST {
        typedef key_is_member<ITEM, KEY, KO> key_feature;
        typedef compare_with_function<KEY, default_compare> compare_feature;
    };

    template <typename REST, typename KEY>
    struct trait_combinator<key_is_base<KEY>, REST> : public REST {
        typedef key_is_base<KEY> key_feature;
        typedef compare_with_function<KEY, default_compare> compare_feature;
    };

    template <typename REST, class ITEM, dnode ITEM::* DO>
    struct trait_combinator<dnode_is_member<ITEM, DO>, REST> : public REST {
        typedef dnode_is_member<ITEM, DO> dnode_feature;
    };

    template <typename REST, class ITEM>
    struct trait_combinator<dnode_is_base<ITEM>, REST> : public REST {
        typedef dnode_is_base<ITEM> dnode_feature;
    };

    template <typename REST>
    struct trait_combinator<static_items, REST> : public REST {
        typedef static_items alloc_feature;
    };

    template <typename REST>
    struct trait_combinator<dynamic_items, REST> : public REST {
        typedef dynamic_items alloc_feature;
    };

    template <typename REST>
    struct trait_combinator<placement_items, REST> : public REST {
        typedef placement_items alloc_feature;
    };

    template <typename T1 = dict_dfl_feat,
              typename T2 = dict_dfl_feat,
              typename T3 = dict_dfl_feat,
              typename T4 = dict_dfl_feat,
              typename T5 = dict_dfl_feat>
    struct traits
    : public trait_combinator<T5, traits<T4, T2, T2, T1> >
    {
    };

    template <>
    struct traits<dict_dfl_feat, dict_dfl_feat, dict_dfl_feat,
                  dict_dfl_feat, dict_dfl_feat>
    : public dict_dfl_feat
    {
    };

    template <typename T1>
    struct traits<T1, dict_dfl_feat, dict_dfl_feat,
                  dict_dfl_feat, dict_dfl_feat>
    : public trait_combinator<T1, dict_dfl_feat>
    {
    };

    template <typename T1, typename T2>
    struct traits<T1, T2, dict_dfl_feat, dict_dfl_feat, dict_dfl_feat>
    : public trait_combinator<T2, traits<T1> >
    {
    };

    template <typename T1, typename T2, typename T3>
    struct traits<T1, T2, T3, dict_dfl_feat, dict_dfl_feat>
    : public trait_combinator<T3, traits<T1, T2> >
    {
    };

    template <typename T1, typename T2, typename T3, typename T4>
    struct traits<T1, T2, T3, T4, dict_dfl_feat>
    : public trait_combinator<T4, traits<T1, T2, T3> >
    {
    };

    template <
        typename TRAIT1 = dict_dfl_feat,
        typename TRAIT2 = dict_dfl_feat,
        typename TRAIT3 = dict_dfl_feat,
        typename TRAIT4 = dict_dfl_feat,
        typename TRAIT5 = dict_dfl_feat
    >
    class dict : public dict_t
    {
    private:
        dict(const dict &);
        void operator = (const dict &);
    protected:
        typedef traits<TRAIT1, TRAIT2, TRAIT3, TRAIT4, TRAIT5> tr;
        typedef typename tr::key_feature::KEY_TYPE KEY;
        typedef typename tr::dnode_feature::ITEM_TYPE ITEM;

        static dnode_t *item2dnode(ITEM *item)
        {
            return tr::dnode_feature::item2dnode(item);
        }
        static ITEM *dnode2item(dnode_t *dnode)
        {
            return tr::dnode_feature::dnode2item(dnode);
        }
        static void *item2key(ITEM *item)
        {
            return tr::key_feature::item2key(item);
        }
        static void delete_item(dnode_t *dnode, void *)
        {
            ITEM *item = tr::dnode_feature::dnode2item(dnode);
            tr::alloc_feature::delete_item(item);
        }
    public:
        dict(dictcount_t count = DICTCOUNT_T_MAX)
        {
            dict_init_alloc(this, count, tr::compare_feature::compare, 0,
                            delete_item, 0);
            if (tr::dupe_feature::allowed)
                dict_allow_dupes(this);
        }
        ~dict()
        {
            tr::alloc_feature::delete_all(*this);
        }
        dictcount_t count()
        {
            return dict_count(this);
        }
        ITEM *insert(ITEM *pitem)
        {
            dict_insert(this, item2dnode(pitem), item2key(pitem));
            return pitem;
        }
        ITEM &insert(ITEM &item)
        {
            return *insert(&item);
        }
        ITEM *erase(ITEM *pitem)
        {
            dict_delete(this, item2dnode(pitem));
            return pitem;
        }
        ITEM &erase(ITEM &item)
        {
            return *erase(&item);
        }
        void delete_all()
        {
            dict_free(this);
        }
        ITEM *lookup(const KEY *pkey)
        {
            return dnode2item(dict_lookup(this, pkey));
        }
        ITEM *lookup(const KEY &key)
        {
            return lookup(&key);
        }
        ITEM *upper_bound(const KEY *pkey)
        {
            return dnode2item(dict_upper_bound(this, pkey));
        }
        ITEM *upper_bound(const KEY &key)
        {
            return upper_bound(&key);
        }
        ITEM *lower_bound(const KEY *pkey)
        {
            return dnode2item(dict_lower_bound(this, pkey));
        }
        ITEM *lower_bound(const KEY &key)
        {
            return lower_bound(&key);
        }
        ITEM *first()
        {
            return dnode2item(dict_first(this));
        }
        ITEM *last()
        {
            return dnode2item(dict_last(this));
        }
        ITEM *next(ITEM *pitem)
        {
            return dnode2item(dict_next(this, item2dnode(pitem)));
        }
        ITEM *next(ITEM &item)
        {
            return next(&item);
        }
        ITEM *prev(ITEM *pitem)
        {
            return dnode2item(dict_prev(this, item2dnode(pitem)));
        }
        ITEM *prev(ITEM &item)
        {
            return prev(&item);
        }
    };
}

#endif

#endif

#endif /* FC_SOLVE__KAZ_TREE_H */
