/*
    pqueue.c - implementation of a priority queue by using a binary heap.

    Originally written by Justin-Heyes Jones
    Modified by Shlomi Fish, 2000

    This file is in the public domain (it's uncopyrighted).

    Check out Justin-Heyes Jones' A* page from which this code has
    originated:
        http://www.geocities.com/jheyesjones/astar.html
  */

/* manage a priority queue as a heap
   the heap is implemented as a fixed size array of pointers to your data */

#include <stdio.h>
#include <stdlib.h>

#include "jhjtypes.h"

#include "pqueue.h"

#ifdef DMALLOC
#include "dmalloc.h"
#endif

#define TRUE 1
#define FALSE 0

/* initialise the priority queue with a maximum size of maxelements. maxrating is the highest or lowest value of an
   entry in the pqueue depending on whether it is ascending or descending respectively. Finally the bool32 tells you whether
   the list is sorted ascending or descending... */

void freecell_solver_PQueueInitialise(
    PQUEUE *pq,
    int32 MaxElements
    )
{
    pq->MaxSize = MaxElements;

    pq->CurrentSize = 0;

    pq->Elements = (pq_element_t*) malloc( sizeof( pq_element_t ) * (MaxElements + 1) );

    if( pq->Elements == NULL )
    {
        printf( "Memory alloc failed!\n" );
    }
}

/* join a priority queue
   returns TRUE if succesful, FALSE if fails. (You fail by filling the pqueue.)
   PGetRating is a function which returns the rating of the item you're adding for sorting purposes */

int freecell_solver_PQueuePush( PQUEUE *pq, void *item, pq_rating_t r)
{
    uint32 i;
    pq_element_t * Elements = pq->Elements;

    int32 CurrentSize = pq->CurrentSize;

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
        Elements[i].item = item;
        Elements[i].rating = r;
    }

    pq->CurrentSize = CurrentSize;

    return TRUE;

}

#define PQueueIsEmpty(pq) ((pq)->CurrentSize == 0)

/* free up memory for pqueue */
void freecell_solver_PQueueFree( PQUEUE *pq )
{
    free( pq->Elements );
}

/* remove the first node from the pqueue and provide a pointer to it */

void *freecell_solver_PQueuePop( PQUEUE *pq)
{
    int32 i;
    int32 child;
    pq_element_t * Elements = pq->Elements;
    int32 CurrentSize = pq->CurrentSize;

    pq_element_t pMaxElement;
    pq_element_t pLastElement;

    if( PQueueIsEmpty( pq ) )
    {
        return NULL;
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

    return pMaxElement.item;
}


