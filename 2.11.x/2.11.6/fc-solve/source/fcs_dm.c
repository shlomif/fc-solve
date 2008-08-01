/*
    fcs_dm.c - Freecell Solver's data management routines.

    Written by Shlomi Fish, 2000

    This file is distributed under the public domain.
    (It's not copyrighted)
*/

#include <stddef.h>
#include <string.h>

#include "fcs_dm.h"

#ifdef DMALLOC
#include "dmalloc.h"
#endif

/*
    freecell_solver_bsearch - an improved binary search function. Highlights:

    * The comparison function accepts a common context argument that
    is passed to SFO_bsearch.
    * If the item was not found the function returns the place in which
    it should be placed, while setting *found to 0. If it was found
      (*found) is set to 1.
*/
void * freecell_solver_bsearch
(
    void * key,
    void * void_array,
    size_t len,
    size_t width,
    int (* compare)(const void *, const void *, void *),
    void * context,
    int * found
)
{
    int low = 0;
    int high = len-1;
    int mid;
    int result;

    char * array = void_array;

    while (low <= high)
    {
        mid = ((low+high)>>1);

        result = compare(key, (void*)(array+mid*width), context);

        if (result < 0)
        {
            high = mid-1;
        }
        else if (result > 0)
        {
            low = mid+1;
        }
        else
        {
            *found = 1;
            return (void*)(array+mid*width);
        }
    }

    *found = 0;
    return ((void*)(array+(high+1)*width));
}



/*
    freecell_solver_merge_large_and_small_sorted_array - merges a large sorted
    array with a small sorted array. The arrays could be of any length
    whatsoever, but it works faster if the first is significantly bigger
    than the second.

    This function assumes that big_array is allocated with enough
    space to hold the extra elements.

    The array should be distinct or else there would be unexpected
    results.
*/
int freecell_solver_merge_large_and_small_sorted_arrays
(
    void * void_big_array,
    size_t size_big_array,
    void * void_small_array,
    size_t size_small_array,
    size_t width,
    int (*compare) (const void *, const void *, void *),
    void * context
)
{
    int item_to_move, num_big_items_moved, pos;
    char * pos_ptr;
    char * big_array;
    char * small_array;
    int found;
    int start_offset, end_offset;

    big_array = (char*)void_big_array;
    small_array = (char*)void_small_array;

    num_big_items_moved = 0;

    for(item_to_move = size_small_array-1 ; item_to_move>=0; item_to_move--)
    {
        pos_ptr = freecell_solver_bsearch (
            small_array+item_to_move*width,
            big_array,
            size_big_array-num_big_items_moved,
            width,
            compare,
            context,
            &found
        );

        pos = (pos_ptr-big_array)/width;

        end_offset = size_big_array + size_small_array -
            num_big_items_moved -
            (size_small_array-item_to_move-1);

        start_offset = end_offset + pos -
            (size_big_array - num_big_items_moved);

        memmove(
            big_array+start_offset*width,
            big_array+pos*width,
            (end_offset-start_offset)*width
        );

        memcpy(
            big_array+(start_offset-1)*width,
            small_array+item_to_move*width,
            width
        );

        num_big_items_moved += (end_offset - start_offset);
    }

    return 1;
}

