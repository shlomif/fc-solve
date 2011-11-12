#include <stdio.h>
#include <string.h>
#include <pthread.h>

#include "config.h"

#include "bool.h"
#include "inline.h"

#include "dbm_solver.h"

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

static void GCC_INLINE cache_destroy(fcs_lru_cache_t * cache)
{
    fc_solve_kaz_tree_destroy(cache->kaz_tree);
    fc_solve_compact_allocator_finish(&(cache->states_values_to_keys_allocator));
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
            (fcs_cache_key_info_t *)
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

static void GCC_INLINE cache_populate_from_pre_cache(
    fcs_lru_cache_t * cache,
    fcs_pre_cache_t * pre_cache
)
{
    dnode_t * node;
    dict_t * kaz_tree;

    kaz_tree = pre_cache->kaz_tree;

    for (node = fc_solve_kaz_tree_first(kaz_tree);
            node ;
            node = fc_solve_kaz_tree_next(kaz_tree, node)
            )
    {
        cache_insert(
            cache, 
            ((fcs_pre_cache_key_val_pair_t *)(node->dict_key))->key
        );
    }
}

static void GCC_INLINE pre_cache_offload_and_destroy(
    fcs_pre_cache_t * pre_cache,
    fcs_dbm_store_t store,
    fcs_lru_cache_t * cache
)
{
    fc_solve_dbm_store_offload_pre_cache(store, pre_cache);
    cache_populate_from_pre_cache(cache, pre_cache);

    /* Now reset the pre_cache. */
    fc_solve_kaz_tree_destroy(pre_cache->kaz_tree);
    fc_solve_compact_allocator_finish(&(pre_cache->kv_allocator));
}

static void GCC_INLINE pre_cache_offload_and_reset(
    fcs_pre_cache_t * pre_cache,
    fcs_dbm_store_t store,
    fcs_lru_cache_t * cache
)
{
    pre_cache_offload_and_destroy(pre_cache, store, cache);
    pre_cache_init(pre_cache);
}

struct fcs_dbm_queue_item_struct
{
    fcs_encoded_state_buffer_t key;
    struct fcs_dbm_queue_item_struct * next;
};

typedef struct fcs_dbm_queue_item_struct fcs_dbm_queue_item_t;

static const pthread_mutex_t initial_mutex_constant =
    PTHREAD_MUTEX_INITIALIZER
    ;

typedef struct {
    fcs_pre_cache_t pre_cache;
    fcs_dbm_store_t store;
    fcs_lru_cache_t cache;

    long pre_cache_max_count;
    /* The queue */
    
    pthread_mutex_t queue_lock;
    /* TODO : initialize the allocator. */
    fcs_compact_allocator_t queue_allocator;
    /* TODO : offload the queue to the hard disk. */
    fcs_dbm_queue_item_t * queue_head, * queue_tail, * queue_recycle_bin;
} fcs_dbm_solver_instance_t;

static void GCC_INLINE instance_init(
    fcs_dbm_solver_instance_t * instance,
    long pre_cache_max_count,
    long caches_delta,
    const char * dbm_store_path
)
{
    instance->queue_lock = initial_mutex_constant;
    fc_solve_compact_allocator_init(
        &(instance->queue_allocator)
    );
    instance->queue_head =
        instance->queue_tail =
        instance->queue_recycle_bin =
        NULL;

    pre_cache_init (&(instance->pre_cache));
    instance->pre_cache_max_count = pre_cache_max_count;
    cache_init (&(instance->cache), pre_cache_max_count+caches_delta);
    fc_solve_dbm_store_init(&(instance->store), dbm_store_path);
}

static void GCC_INLINE instance_destroy(
    fcs_dbm_solver_instance_t * instance
    )
{
    /* TODO : store what's left on the queue on the hard-disk. */
    fc_solve_compact_allocator_finish(&(instance->queue_allocator));

    pre_cache_offload_and_destroy(
        &(instance->pre_cache),
        instance->store,
        &(instance->cache)
    );

    cache_destroy(&(instance->cache));

    fc_solve_dbm_store_destroy(instance->store);
}

static void GCC_INLINE instance_check_key(
    fcs_dbm_solver_instance_t * instance,
    unsigned char * key,
    unsigned char * parent_and_move
)
{
    fcs_lru_cache_t * cache;
    fcs_pre_cache_t * pre_cache;

    cache = &(instance->cache);
    pre_cache = &(instance->pre_cache);

    if (cache_does_key_exist(cache, key))
    {
        return;
    }
    else if (pre_cache_does_key_exist(pre_cache, key))
    {
        return;
    }
    else if (fc_solve_dbm_store_does_key_exist(instance->store, key))
    {
        cache_insert(cache, key);
        return;
    }
    else
    {
        fcs_dbm_queue_item_t * new_item;

        pre_cache_insert(pre_cache, key, parent_and_move);

        /* Now insert it into the queue. */

        pthread_mutex_lock(&instance->queue_lock);
        
        if (instance->queue_recycle_bin)
        {
            instance->queue_recycle_bin = 
            (new_item = instance->queue_recycle_bin)->next;
        }
        else
        {
            new_item =
                (fcs_dbm_queue_item_t *)
                fcs_compact_alloc_ptr(
                    &(instance->queue_allocator),
                    sizeof(*new_item)
                );
        }

        memcpy(new_item->key, key, sizeof(new_item->key));
        new_item->next = NULL;

        if (instance->queue_tail)
        {
            instance->queue_tail = instance->queue_tail->next = new_item;
        }
        else
        {
            instance->queue_head = instance->queue_tail = new_item;
        }
        pthread_mutex_unlock(&instance->queue_lock);
    }
}

/* Temporary main() function to make gcc happy. */
int main(int argc, char * argv[])
{
    printf("%s\n", "Hello");

    return 0;
}
