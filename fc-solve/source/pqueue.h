/*
    pqueue.h - header file for the priority queue implementation.

    Originally written by Justin-Heyes Jones
    Modified by Shlomi Fish, 2000

    This file is in the public domain (it's uncopyrighted).

    Check out Justin-Heyes Jones' A* page from which this code has
    originated:
        http://www.geocities.com/jheyesjones/astar.html
*/

#ifndef __PQUEUE_H
#define __PQUEUE_H

#ifdef __cplusplus
extern "C" {
#endif

#include "jhjtypes.h"

typedef int32 pq_rating_t;

typedef struct struct_pq_element_t
{
    void * item;
    pq_rating_t rating;
} pq_element_t;

typedef struct _PQUEUE
{
    int32 MaxSize;
    int32 CurrentSize;
    pq_element_t * Elements; /* pointer to void pointers */
    pq_rating_t MaxRating; /* biggest element possible */
    int IsAscendingHeap; /* true if the heap should be sorted with the maximum scoring elements first */
} PQUEUE;

/* given an index to any element in a binary tree stored in a linear array with the root at 1 and
   a "sentinel" value at 0 these macros are useful in making the code clearer */

/* the parent is always given by index/2 */
#define PQ_PARENT_INDEX(i) ((i)>>1)
#define PQ_FIRST_ENTRY (1)

/* left and right children are index * 2 and (index * 2) +1 respectively */
#define PQ_LEFT_CHILD_INDEX(i) ((i)<<1)
#define PQ_RIGHT_CHILD_INDEX(i) (((i)<<)+1)

void freecell_solver_PQueueInitialise(
    PQUEUE *pq,
    int32 MaxElements,
    pq_rating_t MaxRating,
    int bIsAscending
    );

void freecell_solver_PQueueFree( PQUEUE *pq );

int freecell_solver_PQueuePush( PQUEUE *pq, void *item, pq_rating_t);

int freecell_solver_PQueueIsEmpty( PQUEUE *pq );

void *freecell_solver_PQueuePop( PQUEUE *pq);

#define PGetRating(elem) ((elem).rating)

#ifdef __cplusplus
}
#endif

#endif /* #ifdef __PQUEUE_H */
