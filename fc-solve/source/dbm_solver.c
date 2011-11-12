#include <stdio.h>
#include "config.h"

#include "inline.h"
#include "bool.h"

#include "kaz_tree.h"

typedef unsigned char fcs_encoded_state_buffer_t[24];

struct fcs_cache_key_info_struct
{
    fcs_encoded_state_buffer_t key;
    /* lower_pri and higher_pri form a doubly linked list.
     *
     * pri == priority.
     * */
    struct fcs_cache_key_info_struct * lower_pri, * higher_pri;
};

typedef struct fcs_cache_key_info_struct fcs_cache_key_info_t;

typedef struct
{
#if (FCS_RCS_CACHE_STORAGE == FCS_RCS_CACHE_STORAGE_JUDY)
    Pvoid_t states_values_to_keys_map;
#elif (FCS_RCS_CACHE_STORAGE == FCS_RCS_CACHE_STORAGE_KAZ_TREE)
    dict_t * kaz_tree;
#else
#error unknown FCS_RCS_CACHE_STORAGE
#endif
    fcs_compact_allocator_t states_values_to_keys_allocator;
    long count_elements_in_cache, max_num_elements_in_cache;

    fcs_cache_key_info_t * lowest_pri, * highest_pri;

    fcs_cache_key_info_t * recycle_bin;
} fcs_lru_cache_t;

/* TODO: make sure the key is '\0'-padded. */
static int fc_solve_compare_lru_cache_keys(
    const void * void_a, const void * void_b, void * context
)
{
#define GET_PARAM(p) ((((const fcs_cache_key_info_t *)(p))->key))
    return memcmp(GET_PARAM(void_a), GET_PARAM(void_b), sizeof(GET_PARAM(void_a)));
#undef GET_PARAM
}

static void GCC_INLINE cache_init(fcs_lru_cache_t * cache, long max_num_elements_in_cache)
{
#if (FCS_RCS_CACHE_STORAGE == FCS_RCS_CACHE_STORAGE_JUDY)
    cache->states_values_to_keys_map = ((Pvoid_t) NULL);
#elif (FCS_RCS_CACHE_STORAGE == FCS_RCS_CACHE_STORAGE_KAZ_TREE)
    cache->kaz_tree = fc_solve_kaz_tree_create(fc_solve_compare_lru_cache_keys, NULL);
#else
#error Unknown FCS_RCS_CACHE_STORAGE
#endif

    fc_solve_compact_allocator_init(
        &(cache->states_values_to_keys_allocator)
    );
    cache->lowest_pri = NULL;
    cache->highest_pri = NULL;
    cache->recycle_bin = NULL;
    cache->count_elements_in_cache = 0;
    cache->max_num_elements_in_cache = max_num_elements_in_cache;
}

static fcs_bool_t GCC_INLINE cache_does_key_exist(fcs_lru_cache_t * cache, unsigned char * key)
{
    fcs_cache_key_info_t to_check;
    dnode_t * node;

    memcpy(to_check.key, key, sizeof(to_check.key));

    node = fc_solve_kaz_tree_lookup(cache->kaz_tree, &to_check);

    if (! node)
    {
        return FALSE;
    }
    else
    {
        /* First - promote this key to the top of the cache. */
        fcs_cache_key_info_t * existing;

        existing = (fcs_cache_key_info_t *)(node->dict_key);

        if (existing->higher_pri)
        {
            existing->higher_pri->lower_pri =
                existing->lower_pri;
        }

        if (existing->lower_pri)
        {
            existing->lower_pri->higher_pri =
                existing->higher_pri;
        }
        /* Bug fix: make sure that ->lowest_pri is always valid. */
        else if (existing->higher_pri)
        {
            cache->lowest_pri = existing->higher_pri;
        }

        cache->highest_pri->higher_pri = existing;
        existing->lower_pri = cache->highest_pri;
        existing->higher_pri = NULL;
        cache->highest_pri = existing;

        return TRUE;
    }
}

static void GCC_INLINE cache_insert(fcs_lru_cache_t * cache, unsigned char * key)
{
    fcs_cache_key_info_t * cache_key;
    dict_t * kaz_tree;

    kaz_tree = cache->kaz_tree;

    if (cache->count_elements_in_cache >= cache->max_num_elements_in_cache)
    {
        fc_solve_kaz_tree_delete_free(
            kaz_tree,
            fc_solve_kaz_tree_lookup(
                kaz_tree, (cache_key = cache->lowest_pri)
                )
            );
            
        cache->lowest_pri = cache->lowest_pri->higher_pri;
        cache->lowest_pri->lower_pri = NULL;
    }
    else
    {
        cache_key =
            fcs_compact_alloc_ptr(
                &(cache->states_values_to_keys_allocator),
                sizeof(*cache_key)
            );
        cache->count_elements_in_cache++;
    }

    memcpy(cache_key->key, key, sizeof(cache_key->key));

    if (cache->highest_pri)
    {
        cache_key->lower_pri = cache->highest_pri;
        cache->highest_pri->higher_pri = cache_key;
        cache->highest_pri = cache_key;
    }
    else
    {
        cache->highest_pri = cache->lowest_pri = cache_key;
        cache_key->higher_pri = cache_key->lower_pri = NULL;
    }

    fc_solve_kaz_tree_alloc_insert(kaz_tree, cache_key);

}

union fcs_pre_cache_key_val_pair_struct
{
    struct {
        fcs_encoded_state_buffer_t key;
        fcs_encoded_state_buffer_t parent_and_move;
    };
    union fcs_pre_cache_key_val_pair_struct * next;
};

typedef union fcs_pre_cache_key_val_pair_struct fcs_pre_cache_key_val_pair_t;

typedef struct {
    dict_t * kaz_tree;
    fcs_compact_allocator_t kv_allocator;
    fcs_pre_cache_key_val_pair_t * kv_recycle_bin;
    long count_elements;
} fcs_pre_cache_t;

static int fc_solve_compare_pre_cache_keys(
    const void * void_a, const void * void_b, void * context
)
{
#define GET_PARAM(p) ((((const fcs_pre_cache_key_val_pair_t *)(p))->key))
    return memcmp(GET_PARAM(void_a), GET_PARAM(void_b), sizeof(GET_PARAM(void_a)));
#undef GET_PARAM
}

static void GCC_INLINE pre_cache_init(fcs_pre_cache_t * pre_cache_ptr)
{
    pre_cache_ptr->kaz_tree =
        fc_solve_kaz_tree_create(fc_solve_compare_pre_cache_keys, NULL);

    fc_solve_compact_allocator_init(&(pre_cache_ptr->kv_allocator));
    pre_cache_ptr->kv_recycle_bin = NULL;
    pre_cache_ptr->count_elements = 0;
}

static fcs_bool_t GCC_INLINE pre_cache_does_key_exist(
    fcs_pre_cache_t * pre_cache,
    unsigned char * key
    )
{
    fcs_pre_cache_key_val_pair_t to_check;

    memcpy(to_check.key, key, sizeof(to_check.key));

    return (fc_solve_kaz_tree_lookup(pre_cache->kaz_tree, &to_check) != NULL);
}

static void GCC_INLINE pre_cache_insert(
    fcs_pre_cache_t * pre_cache,
    unsigned char * key,
    unsigned char * parent_and_move
    )
{
    fcs_pre_cache_key_val_pair_t * to_insert;

    if (pre_cache->kv_recycle_bin)
    {
        pre_cache->kv_recycle_bin =
            (to_insert = pre_cache->kv_recycle_bin)->next;
    }
    else
    {
        to_insert =
            fcs_compact_alloc_ptr(
                &(pre_cache->kv_allocator),
                sizeof(*to_insert)
            );
    }
    memcpy(to_insert->key, key, sizeof(to_insert->key));
    memcpy(to_insert->parent_and_move, parent_and_move, 
            sizeof(to_insert->parent_and_move));

    fc_solve_kaz_tree_alloc_insert(pre_cache->kaz_tree, to_insert);
    pre_cache->count_elements++;
}

/* Temporary main() function to make gcc happy. */
int main(int argc, char * argv[])
{
    printf("%s\n", "Hello");

    return 0;
}
