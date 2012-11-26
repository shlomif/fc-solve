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
/* Removes self-checks. See the kazlib's README for more information
 * about the motivation for this. Without it, everything is much slower.
 *
 * Also see "man assert".
 * */
#ifndef NDEBUG
#define NDEBUG
#endif

#include <stdlib.h>
#include <stddef.h>
#include <assert.h>

#include "alloc_wrap.h"
#include "inline.h"

#define DICT_IMPLEMENTATION
#include "kaz_tree.h"


/*
 * These macros provide short convenient names for structure members,
 * which are embellished with dict_ prefixes so that they are
 * properly confined to the documented namespace. It's legal for a
 * program which uses dict to define, for instance, a macro called ``parent''.
 * Such a macro would interfere with the dnode_t struct definition.
 * In general, highly portable and reusable C modules which expose their
 * structures need to confine structure member names to well-defined spaces.
 * The resulting identifiers aren't necessarily convenient to use, nor
 * readable, in the implementation, however!
 */

#define left dict_left
#define right dict_right
#define parent dict_parent
#define color dict_color
#define data dict_data

#define nilnode dict_nilnode
#define nodecount dict_nodecount
#define maxcount dict_maxcount
#define compare dict_compare
#define allocnode dict_allocnode
#define freenode dict_freenode
#define context dict_context
#define dupes dict_dupes

#define dictptr dict_dictptr

#define dict_root(D) ((D)->nilnode.left)
#define dict_nil(D) (&(D)->nilnode)
#define DICT_DEPTH_MAX 64

#ifdef NO_FC_SOLVE
static dnode_t *dnode_alloc(void *context);
static void dnode_free(dnode_t *node, void *context);
#endif

/*
 * Perform a ``left rotation'' adjustment on the tree.  The given node P and
 * its right child C are rearranged so that the P instead becomes the left
 * child of C.   The left subtree of C is inherited as the new right subtree
 * for P.  The ordering of the keys within the tree is thus preserved.
 */

static void rotate_left(dnode_t *upper)
{
    dnode_t *lower, *lowleft, *upparent;

    lower = upper->right;
    upper->right = lowleft = lower->left;
    lowleft->parent = upper;

    lower->parent = upparent = upper->parent;

    /* don't need to check for root node here because root->parent is
       the sentinel nil node, and root->parent->left points back to root */

    if (upper == upparent->left) {
        upparent->left = lower;
    } else {
        assert (upper == upparent->right);
        upparent->right = lower;
    }

    lower->left = upper;
    upper->parent = lower;
}

/*
 * This operation is the ``mirror'' image of rotate_left. It is
 * the same procedure, but with left and right interchanged.
 */

static void rotate_right(dnode_t *upper)
{
    dnode_t *lower, *lowright, *upparent;

    lower = upper->left;
    upper->left = lowright = lower->right;
    lowright->parent = upper;

    lower->parent = upparent = upper->parent;

    if (upper == upparent->right) {
        upparent->right = lower;
    } else {
        assert (upper == upparent->left);
        upparent->left = lower;
    }

    lower->right = upper;
    upper->parent = lower;
}

#if 0
/*
 * Do a postorder traversal of the tree rooted at the specified
 * node and free everything under it.  Used by dict_free().
 */

static void free_nodes(dict_t *dict, dnode_t *node, dnode_t *nil)
{
    if (node == nil)
        return;
    free_nodes(dict, node->left, nil);
    free_nodes(dict, node->right, nil);
    dict->freenode(node, dict->context);
}
#endif

#ifdef NO_FC_SOLVE
/*
 * This procedure performs a verification that the given subtree is a binary
 * search tree. It performs an inorder traversal of the tree using the
 * dict_next() successor function, verifying that the key of each node is
 * strictly lower than that of its successor, if duplicates are not allowed,
 * or lower or equal if duplicates are allowed.  This function is used for
 * debugging purposes.
 */

static int verify_bintree(dict_t *dict)
{
    dnode_t *first, *next;

    first = fc_solve_kaz_tree_first(dict);

    if (dict->dupes) {
        while (first && (next = dict_next(dict, first))) {
            if (dict->compare(first->key, next->key, dict->context) > 0)
                return 0;
            first = next;
        }
    } else {
        while (first && (next = dict_next(dict, first))) {
            if (dict->compare(first->key, next->key, dict->context) >= 0)
                return 0;
            first = next;
        }
    }
    return 1;
}

#endif

#ifdef NO_FC_SOLVE
/*
 * This function recursively verifies that the given binary subtree satisfies
 * three of the red black properties. It checks that every red node has only
 * black children. It makes sure that each node is either red or black. And it
 * checks that every path has the same count of black nodes from root to leaf.
 * It returns the blackheight of the given subtree; this allows blackheights to
 * be computed recursively and compared for left and right siblings for
 * mismatches. It does not check for every nil node being black, because there
 * is only one sentinel nil node. The return value of this function is the
 * black height of the subtree rooted at the node ``root'', or zero if the
 * subtree is not red-black.
 */

static unsigned int verify_redblack(dnode_t *nil, dnode_t *root)
{
    unsigned height_left, height_right;

    if (root != nil) {
        height_left = verify_redblack(nil, root->left);
        height_right = verify_redblack(nil, root->right);
        if (height_left == 0 || height_right == 0)
            return 0;
        if (height_left != height_right)
            return 0;
        if (root->color == dnode_red) {
            if (root->left->color != dnode_black)
                return 0;
            if (root->right->color != dnode_black)
                return 0;
            return height_left;
        }
        if (root->color != dnode_black)
            return 0;
        return height_left + 1;
    }
    return 1;
}
#endif

#ifdef NO_FC_SOLVE
/*
 * Compute the actual count of nodes by traversing the tree and
 * return it. This could be compared against the stored count to
 * detect a mismatch.
 */

static dictcount_t verify_node_count(dnode_t *nil, dnode_t *root)
{
    if (root == nil)
        return 0;
    else
        return 1 + verify_node_count(nil, root->left)
            + verify_node_count(nil, root->right);
}
#endif

#ifdef NO_FC_SOLVE
/*
 * Verify that the tree contains the given node. This is done by
 * traversing all of the nodes and comparing their pointers to the
 * given pointer. Returns 1 if the node is found, otherwise
 * returns zero. It is intended for debugging purposes.
 */

static int verify_dict_has_node(dnode_t *nil, dnode_t *root, dnode_t *node)
{
    if (root != nil) {
        return root == node
                || verify_dict_has_node(nil, root->left, node)
                || verify_dict_has_node(nil, root->right, node);
    }
    return 0;
}
#endif

/*
 * Dynamically allocate and initialize a dictionary object.
 */

#ifdef NO_FC_SOLVE
dict_t *dict_create(dictcount_t maxcount, dict_comp_t comp, void * context)
#else
dict_t *fc_solve_kaz_tree_create(dict_comp_t comp, void * context, fcs_meta_compact_allocator_t * meta_allocator)
#endif
{
    dict_t *dict = (dict_t *) SMALLOC1(dict);

    if (dict)
        dict_init(dict, comp, meta_allocator);

    dict->context = context;

    return dict;
}

#if 0
/*
 * Select a different set of node allocator routines.
 */

void dict_set_allocator(dict_t *dict, dnode_alloc_t al,
        dnode_free_t fr, void *context)
{
    assert (dict_count(dict) == 0);
    assert ((al == NULL && fr == NULL) || (al != NULL && fr != NULL));

    dict->allocnode = al ? al : dnode_alloc;
    dict->freenode = fr ? fr : dnode_free;
    dict->context = context;
}
#endif

#ifdef NO_FC_SOLVE
/*
 * Iterative bottom-up (postorder) traversal utility,
 * which allows the callback to destroy the node.
 */
static void safe_traverse(dict_t *dict, void (*func)(dnode_t *, void *))
{
    dnode_t *nil = dict_nil(dict), *current = dict_root(dict);
    enum { from_parent, from_left, from_right } came_from = from_parent;

    while (current != nil) {
        dnode_t *next = nil; /* Initialized to shut up gcc warning. */

        switch (came_from) {
        case from_parent:
            if (current->left != nil) {
                next = current->left;
            } else
        case from_left:
            if (current->right != nil) {
                came_from = from_parent;
                next = current->right;
            } else
        case from_right:
            {
                came_from = (current == current->parent->left)
                            ? from_left : from_right;
                next = current->parent;
                func(current, dict->context);
            }
            break;
        }

        current = next;
    }
}
#endif

/*
 * Free a dynamically allocated dictionary object. Removing the nodes
 * from the tree before deleting it is required.
 */
void fc_solve_kaz_tree_destroy(dict_t *dict)
{
    assert (dict_isempty(dict));

    fc_solve_compact_allocator_finish(&(dict->dict_allocator));
    free(dict);
}

/*
 * Free all the nodes in the dictionary by using the dictionary's
 * installed free routine. The dictionary is emptied.
 */

void fc_solve_kaz_tree_free_nodes(dict_t *dict)
{
    /* Removed for fc-solve. */
#ifdef NO_FC_SOLVE
    safe_traverse(dict, dict->freenode);
    dict->nodecount = 0;
#endif
    dict->nilnode.left = &dict->nilnode;
    dict->nilnode.right = &dict->nilnode;
}

#ifdef NO_FC_SOLVE
/*
 * Obsolescent function, equivalent to dict_free_nodes
 */

void dict_free(dict_t *dict)
{
#ifdef KAZLIB_OBSOLESCENT_DEBUG
    assert ("call to obsolescent function dict_free()" && 0);
#endif
    fc_solve_kaz_tree_free_nodes(dict);
}
#endif

/*
 * Initialize a user-supplied dictionary object.
 */

#ifdef NO_FC_SOLVE
dict_t *dict_init(dict_t *dict, dictcount_t maxcount, dict_comp_t comp)
#else
dict_t *dict_init(dict_t *dict, dict_comp_t comp,
    fcs_meta_compact_allocator_t * meta_allocator
    )
#endif
{
    dict->compare = comp;
    /* Removed for fc-solve. */
#if 0
    dict->allocnode = dnode_alloc;
    dict->freenode = dnode_free;
#else
    fc_solve_compact_allocator_init(&(dict->dict_allocator), meta_allocator);
    dict->dict_recycle_bin = NULL;
#endif

    dict->context = NULL;
#ifdef NO_FC_SOLVE
    dict->nodecount = 0;
    dict->maxcount = maxcount;
#endif
    dict->nilnode.left = &dict->nilnode;
    dict->nilnode.right = &dict->nilnode;
    dict->nilnode.parent = &dict->nilnode;
    dict->nilnode.color = dnode_black;
#ifdef NO_FC_SOLVE
    dict->dupes = 0;
#endif
    return dict;
}

#ifdef NO_FC_SOLVE
/*
 * Initialize a dictionary in the likeness of another dictionary
 */

void dict_init_like(dict_t *dict, const dict_t *orig)
{
    dict->compare = orig->compare;
    /* Removed for fc-solve. */
#if 0
    dict->allocnode = orig->allocnode;
    dict->freenode = orig->freenode;
#else
#endif
    dict->context = orig->context;
#ifdef NO_FC_SOLVE
    dict->nodecount = 0;
    dict->maxcount = orig->maxcount;
#endif
    dict->nilnode.left = &dict->nilnode;
    dict->nilnode.right = &dict->nilnode;
    dict->nilnode.parent = &dict->nilnode;
    dict->nilnode.color = dnode_black;
#ifdef NO_FC_SOLVE
    dict->dupes = orig->dupes;
#endif

    assert (dict_similar(dict, orig));
}

/*
 * Initialize with allocator
 */
extern dict_t *dict_init_alloc(dict_t *dict, dictcount_t maxcount,
                               dict_comp_t comp, dnode_alloc_t al,
                               dnode_free_t fr, void *context)
{
    dict->compare = comp;
    /* Removed for fc-solve. */
#if 0
    dict->allocnode = al;
    dict->freenode = fr;
#endif
    dict->context = context;
    dict->nodecount = 0;
    dict->maxcount = maxcount;
    dict->nilnode.left = &dict->nilnode;
    dict->nilnode.right = &dict->nilnode;
    dict->nilnode.parent = &dict->nilnode;
    dict->nilnode.color = dnode_black;
#ifdef NO_FC_SOLVE
    dict->dupes = 0;
#endif
    return dict;
}

/*
 * Remove all nodes from the dictionary (without freeing them in any way).
 */

static void dict_clear(dict_t *dict)
{
#ifdef NO_FC_SOLVE
    dict->nodecount = 0;
#endif
    dict->nilnode.left = &dict->nilnode;
    dict->nilnode.right = &dict->nilnode;
    dict->nilnode.parent = &dict->nilnode;
    assert (dict->nilnode.color == dnode_black);
}



/*
 * Verify the integrity of the dictionary structure.  This is provided for
 * debugging purposes, and should be placed in assert statements.   Just because
 * this function succeeds doesn't mean that the tree is not corrupt. Certain
 * corruptions in the tree may simply cause undefined behavior.
 */

int dict_verify(dict_t *dict)
{
    dnode_t *nil = dict_nil(dict), *root = dict_root(dict);

    /* check that the sentinel node and root node are black */
    if (root->color != dnode_black)
        return 0;
    if (nil->color != dnode_black)
        return 0;
    if (nil->right != nil)
        return 0;
    /* nil->left is the root node; check that its parent pointer is nil */
    if (nil->left->parent != nil)
        return 0;
    /* perform a weak test that the tree is a binary search tree */
    if (!verify_bintree(dict))
        return 0;
    /* verify that the tree is a red-black tree */
    if (!verify_redblack(nil, root))
        return 0;
    if (verify_node_count(nil, root) != dict_count(dict))
        return 0;
    return 1;
}

/*
 * Determine whether two dictionaries are similar: have the same comparison and
 * allocator functions, and same status as to whether duplicates are allowed.
 */

int dict_similar(const dict_t *left, const dict_t *right)
{
    if (left->compare != right->compare)
        return 0;

    if (left->allocnode != right->allocnode)
        return 0;

    if (left->freenode != right->freenode)
        return 0;

    if (left->context != right->context)
        return 0;

    if (left->dupes != right->dupes)
        return 0;

    return 1;
}
#endif

dict_key_t fc_solve_kaz_tree_lookup_value(dict_t *dict, dict_key_t key)
{
    dnode_t *dn;

    dn = fc_solve_kaz_tree_lookup(dict, key);

    return (dn ? dn->dict_key : NULL);
}

/*
 * Locate a node in the dictionary having the given key.
 * If the node is not found, a null a pointer is returned (rather than
 * a pointer that dictionary's nil sentinel node), otherwise a pointer to the
 * located node is returned.
 */

dnode_t *fc_solve_kaz_tree_lookup(dict_t *dict, dict_key_t key)
{
    dnode_t *root = dict_root(dict);
    dnode_t *nil = dict_nil(dict);
#ifdef NO_FC_SOLVE
    dnode_t *saved;
#endif
    int result;

    /* simple binary search adapted for trees that contain duplicate keys */

    while (root != nil) {
#ifdef FCS_KAZ_TREE_USE_RECORD_DICT_KEY
        result = memcmp(&(key.key), &(root->dict_key.key), sizeof(key.key));
#else
        result = dict->compare(key, root->dict_key, dict->context);
#endif

        if (result < 0)
            root = root->left;
        else if (result > 0)
            root = root->right;
        else {
#ifdef NO_FC_SOLVE
            if (!dict->dupes) { /* no duplicates, return match          */
                return root;
            } else {            /* could be dupes, find leftmost one    */
                do {
                    saved = root;
                    root = root->left;
                    while (root != nil && dict->compare(key, root->key, dict->context))
                        root = root->right;
                } while (root != nil);
                return saved;
            }
#else
            return root;
#endif
        }
    }

    return NULL;
}

#ifdef NO_FC_SOLVE
/*
 * Look for the node corresponding to the lowest key that is equal to or
 * greater than the given key.  If there is no such node, return null.
 */

dnode_t *dict_lower_bound(dict_t *dict, const void *key)
{
    dnode_t *root = dict_root(dict);
    dnode_t *nil = dict_nil(dict);
    dnode_t *tentative = 0;

    while (root != nil) {
        int result = dict->compare(key, root->key, dict->context);

        if (result > 0) {
            root = root->right;
        } else if (result < 0) {
            tentative = root;
            root = root->left;
        } else {
#ifdef NO_FC_SOLVE
            if (!dict->dupes) {
                return root;
            } else {
                tentative = root;
                root = root->left;
            }
#else
            return root;
#endif
        }
    }

    return tentative;
}

/*
 * Look for the node corresponding to the greatest key that is equal to or
 * lower than the given key.  If there is no such node, return null.
 */

dnode_t *dict_upper_bound(dict_t *dict, const void *key)
{
    dnode_t *root = dict_root(dict);
    dnode_t *nil = dict_nil(dict);
    dnode_t *tentative = 0;

    while (root != nil) {
        int result = dict->compare(key, root->key, dict->context);

        if (result < 0) {
            root = root->left;
        } else if (result > 0) {
            tentative = root;
            root = root->right;
        } else {
#ifdef NO_FC_SOLVE
            if (!dict->dupes) {
                return root;
            } else {
                tentative = root;
                root = root->right;
            }
#else
            return root;
#endif
        }
    }

    return tentative;
}

/*
 * Look for the node corresponding to the lowest key that
 * is greater than the given key.
 */
dnode_t *dict_strict_lower_bound(dict_t *dict, const void *key)
{
    dnode_t *root = dict_root(dict);
    dnode_t *nil = dict_nil(dict);
    dnode_t *tentative = 0;

    while (root != nil) {
        int result = dict->compare(key, root->key, dict->context);

        if (result >= 0) {
            root = root->right;
        } else {
            tentative = root;
            root = root->left;
        }
    }

    return tentative;
}

/*
 * Look for the node corresponding to the grestest key that
 * is lower than the given key.
 */
dnode_t *dict_strict_upper_bound(dict_t *dict, const void *key)
{
    dnode_t *root = dict_root(dict);
    dnode_t *nil = dict_nil(dict);
    dnode_t *tentative = 0;

    while (root != nil) {
        int result = dict->compare(key, root->key, dict->context);

        if (result <= 0) {
            root = root->left;
        } else {
            tentative = root;
            root = root->right;
        }
    }

    return tentative;
}

#endif

/*
 * Insert a node into the dictionary. The node should have been
 * initialized with a data field. All other fields are ignored.
 *
 * If a matching key was found - return it. Else - return NULL to indicate
 * that the new key was added.
 *
 * The behavior is undefined if the user attempts to insert into
 * a dictionary that is already full (for which the dict_isfull()
 * function returns true).
 */

dict_ret_key_t fc_solve_kaz_tree_insert(dict_t *dict, dnode_t *node, dict_key_t key)
{
    dnode_t *where = dict_root(dict), *nil = dict_nil(dict);
    dnode_t *parent = nil, *uncle, *grandpa;
    int result = -1;

    node->dict_key = key;

    assert (!dict_isfull(dict));
    assert (!dict_contains(dict, node));
    assert (!dnode_is_in_a_dict(node));

    /* basic binary tree insert */

    while (where != nil) {
        parent = where;
#ifdef FCS_KAZ_TREE_USE_RECORD_DICT_KEY
        result = memcmp(&(key.key), &(where->dict_key.key), sizeof(key.key));
#else
        result = dict->compare(key, where->dict_key, dict->context);
#endif

        /* We are remming it out because instead of duplicating the key
         * we return the existing key. -- Shlomi Fish, fc-solve.
         * */
#if 0
        /* trap attempts at duplicate key insertion unless it's explicitly allowed */
        assert (dict->dupes || result != 0);
#endif
        if (result == 0)
        {
#ifdef FCS_KAZ_TREE_USE_RECORD_DICT_KEY
            return &(where->dict_key);
#else
            return where->dict_key;
#endif
        }
        else if (result < 0)
        {
            where = where->left;
        }
        else
        {
            where = where->right;
        }
    }

    assert (where == nil);

    if (result < 0)
        parent->left = node;
    else
        parent->right = node;

    node->parent = parent;
    node->left = nil;
    node->right = nil;

#ifdef NO_FC_SOLVE
    dict->nodecount++;
#endif

    /* red black adjustments */

    node->color = dnode_red;

    while (parent->color == dnode_red) {
        grandpa = parent->parent;
        if (parent == grandpa->left) {
            uncle = grandpa->right;
            if (uncle->color == dnode_red) {    /* red parent, red uncle */
                parent->color = dnode_black;
                uncle->color = dnode_black;
                grandpa->color = dnode_red;
                node = grandpa;
                parent = grandpa->parent;
            } else {                            /* red parent, black uncle */
                if (node == parent->right) {
                    rotate_left(parent);
                    parent = node;
                    assert (grandpa == parent->parent);
                    /* rotation between parent and child preserves grandpa */
                }
                parent->color = dnode_black;
                grandpa->color = dnode_red;
                rotate_right(grandpa);
                break;
            }
        } else {        /* symmetric cases: parent == parent->parent->right */
            uncle = grandpa->left;
            if (uncle->color == dnode_red) {
                parent->color = dnode_black;
                uncle->color = dnode_black;
                grandpa->color = dnode_red;
                node = grandpa;
                parent = grandpa->parent;
            } else {
                if (node == parent->left) {
                    rotate_right(parent);
                    parent = node;
                    assert (grandpa == parent->parent);
                }
                parent->color = dnode_black;
                grandpa->color = dnode_red;
                rotate_left(grandpa);
                break;
            }
        }
    }

    dict_root(dict)->color = dnode_black;

    assert (dict_verify(dict));

    return NULL;
}

/*
 * Delete the given node from the dictionary. If the given node does not belong
 * to the given dictionary, undefined behavior results.  A pointer to the
 * deleted node is returned.
 */

dnode_t *fc_solve_kaz_tree_delete(dict_t *dict, dnode_t *target)
{
    dnode_t *nil = dict_nil(dict), *child, *delparent = target->parent;

    /* basic deletion */

    assert (!dict_isempty(dict));
    assert (dict_contains(dict, target));

    /*
     * If the node being deleted has two children, then we replace it with its
     * successor (i.e. the leftmost node in the right subtree.) By doing this,
     * we avoid the traditional algorithm under which the successor's key and
     * value *only* move to the deleted node and the successor is spliced out
     * from the tree. We cannot use this approach because the user may hold
     * pointers to the successor, or nodes may be inextricably tied to some
     * other structures by way of embedding, etc. So we must splice out the
     * node we are given, not some other node, and must not move contents from
     * one node to another behind the user's back.
     */

    if (target->left != nil && target->right != nil) {
        dnode_t *next = fc_solve_kaz_tree_next(dict, target);
        dnode_t *nextparent = next->parent;
        dnode_color_t nextcolor = next->color;

        assert (next != nil);
        assert (next->parent != nil);
        assert (next->left == nil);

        /*
         * First, splice out the successor from the tree completely, by
         * moving up its right child into its place.
         */

        child = next->right;
        child->parent = nextparent;

        if (nextparent->left == next) {
            nextparent->left = child;
        } else {
            assert (nextparent->right == next);
            nextparent->right = child;
        }

        /*
         * Now that the successor has been extricated from the tree, install it
         * in place of the node that we want deleted.
         */

        next->parent = delparent;
        next->left = target->left;
        next->right = target->right;
        next->left->parent = next;
        next->right->parent = next;
        next->color = target->color;
        target->color = nextcolor;

        if (delparent->left == target) {
            delparent->left = next;
        } else {
            assert (delparent->right == target);
            delparent->right = next;
        }

    } else {
        assert (target != nil);
        assert (target->left == nil || target->right == nil);

        child = (target->left != nil) ? target->left : target->right;

        child->parent = delparent = target->parent;

        if (target == delparent->left) {
            delparent->left = child;
        } else {
            assert (target == delparent->right);
            delparent->right = child;
        }
    }

    target->parent = NULL;
    target->right = NULL;
    target->left = NULL;

#ifdef NO_FC_SOLVE
    dict->nodecount--;
#endif

    assert (verify_bintree(dict));

    /* red-black adjustments */

    if (target->color == dnode_black) {
        dnode_t *parent, *sister;

        dict_root(dict)->color = dnode_red;

        while (child->color == dnode_black) {
            parent = child->parent;
            if (child == parent->left) {
                sister = parent->right;
                assert (sister != nil);
                if (sister->color == dnode_red) {
                    sister->color = dnode_black;
                    parent->color = dnode_red;
                    rotate_left(parent);
                    sister = parent->right;
                    assert (sister != nil);
                }
                if (sister->left->color == dnode_black
                        && sister->right->color == dnode_black) {
                    sister->color = dnode_red;
                    child = parent;
                } else {
                    if (sister->right->color == dnode_black) {
                        assert (sister->left->color == dnode_red);
                        sister->left->color = dnode_black;
                        sister->color = dnode_red;
                        rotate_right(sister);
                        sister = parent->right;
                        assert (sister != nil);
                    }
                    sister->color = parent->color;
                    sister->right->color = dnode_black;
                    parent->color = dnode_black;
                    rotate_left(parent);
                    break;
                }
            } else {    /* symmetric case: child == child->parent->right */
                assert (child == parent->right);
                sister = parent->left;
                assert (sister != nil);
                if (sister->color == dnode_red) {
                    sister->color = dnode_black;
                    parent->color = dnode_red;
                    rotate_right(parent);
                    sister = parent->left;
                    assert (sister != nil);
                }
                if (sister->right->color == dnode_black
                        && sister->left->color == dnode_black) {
                    sister->color = dnode_red;
                    child = parent;
                } else {
                    if (sister->left->color == dnode_black) {
                        assert (sister->right->color == dnode_red);
                        sister->right->color = dnode_black;
                        sister->color = dnode_red;
                        rotate_left(sister);
                        sister = parent->left;
                        assert (sister != nil);
                    }
                    sister->color = parent->color;
                    sister->left->color = dnode_black;
                    parent->color = dnode_black;
                    rotate_right(parent);
                    break;
                }
            }
        }

        child->color = dnode_black;
        dict_root(dict)->color = dnode_black;
    }

    assert (dict_verify(dict));

    return target;
}

#ifdef NO_FC_SOLVE
static GCC_INLINE dnode_t *dnode_init(dnode_t *dnode, void *data)
#else
static GCC_INLINE dnode_t *dnode_init(dnode_t *dnode)
#endif
{
#ifdef NO_FC_SOLVE
    dnode->data = data;
#endif
    dnode->parent = NULL;
    dnode->left = NULL;
    dnode->right = NULL;
    return dnode;
}

/*
 * Allocate a node using the dictionary's allocator routine, give it
 * the data item.
 */

#ifdef NO_FC_SOLVE
const dict_ret_key_t fc_solve_kaz_tree_alloc_insert(dict_t *dict, const void *key, void *data)
#else
const dict_ret_key_t fc_solve_kaz_tree_alloc_insert(dict_t *dict, dict_key_t key)
#endif
{
    dnode_t * from_bin;
    dnode_t * node;
    dict_ret_key_t ret;
    fcs_compact_allocator_t * allocator;


    allocator = &(dict->dict_allocator);
    if ((from_bin = dict->dict_recycle_bin) != NULL)
    {
        node = dict->dict_recycle_bin;
        dict->dict_recycle_bin = DNODE_NEXT(node);
    }
    else
    {
        node = (dnode_t *)
            fcs_compact_alloc_ptr(allocator, sizeof(*node))
            ;
    }

#ifdef NO_FC_SOLVE
    dnode_init(node, data);
#else
    dnode_init(node);
#endif

    if ((ret = fc_solve_kaz_tree_insert(dict, node, key)))
    {
        if (from_bin)
        {
            DNODE_NEXT(node) = dict->dict_recycle_bin;
            dict->dict_recycle_bin = node;
        }
        else
        {
            fcs_compact_alloc_release(allocator);
        }
    }
    return ret;
}

void fc_solve_kaz_tree_delete_free(dict_t *dict, dnode_t *node)
{
    fc_solve_kaz_tree_delete(dict, node);
#ifdef NO_FC_SOLVE
    dict->freenode(node, dict->context);
#else
    DNODE_NEXT(node) = dict->dict_recycle_bin;
    dict->dict_recycle_bin = node;
#endif
}

dnode_t *fc_solve_kaz_tree_first(dict_t *dict)
{
    dnode_t *nil = dict_nil(dict), *root = dict_root(dict), *left;

    if (root != nil)
        while ((left = root->left) != nil)
            root = left;

    return (root == nil) ? NULL : root;
}

#ifdef NO_FC_SOLVE
/*
 * Return the node with the lowest (leftmost) key. If the dictionary is empty
 * (that is, dict_isempty(dict) returns 1) a null pointer is returned.
 */


/*
 * Return the node with the highest (rightmost) key. If the dictionary is empty
 * (that is, dict_isempty(dict) returns 1) a null pointer is returned.
 */

dnode_t *dict_last(dict_t *dict)
{
    dnode_t *nil = dict_nil(dict), *root = dict_root(dict), *right;

    if (root != nil)
        while ((right = root->right) != nil)
            root = right;

    return (root == nil) ? NULL : root;
}

/*
 * Return the given node's successor node---the node which has the
 * next key in the the left to right ordering. If the node has
 * no successor, a null pointer is returned rather than a pointer to
 * the nil node.
 */
#endif

dnode_t * fc_solve_kaz_tree_next(dict_t *dict, dnode_t *curr)
{
    dnode_t *nil = dict_nil(dict), *parent, *left;

    if (curr->right != nil) {
        curr = curr->right;
        while ((left = curr->left) != nil)
            curr = left;
        return curr;
    }

    parent = curr->parent;

    while (parent != nil && curr == parent->right) {
        curr = parent;
        parent = curr->parent;
    }

    return (parent == nil) ? NULL : parent;
}

#ifdef NO_FC_SOLVE

/*
 * Return the given node's predecessor, in the key order.
 * The nil sentinel node is returned if there is no predecessor.
 */

dnode_t *dict_prev(dict_t *dict, dnode_t *curr)
{
    dnode_t *nil = dict_nil(dict), *parent, *right;

    if (curr->left != nil) {
        curr = curr->left;
        while ((right = curr->right) != nil)
            curr = right;
        return curr;
    }

    parent = curr->parent;

    while (parent != nil && curr == parent->left) {
        curr = parent;
        parent = curr->parent;
    }

    return (parent == nil) ? NULL : parent;
}
#endif


#undef dict_count
#undef dict_isempty
#undef dict_isfull
#undef dnode_get
#undef dnode_put
#undef dnode_getkey

#ifdef NO_FC_SOLVE
void dict_allow_dupes(dict_t *dict)
{
    dict->dupes = 1;
}

dictcount_t dict_count(dict_t *dict)
{
    return dict->nodecount;
}

int dict_isempty(dict_t *dict)
{
    return dict->nodecount == 0;
}

int dict_isfull(dict_t *dict)
{
    return dict->nodecount == dict->maxcount;
}

int dict_contains(dict_t *dict, dnode_t *node)
{
    return verify_dict_has_node(dict_nil(dict), dict_root(dict), node);
}

static dnode_t *dnode_alloc(void *context)
{
    return (dnode_t *) malloc(sizeof *dnode_alloc(NULL));
}

static void dnode_free(dnode_t *node, void *context)
{
    free(node);
}

dnode_t *dnode_create(void *data)
{
    dnode_t *dnode = (dnode_t *) SMALLOC1(dnode)
    if (dnode) {
        dnode->data = data;
        dnode->parent = NULL;
        dnode->left = NULL;
        dnode->right = NULL;
    }
    return dnode;
}

void dnode_destroy(dnode_t *dnode)
{
    assert (!dnode_is_in_a_dict(dnode));
    free(dnode);
}

void *dnode_get(dnode_t *dnode)
{
    return dnode->data;
}

const void *dnode_getkey(dnode_t *dnode)
{
    return dnode->key;
}

void dnode_put(dnode_t *dnode, void *data)
{
    dnode->data = data;
}

int dnode_is_in_a_dict(dnode_t *dnode)
{
    return (dnode->parent && dnode->left && dnode->right);
}

void dict_process(dict_t *dict, void *context, dnode_process_t function)
{
    dnode_t *node = fc_solve_kaz_tree_first(dict), *next;

    while (node != NULL) {
        /* check for callback function deleting */
        /* the next node from under us          */
        assert (dict_contains(dict, node));
        next = dict_next(dict, node);
        function(dict, node, context);
        node = next;
    }
}
#endif

static void load_begin_internal(dict_load_t *load, dict_t *dict)
{
    load->dictptr = dict;
    load->nilnode.left = &load->nilnode;
    load->nilnode.right = &load->nilnode;
}

void dict_load_begin(dict_load_t *load, dict_t *dict)
{
    assert (dict_isempty(dict));
    load_begin_internal(load, dict);
}

#ifdef NO_FC_SOLVE
void dict_load_next(dict_load_t *load, dnode_t *newnode, const void *key)
{
    dict_t *dict = load->dictptr;
    dnode_t *nil = &load->nilnode;

    assert (!dnode_is_in_a_dict(newnode));
    assert (dict->nodecount < dict->maxcount);

    #ifndef NDEBUG
    if (dict->nodecount > 0) {
        if (dict->dupes)
            assert (dict->compare(nil->left->key, key) <= 0);
        else
            assert (dict->compare(nil->left->key, key) < 0);
    }
    #endif

    newnode->key = key;
    nil->right->left = newnode;
    nil->right = newnode;
    newnode->left = nil;
#ifdef NO_FC_SOLVE
    dict->nodecount++;
#endif
}

void dict_load_end(dict_load_t *load)
{
    dict_t *dict = load->dictptr;
    dnode_t *tree[DICT_DEPTH_MAX] = { 0 };
    dnode_t *curr, *dictnil = dict_nil(dict), *loadnil = &load->nilnode, *next;
    dnode_t *complete = 0;
    dictcount_t fullcount = DICTCOUNT_T_MAX, nodecount = dict->nodecount;
    dictcount_t botrowcount;
    unsigned baselevel = 0, level = 0, i;

    assert (dnode_red == 0 && dnode_black == 1);

    while (fullcount >= nodecount && fullcount)
        fullcount >>= 1;

    botrowcount = nodecount - fullcount;

    for (curr = loadnil->left; curr != loadnil; curr = next) {
        next = curr->left;

        if (complete == NULL && botrowcount-- == 0) {
            assert (baselevel == 0);
            assert (level == 0);
            baselevel = level = 1;
            complete = tree[0];

            if (complete != 0) {
                tree[0] = 0;
                complete->right = dictnil;
                while (tree[level] != 0) {
                    tree[level]->right = complete;
                    complete->parent = tree[level];
                    complete = tree[level];
                    tree[level++] = 0;
                }
            }
        }

        if (complete == NULL) {
            curr->left = dictnil;
            curr->right = dictnil;
            curr->color = (dnode_color_t) (level % 2);
            complete = curr;

            assert (level == baselevel);
            while (tree[level] != 0) {
                tree[level]->right = complete;
                complete->parent = tree[level];
                complete = tree[level];
                tree[level++] = 0;
            }
        } else {
            curr->left = complete;
            curr->color = (dnode_color_t) ((level + 1) % 2);
            complete->parent = curr;
            tree[level] = curr;
            complete = 0;
            level = baselevel;
        }
    }

    if (complete == NULL)
        complete = dictnil;

    for (i = 0; i < DICT_DEPTH_MAX; i++) {
        if (tree[i] != 0) {
            tree[i]->right = complete;
            complete->parent = tree[i];
            complete = tree[i];
        }
    }

    dictnil->color = dnode_black;
    dictnil->right = dictnil;
    complete->parent = dictnil;
    complete->color = dnode_black;
    dict_root(dict) = complete;

    assert (dict_verify(dict));
}

void dict_merge(dict_t *dest, dict_t *source)
{
    dict_load_t load;
    dnode_t *leftnode = fc_solve_kaz_tree_first(dest), *rightnode = fc_solve_kaz_tree_first(source);

    assert (dict_similar(dest, source));

    if (source == dest)
        return;

#ifdef NO_FC_SOLVE
    dest->nodecount = 0;
#endif
    load_begin_internal(&load, dest);

    for (;;) {
        if (leftnode != NULL && rightnode != NULL) {
            if (dest->compare(leftnode->key, rightnode->key, source->context) < 0)
                goto copyleft;
            else
                goto copyright;
        } else if (leftnode != NULL) {
            goto copyleft;
        } else if (rightnode != NULL) {
            goto copyright;
        } else {
            assert (leftnode == NULL && rightnode == NULL);
            break;
        }

    copyleft:
        {
            dnode_t *next = dict_next(dest, leftnode);
            #ifndef NDEBUG
            leftnode->left = NULL;      /* suppress assertion in dict_load_next */
            #endif
            dict_load_next(&load, leftnode, leftnode->key);
            leftnode = next;
            continue;
        }

    copyright:
        {
            dnode_t *next = dict_next(source, rightnode);
            #ifndef NDEBUG
            rightnode->left = NULL;
            #endif
            dict_load_next(&load, rightnode, rightnode->key);
            rightnode = next;
            continue;
        }
    }

    dict_clear(source);
    dict_load_end(&load);
}
#endif

#ifdef KAZLIB_TEST_MAIN

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdarg.h>

typedef char input_t[256];

static int tokenize(char *string, ...)
{
    char **tokptr;
    va_list arglist;
    int tokcount = 0;

    va_start(arglist, string);
    tokptr = va_arg(arglist, char **);
    while (tokptr) {
        while (*string && isspace((unsigned char) *string))
            string++;
        if (!*string)
            break;
        *tokptr = string;
        while (*string && !isspace((unsigned char) *string))
            string++;
        tokptr = va_arg(arglist, char **);
        tokcount++;
        if (!*string)
            break;
        *string++ = 0;
    }
    va_end(arglist);

    return tokcount;
}

static int comparef(const void *key1, const void *key2)
{
    return strcmp((const char *) key1, (const char *) key2);
}

static dnode_t *new_node(void *c)
{
    static dnode_t few[5];
    static int count;

    if (count < 5)
        return few + count++;

    return NULL;
}

static void del_node(dnode_t *n, void *c)
{
}

static int prompt = 0;

static void construct(dict_t *d)
{
    input_t in;
    int done = 0;
    dict_load_t dl;
    dnode_t *dn;
    char *tok1, *tok2, *val;
    const char *key;
    char *help =
        "p                      turn prompt on\n"
        "q                      finish construction\n"
        "a <key> <val>          add new entry\n";

    if (!dict_isempty(d))
        puts("warning: dictionary not empty!");

    dict_load_begin(&dl, d);

    while (!done) {
        if (prompt)
            putchar('>');
        fflush(stdout);

        if (!fgets(in, sizeof(input_t), stdin))
            break;

        switch (in[0]) {
            case '?':
                puts(help);
                break;
            case 'p':
                prompt = 1;
                break;
            case 'q':
                done = 1;
                break;
            case 'a':
                if (tokenize(in+1, &tok1, &tok2, (char **) 0) != 2) {
                    puts("what?");
                    break;
                }
                key = strdup(tok1);
                val = strdup(tok2);
                dn = dnode_create(val);

                if (!key || !val || !dn) {
                    puts("out of memory");
                    free((void *) key);
                    free(val);
                    if (dn)
                        dnode_destroy(dn);
                }

                dict_load_next(&dl, dn, key);
                break;
            default:
                putchar('?');
                putchar('\n');
                break;
        }
    }

    dict_load_end(&dl);
}

int main(void)
{
    input_t in;
    dict_t darray[10];
    dict_t *d = &darray[0];
    dnode_t *dn;
    size_t i;
    char *tok1, *tok2, *val;
    const char *key;

    char *help =
        "a <key> <val>          add value to dictionary\n"
        "d <key>                delete value from dictionary\n"
        "l <key>                lookup value in dictionary\n"
        "( <key>                lookup lower bound\n"
        ") <key>                lookup upper bound\n"
        "< <key>                lookup strict lower bound\n"
        "> <key>                lookup strict upper bound\n"
        "# <num>                switch to alternate dictionary (0-9)\n"
        "j <num> <num>          merge two dictionaries\n"
        "f                      free the whole dictionary\n"
        "k                      allow duplicate keys\n"
        "c                      show number of entries\n"
        "t                      dump whole dictionary in sort order\n"
        "m                      make dictionary out of sorted items\n"
        "p                      turn prompt on\n"
        "s                      switch to non-functioning allocator\n"
        "q                      quit";

    for (i = 0; i < sizeof darray / sizeof *darray; i++)
        dict_init(&darray[i], DICTCOUNT_T_MAX, comparef);

    for (;;) {
        if (prompt)
            putchar('>');
        fflush(stdout);

        if (!fgets(in, sizeof(input_t), stdin))
            break;

        switch(in[0]) {
            case '?':
                puts(help);
                break;
            case 'a':
                if (tokenize(in+1, &tok1, &tok2, (char **) 0) != 2) {
                    puts("what?");
                    break;
                }
                key = strdup(tok1);
                val = strdup(tok2);

                if (!key || !val) {
                    puts("out of memory");
                    free((void *) key);
                    free(val);
                }

                if (!dict_alloc_insert(d, key, val)) {
                    puts("dict_alloc_insert failed");
                    free((void *) key);
                    free(val);
                    break;
                }
                break;
            case 'd':
                if (tokenize(in+1, &tok1, (char **) 0) != 1) {
                    puts("what?");
                    break;
                }
                dn = dict_lookup(d, tok1);
                if (!dn) {
                    puts("dict_lookup failed");
                    break;
                }
                val = (char *) dnode_get(dn);
                key = (char *) dnode_getkey(dn);
                dict_delete_free(d, dn);

                free(val);
                free((void *) key);
                break;
            case 'f':
                dict_free_nodes(d);
                break;
            case 'l':
            case '(':
            case ')':
            case '<':
            case '>':
                if (tokenize(in+1, &tok1, (char **) 0) != 1) {
                    puts("what?");
                    break;
                }
                dn = 0;
                switch (in[0]) {
                case 'l':
                    dn = dict_lookup(d, tok1);
                    break;
                case '(':
                    dn = dict_lower_bound(d, tok1);
                    break;
                case ')':
                    dn = dict_upper_bound(d, tok1);
                    break;
                case '<':
                    dn = dict_strict_lower_bound(d, tok1);
                    break;
                case '>':
                    dn = dict_strict_upper_bound(d, tok1);
                    break;
                }
                if (!dn) {
                    puts("lookup failed");
                    break;
                }
                val = (char *) dnode_get(dn);
                puts(val);
                break;
            case 'm':
                construct(d);
                break;
            case 'k':
                dict_allow_dupes(d);
                break;
            case 'c':
                printf("%lu\n", (unsigned long) dict_count(d));
                break;
            case 't':
                for (dn = fc_solve_kaz_tree_first(d); dn; dn = dict_next(d, dn)) {
                    printf("%s\t%s\n", (char *) dnode_getkey(dn),
                            (char *) dnode_get(dn));
                }
                break;
            case 'q':
                exit(0);
                break;
            case '\0':
                break;
            case 'p':
                prompt = 1;
                break;
            case 's':
                dict_set_allocator(d, new_node, del_node, NULL);
                break;
            case '#':
                if (tokenize(in+1, &tok1, (char **) 0) != 1) {
                    puts("what?");
                    break;
                } else {
                    int dictnum = atoi(tok1);
                    if (dictnum < 0 || dictnum > 9) {
                        puts("invalid number");
                        break;
                    }
                    d = &darray[dictnum];
                }
                break;
            case 'j':
                if (tokenize(in+1, &tok1, &tok2, (char **) 0) != 2) {
                    puts("what?");
                    break;
                } else {
                    int dict1 = atoi(tok1), dict2 = atoi(tok2);
                    if (dict1 < 0 || dict1 > 9 || dict2 < 0 || dict2 > 9) {
                        puts("invalid number");
                        break;
                    }
                    dict_merge(&darray[dict1], &darray[dict2]);
                }
                break;
            default:
                putchar('?');
                putchar('\n');
                break;
        }
    }

    return 0;
}

#endif
