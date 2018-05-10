/* Produced by texiweb from libavl.w. */

/* libavl - library for manipulation of binary trees.
   Copyright (C) 1998, 1999, 2000, 2001, 2002, 2004 Free Software
   Foundation, Inc.

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 3 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with this library; if not, write to the Free Software
   Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
   02110-1301 USA.
*/
#pragma once
#include <stddef.h>
#include <stdint.h>

#include "meta_alloc.h"

#ifdef FCS_LIBAVL_STORE_WHOLE_KEYS
#include "delta_states.h"
#endif

/* Function types. */
typedef int rb_comparison_func (const void *rb_a, const void *rb_b,
                                 void *rb_param);
typedef void rb_item_func (void *rb_item, void *rb_param);
typedef void *rb_copy_func (void *rb_item, void *rb_param);

#if 0
#ifndef LIBAVL_ALLOCATOR
#define LIBAVL_ALLOCATOR
/* Memory allocator. */
struct libavl_allocator
  {
    void *(*libavl_malloc) (struct libavl_allocator *, size_t libavl_size);
    void (*libavl_free) (struct libavl_allocator *, void *libavl_block);
  };
#endif

/* Default memory allocator. */
extern struct libavl_allocator rb_allocator_default;
void *rb_malloc (struct libavl_allocator *, size_t);
void rb_free (struct libavl_allocator *, void *);
#endif

/* Maximum RB height. */
#ifndef RB_MAX_HEIGHT
#define RB_MAX_HEIGHT 128
#endif

#ifdef FCS_LIBAVL_STORE_WHOLE_KEYS
typedef fcs_dbm_record avl_key_type;
#define AVL_KEY_PTR_PTR(p) (p)
#define NODE_DATA_PTR(p) (&((p)->rb_data))
#define NODE_ASSIGN_DATA_PTR(node_p, ptr) (((node_p)->rb_data) = *(fcs_dbm_record *)ptr)
#define AVL_KEY_ASSIGN_DATA_PTR(p, ptr) ((*(p)) = *(fcs_dbm_record *)ptr)
#define AVL_KEY_EQUAL_TO_PTR(key, ptr) (!memcmp(&(key), (ptr), sizeof(key)))
#else
typedef void * avl_key_type;
#define AVL_KEY_PTR_PTR(p) (*(p))
#define NODE_DATA_PTR(p) ((p)->rb_data)
#define NODE_ASSIGN_DATA_PTR(node_p, ptr) (((node_p)->rb_data) = ptr)
#define AVL_KEY_ASSIGN_DATA_PTR(p, ptr) ((*(p)) = ptr)
#define AVL_KEY_EQUAL_TO_PTR(key, ptr) ((key) == (ptr))
#endif

#define AVL_NEXT(p) (*((struct rb_node * *)(NODE_DATA_PTR(p))))
#define AVL_SET_NEXT(p, val) (AVL_NEXT(p) = (val))
/* Tree data structure. */
#define rb_root rb_proto_root.rb_mylink[0]
#define TREE_AVL_ROOT(tree) (rb_process_link((tree)->rb_root))

/* An RB tree node. */
struct rb_node
  {
    avl_key_type rb_data;                /* Pointer to data. */
    uintptr_t rb_mylink[2];  /* Subtrees. */
#ifdef WITH_AVL_BALANCE_FIELD
    unsigned char rb_color;       /* Balance factor. */
#endif
  };

struct rb_table
  {
    struct rb_node rb_proto_root;          /* Tree's root. */
    rb_comparison_func *rb_compare;   /* Comparison function. */
    void *rb_param;                    /* Extra argument to |rb_compare|. */
    compact_allocator dict_allocator;
    struct rb_node * * rb_recycle_bin;
    size_t rb_count;                   /* Number of items in tree. */
    unsigned long rb_generation;       /* Generation number. */
  };

/* Color of a red-black node. */
enum rb_color
  {
    RB_BLACK,   /* Black. */
    RB_RED      /* Red. */
  };

/* RB traverser structure. */
struct rb_traverser
  {
    struct rb_table *rb_table;        /* Tree being traversed. */
    struct rb_node *rb_node;          /* Current node in tree. */
    struct rb_node *rb_stack[RB_MAX_HEIGHT];
                                        /* All the nodes above |rb_node|. */
    size_t rb_height;                  /* Number of nodes in |rb_parent|. */
    unsigned long rb_generation;       /* Generation number. */
  };

/* Table functions. */
struct rb_table *rb_create (rb_comparison_func *, void *,
                              meta_allocator *, void * * common_recycle_bin);
struct rb_table *rb_copy (const struct rb_table *, rb_copy_func *,
                            rb_item_func *);
void rb_destroy (struct rb_table *, rb_item_func *);
avl_key_type *rb_probe (struct rb_table *, void *);
void *rb_insert (struct rb_table *, void *);
void *rb_replace (struct rb_table *, void *);
void *rb_delete (struct rb_table *, const void *);
void *rb_find (const struct rb_table *, const void *);
void rb_assert_insert (struct rb_table *, void *);
void *rb_assert_delete (struct rb_table *, void *);

#define rb_count(table) ((size_t) (table)->rb_count)

/* Table traverser functions. */
void rb_t_init (struct rb_traverser *, struct rb_table *);
void *rb_t_first (struct rb_traverser *, struct rb_table *);
void *rb_t_last (struct rb_traverser *, struct rb_table *);
void *rb_t_find (struct rb_traverser *, struct rb_table *, void *);
void *rb_t_insert (struct rb_traverser *, struct rb_table *, void *);
void *rb_t_copy (struct rb_traverser *, const struct rb_traverser *);
void *rb_t_next (struct rb_traverser *);
void *rb_t_prev (struct rb_traverser *);
void *rb_t_cur (struct rb_traverser *);
void *rb_t_replace (struct rb_traverser *, void *);

static inline int rb_get_decommissioned_flag(struct rb_node *const node)
{
    return ((int)(node->rb_mylink[1] & 0x1));
}

static inline void rb_set_decommissioned_flag(struct rb_node *const node, const int decommissioned_flag)
{
    node->rb_mylink[1] &= (~0x1UL);
    node->rb_mylink[1] |= (decommissioned_flag ? 0x1UL : 0x0UL);
}
