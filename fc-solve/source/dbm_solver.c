#include <stdio.h>
#include <string.h>
#include <pthread.h>

#include "config.h"

#include "bool.h"
#include "inline.h"

#include "dbm_solver.h"

#include "delta_states.c"

#define STACKS_NUM 8
#define LOCAL_STACKS_NUM STACKS_NUM
#define INSTANCE_DECKS_NUM 1
#define LOCAL_FREECELLS_NUM 2
#define DECKS_NUM INSTANCE_DECKS_NUM
#define RANK_KING 13

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
    pthread_mutex_t storage_lock;
    fcs_pre_cache_t pre_cache;
    fcs_dbm_store_t store;
    fcs_lru_cache_t cache;

    long pre_cache_max_count;
    /* The queue */
    
    pthread_mutex_t queue_lock;
    fcs_bool_t queue_solution_was_found;
    fcs_encoded_state_buffer_t queue_solution;
    fcs_compact_allocator_t queue_allocator;
    int queue_num_extracted_and_processed;
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
    instance->storage_lock = initial_mutex_constant;

    fc_solve_compact_allocator_init(
        &(instance->queue_allocator)
    );
    instance->queue_solution_was_found = FALSE;
    instance->queue_num_extracted_and_processed = 0;
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

struct fcs_derived_state_struct
{
    fcs_state_keyval_pair_t state;
    fcs_encoded_state_buffer_t key;
    fcs_encoded_state_buffer_t parent_and_move;
    struct fcs_derived_state_struct * next;
#ifdef INDIRECT_STACK_STATES
    dll_ind_buf_t indirect_stacks_buffer;
#endif
};

typedef struct fcs_derived_state_struct fcs_derived_state_t;

static void GCC_INLINE instance_check_multiple_keys(
    fcs_dbm_solver_instance_t * instance,
    fcs_derived_state_t * list
)
{
    /* Small optimization in case the list is empty. */
    if (!list)
    {
        return;
    }
    pthread_mutex_lock(&(instance->storage_lock));
    for (; list ; list = list->next)
    {
        instance_check_key(instance, list->key, list->parent_and_move);
    }
    if (instance->pre_cache.count_elements >= instance->pre_cache_max_count)
    {
        pre_cache_offload_and_reset(
            &(instance->pre_cache),
            instance->store,
            &(instance->cache)
        );
    }
    pthread_mutex_unlock(&(instance->storage_lock));
}

typedef struct {
    fcs_dbm_solver_instance_t * instance;
    fc_solve_delta_stater_t * delta_stater;
} fcs_dbm_solver_thread_t;

typedef struct {
    fcs_dbm_solver_thread_t * thread;
} thread_arg_t;

#define MAKE_MOVE(src, dest) ((src) | (dest) << 4)
#define COL2MOVE(idx) (idx)
#define FREECELL2MOVE(idx) (idx+8)
#define FOUND2MOVE(idx) ((idx)+8+4)

#ifdef INDIRECT_STACK_STATES

#define COPY_INDIRECT_COLS() \
{ \
    for (copy_col_idx=0;copy_col_idx < LOCAL_STACKS_NUM ; copy_col_idx++) \
    { \
        copy_stack_col = fcs_state_get_col((ptr_new_state->state.s), copy_col_idx); \
        memcpy(&ptr_new_state->indirect_stacks_buffer[copy_col_idx << 7], copy_stack_col, fcs_col_len(copy_stack_col)+1); \
   } \
}

#else

#define COPY_INDIRECT_COLS() {}

#endif

#define BEGIN_NEW_STATE() \
{ \
    if (*derived_list_recycle_bin) \
    {  \
        (*derived_list_recycle_bin) = \
        (ptr_new_state =  \
         (*derived_list_recycle_bin) \
        )->next; \
    } \
    else \
    { \
        ptr_new_state = \
        (fcs_derived_state_t *) \
        fcs_compact_alloc_ptr( \
                derived_list_allocator, \
                sizeof(*ptr_new_state) \
                ); \
    } \
    fcs_duplicate_state(  \
        &(ptr_new_state->state), \
        init_state_kv_ptr \
    ); \
        \
    COPY_INDIRECT_COLS() \
}

#define COMMIT_NEW_STATE(src, dest) \
{ \
 \
    memcpy( \
            ptr_new_state->parent_and_move, \
            key, \
            sizeof(ptr_new_state->parent_and_move) \
          ); \
    ptr_new_state->parent_and_move[ \
        ptr_new_state->parent_and_move[0]+1 \
        ] = MAKE_MOVE(src, dest); \
 \
    /* Finally, enqueue the new state. */ \
    ptr_new_state->next = (*derived_list); \
    (*derived_list) = ptr_new_state; \
 \
}

static GCC_INLINE fcs_bool_t instance_solver_thread_calc_derived_states(
    fcs_state_keyval_pair_t * init_state_kv_ptr,
    fcs_encoded_state_buffer_t * key,
    fcs_derived_state_t * * derived_list,
    fcs_derived_state_t * * derived_list_recycle_bin,
    fcs_compact_allocator_t * derived_list_allocator
)
{
    fcs_derived_state_t * ptr_new_state; 
    int stack_idx, copy_col_idx, cards_num;
    fcs_cards_column_t col, copy_stack_col;
    fcs_card_t card;
    int deck, suit;

    /* TODO : the actual calculation. */

#define the_state (init_state_kv_ptr->s)
#define new_state (ptr_new_state->state.s)

#define SUIT_LIMIT ( DECKS_NUM * 4 )
    for (suit = 0 ; suit < SUIT_LIMIT ; suit++)
    {
        if (fcs_foundation_value(the_state, suit) < RANK_KING)
        {
            break;
        }
    }

    if (suit == SUIT_LIMIT)
    {
        /* Solved state. */
        return TRUE;
    }

    /* Move top stack cards to foundations. */
    for (stack_idx=0 ; stack_idx < LOCAL_STACKS_NUM ; stack_idx++)
    {
        col = fcs_state_get_col(the_state, stack_idx);
        cards_num = fcs_col_len(col);
        if (cards_num)
        {
            /* Get the top card in the stack */
            card = fcs_col_get_card(col, cards_num-1);
            suit = fcs_card_suit(card);
            for (deck=0 ; deck < INSTANCE_DECKS_NUM ; deck++)
            {

                if (fcs_foundation_value(the_state, deck*4+suit) == fcs_card_card_num(card) - 1)
                {
                    /* We can put it there */
                    BEGIN_NEW_STATE()

                    {
                        fcs_cards_column_t new_temp_col;
                        new_temp_col = fcs_state_get_col(new_state, stack_idx);
                        fcs_col_pop_top(new_temp_col);
                    }

                    fcs_increment_foundation(new_state, deck*4+suit);

                    COMMIT_NEW_STATE(COL2MOVE(stack_idx), FOUND2MOVE(suit))
                }
            }
        }
    }

#define fc_idx stack_idx
    /* Move freecell stack cards to foundations. */
    for (fc_idx=0 ; fc_idx < LOCAL_FREECELLS_NUM ; fc_idx++)
    {
        card = fcs_freecell_card(the_state, fc_idx);
        suit = fcs_card_suit(card);
        if (fcs_card_card_num(card) != 0)
        {
            for(deck=0;deck<INSTANCE_DECKS_NUM;deck++)
            {
                if (fcs_foundation_value(the_state, deck*4+suit) == fcs_card_card_num(card) - 1)
                {
                    BEGIN_NEW_STATE()

                    /* We can put it there */
                    fcs_empty_freecell(new_state, fc_idx);

                    fcs_increment_foundation(new_state, deck*4+suit);

                    COMMIT_NEW_STATE(
                        FREECELL2MOVE(fc_idx), FOUND2MOVE(suit)
                    )
                }
            }
        }
    }
#undef fc_idx

    return FALSE;
}

#undef the_state
#undef new_state

void * instance_run_solver_thread(void * void_arg)
{
    thread_arg_t * arg;
    fcs_bool_t queue_solution_was_found;
    fcs_dbm_solver_thread_t * thread;
    fcs_dbm_solver_instance_t * instance;
    fcs_dbm_queue_item_t * item, * prev_item;
    int queue_num_extracted_and_processed;
    fcs_derived_state_t * derived_list, * derived_list_recycle_bin, 
                        * derived_iter;
    fcs_compact_allocator_t derived_list_allocator;
    fc_solve_delta_stater_t * delta_stater;
    fcs_state_keyval_pair_t state;
#ifdef INDIRECT_STACK_STATES
    dll_ind_buf_t indirect_stacks_buffer;
#endif
    fc_solve_bit_reader_t bit_r;
    fc_solve_bit_writer_t bit_w;

    arg = (thread_arg_t *)void_arg;
    thread = arg->thread;
    instance = thread->instance;
    delta_stater = thread->delta_stater;

    prev_item = NULL;

    fc_solve_compact_allocator_init(&(derived_list_allocator));
    derived_list_recycle_bin = NULL;
    derived_list = NULL;

    while (1)
    {
        /* First of all extract an item. */
        pthread_mutex_lock(&instance->queue_lock);

        if (prev_item)
        {
            instance->queue_num_extracted_and_processed--;
            prev_item->next = instance->queue_recycle_bin;
            instance->queue_recycle_bin = prev_item;
        }

        if (! (queue_solution_was_found = instance->queue_solution_was_found))
        {
            if ((item = instance->queue_head))
            {
                if (!(instance->queue_head = item->next))
                {
                    instance->queue_tail = NULL;
                }
                instance->queue_num_extracted_and_processed++;
            }

            queue_num_extracted_and_processed =
                instance->queue_num_extracted_and_processed;
        }
        pthread_mutex_unlock(&instance->queue_lock);

        if (queue_solution_was_found || (! queue_num_extracted_and_processed))
        {
            break;
        }

        fc_solve_bit_reader_init(&bit_r, item->key + 1);

        fc_solve_state_init(&state, STACKS_NUM
#ifdef INDIRECT_STACK_STATES
            , indirect_stacks_buffer
#endif
        );

        fc_solve_delta_stater_decode(
            delta_stater,
            &bit_r,
            &(state.s)
        );

        if (instance_solver_thread_calc_derived_states(
            &state,
            &(item->key),
            &derived_list,
            &derived_list_recycle_bin,
            &derived_list_allocator
        ))
        {
            pthread_mutex_lock(&instance->queue_lock);
            instance->queue_solution_was_found = TRUE;
            memcpy(&(instance->queue_solution), &(item->key),
                   sizeof(instance->queue_solution));
            pthread_mutex_unlock(&instance->queue_lock);
            break;
        }

        /* Encode all the states. */
        for (derived_iter = derived_list;
                derived_iter ;
                derived_iter = derived_iter->next
        )
        {
            memset(derived_iter->key, '\0', sizeof(derived_iter->key));
            fc_solve_bit_writer_init(&bit_w, derived_iter->key+1);
            fc_solve_delta_stater_set_derived(
                delta_stater, &(derived_iter->state.s)
            );
            fc_solve_delta_stater_encode_composite(delta_stater, &bit_w);
            derived_iter->key[0] =
                bit_w.current - bit_w.start + (bit_w.bit_in_char_idx > 0)
                ;
        }

        instance_check_multiple_keys(instance, derived_list);

        /* Now recycle the derived_list */
        while (derived_list)
        {
#define derived_list_next derived_iter
            derived_list_next = derived_list->next;
            derived_list->next = derived_list_recycle_bin;
            derived_list_recycle_bin = derived_list;
            derived_list = derived_list_next;
#undef derived_list_next
        }
        /* End of main thread loop */
        prev_item = item;
    }

    fc_solve_compact_allocator_finish(&(derived_list_allocator));

    return NULL;
}

/* Temporary main() function to make gcc happy. */
int main(int argc, char * argv[])
{
    printf("%s\n", "Hello");

    return 0;
}
