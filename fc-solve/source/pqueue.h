/*
    pqueue.h - header file for the priority queue implementation.

    Originally written by Justin-Heyes Jones
    Modified by Shlomi Fish, 2000

    This file is in the public domain (it's uncopyrighted).

    Check out Justin-Heyes Jones' A* page from which this code has
    originated:
        http://www.geocities.com/jheyesjones/astar.html
*/

#ifndef FC_SOLVE__PQUEUE_H
#define FC_SOLVE__PQUEUE_H

#ifdef __cplusplus
extern "C" {
#endif

#include <limits.h>

#include "state.h"

#include "inline.h"

#define PQUEUE_MaxRating INT_MAX

typedef int pq_rating_t;

typedef struct
{
    fcs_state_extra_info_t * val;
    pq_rating_t rating;
} pq_element_t;

typedef struct
{
    int MaxSize;
    int CurrentSize;
    pq_element_t * Elements; /* pointer to void pointers */
} PQUEUE;

/* given an index to any element in a binary tree stored in a linear array with the root at 1 and
   a "sentinel" value at 0 these macros are useful in making the code clearer */

/* the parent is always given by index/2 */
#define PQ_PARENT_INDEX(i) ((i)>>1)
#define PQ_FIRST_ENTRY (1)

/* left and right children are index * 2 and (index * 2) +1 respectively */
#define PQ_LEFT_CHILD_INDEX(i) ((i)<<1)

void fc_solve_PQueueInitialise(
    PQUEUE *pq,
    int MaxElements
    );

static void GCC_INLINE fc_solve_PQueueFree( PQUEUE *pq )
{
    free( pq->Elements );
}

int fc_solve_PQueuePush(
        PQUEUE *pq,
        fcs_state_extra_info_t * val,
        pq_rating_t r
        );

int fc_solve_PQueuePop( PQUEUE *pq, fcs_state_extra_info_t * * val);

#define PGetRating(elem) ((elem).rating)

#ifdef __cplusplus
}
#endif

#endif /* #ifdef FC_SOLVE__PQUEUE_H */
