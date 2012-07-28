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

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "avl.h"

#include "inline.h"

static GCC_INLINE signed char avl_get_balance(struct avl_node * node)
{
    return (((signed char)(node->avl_mylink[0] & 0x3))-1);
}

static GCC_INLINE struct avl_node * avl_process_link(uintptr_t mylink)
{
    return (struct avl_node *)(mylink & (~((uintptr_t)0x3)));
}

static GCC_INLINE void avl_set_link(struct avl_node * node, int myindex, struct avl_node * val)
{
    node->avl_mylink[myindex] = (((uintptr_t)val) | (node->avl_mylink[myindex] & 0x3));
}

/*
 * "Be conservative in what you do, be liberal in what you accept from others."
 * -- http://www.joelonsoftware.com/items/2008/03/17.htm
 *
 * Without bounding this, the program fails.
 * */
#define AVL_BOUND(x) (((x) >= 1) ? 1 : ((x) <= (-1)) ? -1 : 0)
static GCC_INLINE void avl_set_balance(struct avl_node * node, signed char balance)
{
    node->avl_mylink[0] &= (~0x3);
    node->avl_mylink[0] |= (((uintptr_t)(AVL_BOUND(balance)+1))&0x3);
}

static GCC_INLINE void avl_increment_balance(struct avl_node * node)
{
    avl_set_balance(node, avl_get_balance(node)+1);
}

static GCC_INLINE void avl_decrement_balance(struct avl_node * node)
{
    avl_set_balance(node, avl_get_balance(node)-1);
}

#define NEXT(p) L((p), 0)
#define SET_NEXT(p, val) SET_L((p), 0, (val))

#define NODEPTR_SET_BALANCE(p, b) avl_set_balance((p), (b))
#define NODEPTR_INC_BALANCE(p) avl_increment_balance(p)
#define NODEPTR_DEC_BALANCE(p) avl_decrement_balance(p)

/* Creates and returns a new table
   with comparison function |compare| using parameter |param|
   and memory allocator |allocator|.
   Returns |NULL| if memory allocation failed. */
struct avl_table *
avl_create (avl_comparison_func *compare, void *param, fcs_meta_compact_allocator_t * meta_alloc)
{
  struct avl_table *tree;

  assert (compare != NULL);

  tree = malloc (sizeof *tree);
  if (tree == NULL)
    return NULL;

  SET_TREE_AVL_ROOT(tree, NULL);
  tree->avl_compare = compare;
  tree->avl_param = param;
  fc_solve_compact_allocator_init(&(tree->avl_allocator), meta_alloc);
  tree->avl_recycle_bin = NULL;
  tree->avl_count = 0;
  tree->avl_generation = 0;

  return tree;
}

/* Search |tree| for an item matching |item|, and return it if found.
   Otherwise return |NULL|. */
void *
avl_find (const struct avl_table *tree, const void *item)
{
  struct avl_node *p;

  assert (tree != NULL && item != NULL);
  for (p = TREE_AVL_ROOT(tree); p != NULL; )
    {
      int cmp = tree->avl_compare (item, NODE_DATA_PTR(p), tree->avl_param);

      if (cmp < 0)
        p = L(p, 0);
      else if (cmp > 0)
        p = L(p, 1);
      else /* |cmp == 0| */
        return NODE_DATA_PTR(p);
    }

  return NULL;
}

/* Inserts |item| into |tree| and returns a pointer to |item|'s address.
   If a duplicate item is found in the tree,
   returns a pointer to the duplicate without inserting |item|.
   Returns |NULL| in case of memory allocation failure. */
avl_key_t*
avl_probe (struct avl_table *tree, void *item)
{
  struct avl_node *y, *z; /* Top node to update balance factor, and parent. */
  struct avl_node *p, *q; /* Iterator, and parent. */
  struct avl_node *n;     /* Newly inserted node. */
  struct avl_node *w;     /* New root of rebalanced subtree. */
  int dir;                /* Direction to descend. */

  unsigned char da[AVL_MAX_HEIGHT]; /* Cached comparison results. */
  int k = 0;              /* Number of cached results. */

  assert (tree != NULL && item != NULL);

  z = (struct avl_node *)&(tree->avl_proto_root);
  y = TREE_AVL_ROOT(tree);
  dir = 0;
  for (q = z, p = y; p != NULL; q = p, p = L(p, dir))
    {
      int cmp = tree->avl_compare (item, NODE_DATA_PTR(p), tree->avl_param);
      if (cmp == 0)
        return &p->avl_data;

      if (NODEPTR_GET_BALANCE(p) != 0)
        z = q, y = p, k = 0;
      da[k++] = dir = cmp > 0;
    }

  if ((n = tree->avl_recycle_bin) != NULL)
  {
      tree->avl_recycle_bin = NEXT(n);
  }
  else
  {
      n = fcs_compact_alloc_ptr(
          &(tree->avl_allocator), sizeof(*n)
          );
  }
  SET_L(q, dir, n);
  if (n == NULL)
    return NULL;

  tree->avl_count++;
  NODE_ASSIGN_DATA_PTR(n, item);
  SET_L(n, 0, NULL);
  SET_L(n, 1, NULL);
  NODEPTR_SET_BALANCE(n, 0);
  if (y == NULL)
    return &n->avl_data;

  for (p = y, k = 0; p != n; p = L(p, da[k]), k++)
    if (da[k] == 0)
      NODEPTR_DEC_BALANCE(p);
    else
      NODEPTR_INC_BALANCE(p);

  if (NODEPTR_GET_BALANCE(y) == -2)
    {
      struct avl_node *x = L(y, 0);
      if (NODEPTR_GET_BALANCE(x) == -1)
        {
          w = x;
          SET_L(y, 0, L(x, 1));
          SET_L(x, 1, y);
          NODEPTR_SET_BALANCE(x, 0);
          NODEPTR_SET_BALANCE(y, 0);
        }
      else
        {
          assert (NODEPTR_GET_BALANCE(x) == +1);
          w = L(x, 1);
          SET_L(x, 1, L(w, 0));
          SET_L(w, 0, x);
          SET_L(y, 0, L(w, 1));
          SET_L(w, 1, y);
          if (NODEPTR_GET_BALANCE(w) == -1)
          {
            NODEPTR_SET_BALANCE(x, 0); NODEPTR_SET_BALANCE(y, +1);
          }
          else if (NODEPTR_GET_BALANCE(w) == 0)
          {
            NODEPTR_SET_BALANCE(x, 0); NODEPTR_SET_BALANCE(y, 0);
          }
          else /* |NODEPTR_GET_BALANCE(w) == +1| */
          {
            NODEPTR_SET_BALANCE(x, -1); NODEPTR_SET_BALANCE(y, 0);
          }
          NODEPTR_SET_BALANCE(w, 0);
        }
    }
  else if (NODEPTR_GET_BALANCE(y) == +2)
    {
      struct avl_node *x = L(y, 1);
      if (NODEPTR_GET_BALANCE(x) == +1)
        {
          w = x;
          SET_L(y, 1, L(x, 0));
          SET_L(x, 0, y);
          NODEPTR_SET_BALANCE(x, 0);
          NODEPTR_SET_BALANCE(y, 0);
        }
      else
        {
          assert (NODEPTR_GET_BALANCE(x) == -1);
          w = L(x, 0);
          SET_L(x, 0, (L(w, 1)));
          SET_L(w, 1, (x));
          SET_L(y, 1, (L(w, 0)));
          SET_L(w, 0, (y));
          if (NODEPTR_GET_BALANCE(w) == +1)
          {
            NODEPTR_SET_BALANCE(x,0);
            NODEPTR_SET_BALANCE(y, -1);
          }
          else if (NODEPTR_GET_BALANCE(w) == 0)
          {
            NODEPTR_SET_BALANCE(x, 0);
            NODEPTR_SET_BALANCE(y, 0);
          }
          else /* |NODEPTR_GET_BALANCE(w) == -1| */
          {
            NODEPTR_SET_BALANCE(x, +1);
            NODEPTR_SET_BALANCE(y, 0);
          }
          NODEPTR_SET_BALANCE(w , 0);
        }
    }
  else
    return &n->avl_data;
  SET_L(z, y != L(z, 0), w);

  tree->avl_generation++;
  return &n->avl_data;
}

/* Inserts |item| into |table|.
   Returns |NULL| if |item| was successfully inserted
   or if a memory allocation error occurred.
   Otherwise, returns the duplicate item. */
void *
avl_insert (struct avl_table *table, void *item)
{
  avl_key_t*p = avl_probe (table, item);
  return p == NULL || AVL_KEY_EQUAL_TO_PTR(*p, item) ? NULL : AVL_KEY_PTR_PTR(p);
}

/* Inserts |item| into |table|, replacing any duplicate item.
   Returns |NULL| if |item| was inserted without replacing a duplicate,
   or if a memory allocation error occurred.
   Otherwise, returns the item that was replaced. */
void *
avl_replace (struct avl_table *table, void *item)
{
  avl_key_t *p = avl_probe (table, item);
  if (p == NULL || AVL_KEY_EQUAL_TO_PTR(p, item))
    return NULL;
  else
    {
      void *r = AVL_KEY_PTR_PTR(p);
      AVL_KEY_ASSIGN_DATA_PTR(p, item);
      return r;
    }
}

/* Deletes from |tree| and returns an item matching |item|.
   Returns a null pointer if no matching item found. */
void *
avl_delete (struct avl_table *tree, const void *item)
{
  /* Stack of nodes. */
  struct avl_node *pa[AVL_MAX_HEIGHT]; /* Nodes. */
  unsigned char da[AVL_MAX_HEIGHT];    /* |avl_link[]| indexes. */
  int k;                               /* Stack pointer. */

  struct avl_node *p;   /* Traverses tree to find node to delete. */
  int cmp;              /* Result of comparison between |item| and |p|. */
  void * ret;

  assert (tree != NULL && item != NULL);

  k = 0;
  p = (struct avl_node *) &tree->avl_proto_root;
  for (cmp = -1; cmp != 0;
       cmp = tree->avl_compare (item, NODE_DATA_PTR(p), tree->avl_param))
    {
      int dir = cmp > 0;

      pa[k] = p;
      da[k++] = dir;

      p = L(p, dir);
      if (p == NULL)
        return NULL;
    }
  ret = NODE_DATA_PTR(p);

  if (L(p, 1) == NULL)
    SET_L(pa[k - 1], da[k - 1], (L(p, 0)));
  else
    {
      struct avl_node *r = L(p, 1);
      if (L(r, 0) == NULL)
        {
          SET_L(r, 0, (L(p, 0)));
          NODEPTR_SET_BALANCE(r, NODEPTR_GET_BALANCE(p));
          SET_L(pa[k - 1], da[k - 1], (r));
          da[k] = 1;
          pa[k++] = r;
        }
      else
        {
          struct avl_node *s;
          int j = k++;

          for (;;)
            {
              da[k] = 0;
              pa[k++] = r;
              s = L(r, 0);
              if (L(s, 0) == NULL)
                break;

              r = s;
            }

          SET_L(s, 0, (L(p, 0)));
          SET_L(r, 0, (L(s, 1)));
          SET_L(s, 1, (L(p, 1)));
          NODEPTR_SET_BALANCE(s, NODEPTR_GET_BALANCE(p));

          SET_L( pa[j - 1], da[j - 1], (s));
          da[j] = 1;
          pa[j] = s;
        }
    }


#if 0
  tree->avl_alloc->libavl_free (tree->avl_alloc, p);
#else
  SET_NEXT(p, tree->avl_recycle_bin);
  tree->avl_recycle_bin = p;
#endif

  assert (k > 0);
  while (--k > 0)
    {
      struct avl_node *y = pa[k];

      if (da[k] == 0)
        {
          NODEPTR_INC_BALANCE(y);
          if (NODEPTR_GET_BALANCE(y) == +1)
            break;
          else if (NODEPTR_GET_BALANCE(y) == +2)
            {
              struct avl_node *x = L(y, 1);
              if (NODEPTR_GET_BALANCE(x) == -1)
                {
                  struct avl_node *w;
                  assert (NODEPTR_GET_BALANCE(x) == -1);
                  w = L(x, 0);
                  SET_L(x, 0, (L(w, 1)));
                  SET_L(w, 1, (x));
                  SET_L(y, 1, (L(w, 0)));
                  SET_L(w, 0, (y));
                  if (NODEPTR_GET_BALANCE(w) == +1)
                  {
                    NODEPTR_SET_BALANCE(x, 0); NODEPTR_SET_BALANCE(y, -1);
                  }
                  else if (NODEPTR_GET_BALANCE(w) == 0)
                  {
                    NODEPTR_SET_BALANCE(x, 0); NODEPTR_SET_BALANCE(y, 0);
                  }
                  else /* |NODEPTR_GET_BALANCE(w) == -1| */
                  {
                    NODEPTR_SET_BALANCE(x, +1); NODEPTR_SET_BALANCE(y, 0);
                  }
                  NODEPTR_SET_BALANCE(w, 0);
                  SET_L(pa[k - 1], da[k - 1], (w));
                }
              else
                {
                  SET_L(y, 1, (L(x, 0)));
                  SET_L(x, 0, (y));
                  SET_L(pa[k - 1], da[k - 1], (x));
                  if (NODEPTR_GET_BALANCE(x) == 0)
                    {
                      NODEPTR_SET_BALANCE(x, -1);
                      NODEPTR_SET_BALANCE(y, +1);
                      break;
                    }
                  else
                  {
                    NODEPTR_SET_BALANCE(x, 0);
                    NODEPTR_SET_BALANCE(y, 0);
                  }
                }
            }
        }
      else
        {
          NODEPTR_DEC_BALANCE(y);
          if (NODEPTR_GET_BALANCE(y) == -1)
            break;
          else if (NODEPTR_GET_BALANCE(y) == -2)
            {
              struct avl_node *x = L(y, 0);
              if (NODEPTR_GET_BALANCE(x) == +1)
                {
                  struct avl_node *w;
                  assert (NODEPTR_GET_BALANCE(x) == +1);
                  w = L(x, 1);
                  SET_L(x, 1, (L(w, 0)));
                  SET_L(w, 0, (x));
                  SET_L(y, 0, (L(w, 1)));
                  SET_L(w, 1, (y));
                  if (NODEPTR_GET_BALANCE(w) == -1)
                  {
                    NODEPTR_SET_BALANCE(x, 0); NODEPTR_SET_BALANCE(y, +1);
                  }
                  else if (NODEPTR_GET_BALANCE(w) == 0)
                  {
                    NODEPTR_SET_BALANCE(x, 0); NODEPTR_SET_BALANCE(y, 0);
                  }
                  else /* |NODEPTR_GET_BALANCE(w) == +1| */
                  {
                    NODEPTR_SET_BALANCE(x, -1);
                    NODEPTR_SET_BALANCE(y, 0);
                  }
                  NODEPTR_SET_BALANCE(w, 0);
                  SET_L(pa[k - 1], da[k - 1], (w));
                }
              else
                {
                  SET_L(y, 0, (L(x, 1)));
                  SET_L(x, 1, (y));
                  SET_L(pa[k - 1], da[k - 1], (x));
                  if (NODEPTR_GET_BALANCE(x) == 0)
                    {
                      NODEPTR_SET_BALANCE(x, +1);
                      NODEPTR_SET_BALANCE(y, -1);
                      break;
                    }
                  else
                  {
                    NODEPTR_SET_BALANCE(x, 0);
                    NODEPTR_SET_BALANCE(y, 0);
                  }
                }
            }
        }
    }

  tree->avl_count--;
  tree->avl_generation++;
  return ret;
}

/* Refreshes the stack of parent pointers in |trav|
   and updates its generation number. */
static void
trav_refresh (struct avl_traverser *trav)
{
  assert (trav != NULL);

  trav->avl_generation = trav->avl_table->avl_generation;

  if (trav->avl_node != NULL)
    {
      avl_comparison_func *cmp = trav->avl_table->avl_compare;
      void *param = trav->avl_table->avl_param;
      struct avl_node *node = trav->avl_node;
      struct avl_node *i;

      trav->avl_height = 0;
      for (i = TREE_AVL_ROOT(trav->avl_table); i != node; )
        {
          assert (trav->avl_height < AVL_MAX_HEIGHT);
          assert (i != NULL);

          trav->avl_stack[trav->avl_height++] = i;
          i = L(i, (cmp (NODE_DATA_PTR(node), NODE_DATA_PTR(i), param) > 0));
        }
    }
}

/* Initializes |trav| for use with |tree|
   and selects the null node. */
void
avl_t_init (struct avl_traverser *trav, struct avl_table *tree)
{
  trav->avl_table = tree;
  trav->avl_node = NULL;
  trav->avl_height = 0;
  trav->avl_generation = tree->avl_generation;
}

/* Initializes |trav| for |tree|
   and selects and returns a pointer to its least-valued item.
   Returns |NULL| if |tree| contains no nodes. */
void *
avl_t_first (struct avl_traverser *trav, struct avl_table *tree)
{
  struct avl_node *x;

  assert (tree != NULL && trav != NULL);

  trav->avl_table = tree;
  trav->avl_height = 0;
  trav->avl_generation = tree->avl_generation;

  x = TREE_AVL_ROOT(tree);
  if (x != NULL)
    while (L(x, 0) != NULL)
      {
        assert (trav->avl_height < AVL_MAX_HEIGHT);
        trav->avl_stack[trav->avl_height++] = x;
        x = L(x, 0);
      }
  trav->avl_node = x;

  return x != NULL ? NODE_DATA_PTR(x) : NULL;
}

/* Initializes |trav| for |tree|
   and selects and returns a pointer to its greatest-valued item.
   Returns |NULL| if |tree| contains no nodes. */
void *
avl_t_last (struct avl_traverser *trav, struct avl_table *tree)
{
  struct avl_node *x;

  assert (tree != NULL && trav != NULL);

  trav->avl_table = tree;
  trav->avl_height = 0;
  trav->avl_generation = tree->avl_generation;

  x = TREE_AVL_ROOT(tree);
  if (x != NULL)
    while (L(x, 1) != NULL)
      {
        assert (trav->avl_height < AVL_MAX_HEIGHT);
        trav->avl_stack[trav->avl_height++] = x;
        x = L(x, 1);
      }
  trav->avl_node = x;

  return x != NULL ? NODE_DATA_PTR(x) : NULL;
}

/* Searches for |item| in |tree|.
   If found, initializes |trav| to the item found and returns the item
   as well.
   If there is no matching item, initializes |trav| to the null item
   and returns |NULL|. */
void *
avl_t_find (struct avl_traverser *trav, struct avl_table *tree, void *item)
{
  struct avl_node *p, *q;

  assert (trav != NULL && tree != NULL && item != NULL);
  trav->avl_table = tree;
  trav->avl_height = 0;
  trav->avl_generation = tree->avl_generation;
  for (p = TREE_AVL_ROOT(tree); p != NULL; p = q)
    {
      int cmp = tree->avl_compare (item, NODE_DATA_PTR(p), tree->avl_param);

      if (cmp < 0)
        q = L(p, 0);
      else if (cmp > 0)
        q = L(p, 1);
      else /* |cmp == 0| */
        {
          trav->avl_node = p;
          return NODE_DATA_PTR(p);
        }

      assert (trav->avl_height < AVL_MAX_HEIGHT);
      trav->avl_stack[trav->avl_height++] = p;
    }

  trav->avl_height = 0;
  trav->avl_node = NULL;
  return NULL;
}

/* Attempts to insert |item| into |tree|.
   If |item| is inserted successfully, it is returned and |trav| is
   initialized to its location.
   If a duplicate is found, it is returned and |trav| is initialized to
   its location.  No replacement of the item occurs.
   If a memory allocation failure occurs, |NULL| is returned and |trav|
   is initialized to the null item. */
void *
avl_t_insert (struct avl_traverser *trav, struct avl_table *tree, void *item)
{
  avl_key_t *p;

  assert (trav != NULL && tree != NULL && item != NULL);

  p = avl_probe (tree, item);
  if (p != NULL)
    {
      trav->avl_table = tree;
      trav->avl_node =
        ((struct avl_node *)
         ((char *) p - offsetof (struct avl_node, avl_data)));
      trav->avl_generation = tree->avl_generation - 1;
      return AVL_KEY_PTR_PTR(p);
    }
  else
    {
      avl_t_init (trav, tree);
      return NULL;
    }
}

/* Initializes |trav| to have the same current node as |src|. */
void *
avl_t_copy (struct avl_traverser *trav, const struct avl_traverser *src)
{
  assert (trav != NULL && src != NULL);

  if (trav != src)
    {
      trav->avl_table = src->avl_table;
      trav->avl_node = src->avl_node;
      trav->avl_generation = src->avl_generation;
      if (trav->avl_generation == trav->avl_table->avl_generation)
        {
          trav->avl_height = src->avl_height;
          memcpy (trav->avl_stack, (const void *) src->avl_stack,
                  sizeof *trav->avl_stack * trav->avl_height);
        }
    }

  return trav->avl_node != NULL ? NODE_DATA_PTR(trav->avl_node) : NULL;
}

/* Returns the next data item in inorder
   within the tree being traversed with |trav|,
   or if there are no more data items returns |NULL|. */
void *
avl_t_next (struct avl_traverser *trav)
{
  struct avl_node *x;

  assert (trav != NULL);

  if (trav->avl_generation != trav->avl_table->avl_generation)
    trav_refresh (trav);

  x = trav->avl_node;
  if (x == NULL)
    {
      return avl_t_first (trav, trav->avl_table);
    }
  else if (L(x, 1) != NULL)
    {
      assert (trav->avl_height < AVL_MAX_HEIGHT);
      trav->avl_stack[trav->avl_height++] = x;
      x = L(x, 1);

      while (L(x, 0) != NULL)
        {
          assert (trav->avl_height < AVL_MAX_HEIGHT);
          trav->avl_stack[trav->avl_height++] = x;
          x = L(x, 0);
        }
    }
  else
    {
      struct avl_node *y;

      do
        {
          if (trav->avl_height == 0)
            {
              trav->avl_node = NULL;
              return NULL;
            }

          y = x;
          x = trav->avl_stack[--trav->avl_height];
        }
      while (y == L(x, 1));
    }
  trav->avl_node = x;

  return NODE_DATA_PTR(x);
}

/* Returns the previous data item in inorder
   within the tree being traversed with |trav|,
   or if there are no more data items returns |NULL|. */
void *
avl_t_prev (struct avl_traverser *trav)
{
  struct avl_node *x;

  assert (trav != NULL);

  if (trav->avl_generation != trav->avl_table->avl_generation)
    trav_refresh (trav);

  x = trav->avl_node;
  if (x == NULL)
    {
      return avl_t_last (trav, trav->avl_table);
    }
  else if (L(x, 0) != NULL)
    {
      assert (trav->avl_height < AVL_MAX_HEIGHT);
      trav->avl_stack[trav->avl_height++] = x;
      x = L(x, 0);

      while (L(x, 1) != NULL)
        {
          assert (trav->avl_height < AVL_MAX_HEIGHT);
          trav->avl_stack[trav->avl_height++] = x;
          x = L(x, 1);
        }
    }
  else
    {
      struct avl_node *y;

      do
        {
          if (trav->avl_height == 0)
            {
              trav->avl_node = NULL;
              return NULL;
            }

          y = x;
          x = trav->avl_stack[--trav->avl_height];
        }
      while (y == L(x, 0));
    }
  trav->avl_node = x;

  return NODE_DATA_PTR(x);
}

/* Returns |trav|'s current item. */
void *
avl_t_cur (struct avl_traverser *trav)
{
  assert (trav != NULL);

  return trav->avl_node != NULL ? NODE_DATA_PTR(trav->avl_node) : NULL;
}

/* Replaces the current item in |trav| by |new| and returns the item replaced.
   |trav| must not have the null item selected.
   The new item must not upset the ordering of the tree. */
void *
avl_t_replace (struct avl_traverser *trav, void *new)
{
  void *old;

  assert (trav != NULL && trav->avl_node != NULL && new != NULL);
  old = NODE_DATA_PTR(trav->avl_node);
  NODE_ASSIGN_DATA_PTR(trav->avl_node, new);
  return old;
}

#if 0
/* Destroys |new| with |avl_destroy (new, destroy)|,
   first setting right links of nodes in |stack| within |new|
   to null pointers to avoid touching uninitialized data. */
static void
copy_error_recovery (struct avl_node **stack, int height,
                     struct avl_table *new, avl_item_func *destroy)
{
  assert (stack != NULL && height >= 0 && new != NULL);

  for (; height > 2; height -= 2)
    stack[height - 1]->avl_link[1] = NULL;
  avl_destroy (new, destroy);
}
#endif

#if 0
/* Copies |org| to a newly created tree, which is returned.
   If |copy != NULL|, each data item in |org| is first passed to |copy|,
   and the return values are inserted into the tree,
   with |NULL| return values taken as indications of failure.
   On failure, destroys the partially created new tree,
   applying |destroy|, if non-null, to each item in the new tree so far,
   and returns |NULL|.
   If |allocator != NULL|, it is used for allocation in the new tree.
   Otherwise, the same allocator used for |org| is used. */
struct avl_table *
avl_copy (const struct avl_table *org, avl_copy_func *copy,
          avl_item_func *destroy)
{
  struct avl_node *stack[2 * (AVL_MAX_HEIGHT + 1)];
  int height = 0;

  struct avl_table *new;
  const struct avl_node *x;
  struct avl_node *y;

  assert (org != NULL);
  new = avl_create (org->avl_compare, org->avl_param);
  if (new == NULL)
    return NULL;
  new->avl_count = org->avl_count;
  if (new->avl_count == 0)
    return new;

  x = (const struct avl_node *) &TREE_AVL_ROOT(org);
  y = (struct avl_node *) &TREE_AVL_ROOT(new);
  for (;;)
    {
      while (L(x, 0) != NULL)
        {
          assert (height < 2 * (AVL_MAX_HEIGHT + 1));

          L(y, 0) =
            new->avl_alloc->libavl_malloc (new->avl_alloc,
                                           sizeof *L(y, 0));
          if (L(y, 0) == NULL)
            {
              if (y != (struct avl_node *) &TREE_AVL_ROOT(new))
                {
                  y->avl_data = NULL;
                  SET_L(y, 1, (NULL));
                }

              copy_error_recovery (stack, height, new, destroy);
              return NULL;
            }

          stack[height++] = (struct avl_node *) x;
          stack[height++] = y;
          x = L(x, 0);
          y = L(y, 0);
        }
      SET_L(y, 0, (NULL));

      for (;;)
        {
          NODEPTR_GET_BALANCE(y) = NODEPTR_GET_BALANCE(x);
          if (copy == NULL)
            y->avl_data = x->avl_data;
          else
            {
              y->avl_data = copy (x->avl_data, org->avl_param);
              if (y->avl_data == NULL)
                {
                  SET_L(y, 1, (NULL));
                  copy_error_recovery (stack, height, new, destroy);
                  return NULL;
                }
            }

          if (L(x, 1) != NULL)
            {
              L(y, 1) =
                new->avl_alloc->libavl_malloc (new->avl_alloc,
                                               sizeof *L(y, 1));
              if (L(y, 1) == NULL)
                {
                  copy_error_recovery (stack, height, new, destroy);
                  return NULL;
                }

              x = L(x, 1);
              y = L(y, 1);
              break;
            }
          else
            SET_L(y, 1, (NULL));

          if (height <= 2)
            return new;

          y = stack[--height];
          x = stack[--height];
        }
    }
}
#endif

/* Frees storage allocated for |tree|.
   If |destroy != NULL|, applies it to each data item in inorder. */
void
avl_destroy (struct avl_table *tree, avl_item_func *destroy)
{
  struct avl_node *p, *q;

  assert (tree != NULL);

  if (destroy != NULL)
  {
      for (p = TREE_AVL_ROOT(tree); p != NULL; p = q)
          if (L(p, 0) == NULL)
          {
              q = L(p, 1);
#ifndef FCS_LIBAVL_STORE_WHOLE_KEYS
              if (p->avl_data != NULL)
                  destroy (p->avl_data, tree->avl_param);
#endif
#if 0
              tree->avl_alloc->libavl_free (tree->avl_alloc, p);
#endif
          }
          else
          {
              q = L(p, 0);
              SET_L(p, 0, (L(q, 1)));
              SET_L(q, 1, (p));
          }
  }

  fc_solve_compact_allocator_finish(&(tree->avl_allocator));

  free(tree);
}

#if 0
/* Allocates |size| bytes of space using |malloc()|.
   Returns a null pointer if allocation fails. */
void *
avl_malloc (struct libavl_allocator *allocator, size_t size)
{
  assert (allocator != NULL && size > 0);
  return malloc (size);
}
#endif

#if 0
/* Frees |block|. */
void
avl_free (struct libavl_allocator *allocator, void *block)
{
  assert (allocator != NULL && block != NULL);
  free (block);
}

/* Default memory allocator that uses |malloc()| and |free()|. */
struct libavl_allocator avl_allocator_default =
  {
    avl_malloc,
    avl_free
  };
#endif

#undef NDEBUG
#include <assert.h>

/* Asserts that |avl_insert()| succeeds at inserting |item| into |table|. */
void
(avl_assert_insert) (struct avl_table *table, void *item)
{
  avl_key_t*p = avl_probe (table, item);
  assert (p != NULL && AVL_KEY_EQUAL_TO_PTR(*p, item));
}

/* Asserts that |avl_delete()| really removes |item| from |table|,
   and returns the removed item. */
void *
(avl_assert_delete) (struct avl_table *table, void *item)
{
  void *p = avl_delete (table, item);
  assert (p != NULL);
  return p;
}

