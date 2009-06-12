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

/* manage a priority queue as a heap
   the heap is implemented as a fixed size array of pointers to your data */

#define PQUEUE_MaxRating INT_MAX

#define PGetRating(elem) ((elem).rating)

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
/* initialise the priority queue with a maximum size of maxelements. maxrating is the highest or lowest value of an
   entry in the pqueue depending on whether it is ascending or descending respectively. Finally the bool32 tells you whether
   the list is sorted ascending or descending... */

static void GCC_INLINE fc_solve_PQueueInitialise(
    PQUEUE *pq,
    int MaxElements
    )
{
    pq->MaxSize = MaxElements;

    pq->CurrentSize = 0;

    pq->Elements = (pq_element_t*) malloc( sizeof( pq_element_t ) * (MaxElements + 1) );
}

static void GCC_INLINE fc_solve_PQueueFree( PQUEUE *pq )
{
    free( pq->Elements );
}


#include <stdio.h>
#include <stdlib.h>

/* join a priority queue
   returns TRUE if succesful, FALSE if fails. (You fail by filling the pqueue.)
   PGetRating is a function which returns the rating of the item you're adding for sorting purposes */

static GCC_INLINE int fc_solve_PQueuePush(
        PQUEUE *pq,
        fcs_state_extra_info_t * val,
        pq_rating_t r
        )
{
    unsigned int i;
    pq_element_t * Elements = pq->Elements;

    int CurrentSize = pq->CurrentSize;

    if (CurrentSize == pq->MaxSize )
    {
        int new_size;
        new_size = pq->MaxSize + 256;
        pq->Elements = Elements = (pq_element_t *)realloc( Elements, sizeof(pq_element_t) * (new_size+1));
        pq->MaxSize = new_size;
    }

    {
        /* set i to the first unused element and increment CurrentSize */

        i = (++CurrentSize);

        /* while the parent of the space we're putting the new node into is worse than
           our new node, swap the space with the worse node. We keep doing that until we
           get to a worse node or until we get to the top

           note that we also can sort so that the minimum elements bubble up so we need to loops
           with the comparison operator flipped... */

        {

            while( ( i==PQ_FIRST_ENTRY ?
                     (PQUEUE_MaxRating) /* return biggest possible rating if first element */
                     :
                     (PGetRating(Elements[ PQ_PARENT_INDEX(i) ]) )
                   )
                   < r
                 )
            {
                Elements[ i ] = Elements[ PQ_PARENT_INDEX(i) ];

                i = PQ_PARENT_INDEX(i);
            }
        }

        /* then add the element at the space we created. */
        Elements[i].val = val;
        Elements[i].rating = r;
    }

    pq->CurrentSize = CurrentSize;

    return 1;
}

#define PQueueIsEmpty(pq) ((pq)->CurrentSize == 0)

/* remove the first node from the pqueue and provide a pointer to it
 *
 * Returns 0 if found.
 * Returns 1 if not found.
 *
 * */

static GCC_INLINE int fc_solve_PQueuePop( PQUEUE *pq, fcs_state_extra_info_t * * val)
{
    int i;
    int child;
    pq_element_t * Elements = pq->Elements;
    int CurrentSize = pq->CurrentSize;

    pq_element_t pMaxElement;
    pq_element_t pLastElement;

    if( PQueueIsEmpty( pq ) )
    {
        *val = NULL;
        return 1;
    }

    pMaxElement = Elements[PQ_FIRST_ENTRY];

    /* get pointer to last element in tree */
    pLastElement = Elements[ CurrentSize-- ];

    {

        /* code to pop an element from an ascending (top to bottom) pqueue */

        /*  UNTESTED */

        for( i=PQ_FIRST_ENTRY; (child = PQ_LEFT_CHILD_INDEX(i)) <= CurrentSize; i=child )
        {
            /* set child to the smaller of the two children... */

            if( (child != CurrentSize) &&
                (PGetRating(Elements[child + 1]) > PGetRating(Elements[child])) )
            {
                child ++;
            }

            if( PGetRating( pLastElement ) < PGetRating( Elements[ child ] ) )
            {
                Elements[ i ] = Elements[ child ];
            }
            else
            {
                break;
            }
        }
    }

    Elements[i] = pLastElement;
    pq->CurrentSize = CurrentSize;

    *val = pMaxElement.val;
    return 0;
}


#ifdef __cplusplus
}
#endif

#endif /* #ifdef FC_SOLVE__PQUEUE_H */
