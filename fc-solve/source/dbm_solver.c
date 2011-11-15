#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <assert.h>

#include "config.h"

#include "bool.h"
#include "inline.h"

#include "dbm_solver.h"

#include "delta_states.c"

#define STACKS_NUM 8
#define LOCAL_STACKS_NUM STACKS_NUM
#define INSTANCE_DECKS_NUM 1
#define LOCAL_FREECELLS_NUM 2
#define FREECELLS_NUM LOCAL_FREECELLS_NUM
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
            if (existing->lower_pri)
            {
                existing->lower_pri->higher_pri =
                    existing->higher_pri;
            }
            else
            {
                cache->lowest_pri = existing->higher_pri;
                /* Bug fix: keep the chain intact. */
                existing->higher_pri->lower_pri = NULL;
            }
            cache->highest_pri->higher_pri = existing;
            existing->lower_pri = cache->highest_pri;
            cache->highest_pri = existing;
            existing->higher_pri = NULL;
        }


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
        cache_key->higher_pri = NULL;
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

static fcs_bool_t GCC_INLINE pre_cache_lookup_parent_and_move(
    fcs_pre_cache_t * pre_cache,
    unsigned char * key,
    unsigned char * parent_and_move
    )
{
    fcs_pre_cache_key_val_pair_t to_check;
    dnode_t * node;

    memcpy(to_check.key, key, sizeof(to_check.key));

    node = fc_solve_kaz_tree_lookup(pre_cache->kaz_tree, &to_check);

    if (node)
    {
        memcpy(
            parent_and_move,
            ((fcs_pre_cache_key_val_pair_t *)(node->dict_key))->parent_and_move,
            sizeof(fcs_encoded_state_buffer_t)
        );
        return TRUE;
    }
    else
    {
        return FALSE;
    }
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
    long count_num_processed;
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
    instance->count_num_processed = 0;
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
        memcpy(&(ptr_new_state->indirect_stacks_buffer[copy_col_idx << 7]), copy_stack_col, fcs_col_len(copy_stack_col)+1); \
        fcs_state_get_col((ptr_new_state->state.s), copy_col_idx) = &(ptr_new_state->indirect_stacks_buffer[copy_col_idx << 7]); \
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
        ] = MAKE_MOVE((src), (dest)); \
 \
    /* Finally, enqueue the new state. */ \
    ptr_new_state->next = (*derived_list); \
    (*derived_list) = ptr_new_state; \
 \
}

static GCC_INLINE int calc_foundation_to_put_card_on(
        fcs_state_t * my_ptr_state,
        fcs_card_t card
        )
{
    int deck;

    for(deck=0;deck < INSTANCE_DECKS_NUM;deck++)
    {
        if (fcs_foundation_value(*my_ptr_state, (deck<<2)+fcs_card_suit(card)) == fcs_card_card_num(card) - 1)
        {
            int other_deck_idx;

            for (other_deck_idx = 0 ; other_deck_idx < (INSTANCE_DECKS_NUM << 2) ; other_deck_idx++)
            {
                if (fcs_foundation_value(*my_ptr_state, other_deck_idx)
                        < fcs_card_card_num(card) - 2 -
                        ((other_deck_idx&0x1) == (fcs_card_suit(card)&0x1))
                   )
                {
                    break;
                }
            }
            if (other_deck_idx == (INSTANCE_DECKS_NUM << 2))
            {
                return (deck<<2)+fcs_card_suit(card);
            }
        }
    }
    return -1;
}

static GCC_INLINE void horne_prune(
    fcs_state_keyval_pair_t * init_state_kv_ptr
)
{
    int stack_idx, fc;
    fcs_cards_column_t col;
    int cards_num;
    int dest_foundation;
    int num_cards_moved;
    fcs_card_t card;

#define the_state (init_state_kv_ptr->s)
    do {
        num_cards_moved = 0;
        for( stack_idx=0 ; stack_idx < LOCAL_STACKS_NUM ; stack_idx++)
        {
            col = fcs_state_get_col(the_state, stack_idx);
            cards_num = fcs_col_len(col);
            if (cards_num)
            {
                /* Get the top card in the stack */
                card = fcs_col_get_card(col, cards_num-1);

                if ((dest_foundation = 
                    calc_foundation_to_put_card_on(&the_state, card)) >= 0)
                {
                    /* We can safely move it. */
                    num_cards_moved++;

                    fcs_col_pop_top(col);

                    fcs_increment_foundation(the_state, dest_foundation);
                }
            }
        }

        /* Now check the same for the free cells */
        for( fc=0 ; fc < LOCAL_FREECELLS_NUM ; fc++)
        {
            card = fcs_freecell_card(the_state, fc);
            if (fcs_card_card_num(card) != 0)
            {
                if ((dest_foundation = 
                    calc_foundation_to_put_card_on(&the_state, card)) >= 0)
                {
                    num_cards_moved++;

                    /* We can put it there */

                    fcs_empty_freecell(the_state, fc);
                    fcs_increment_foundation(the_state, dest_foundation);
                }
            }
        }
    } while (num_cards_moved);
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
    int stack_idx, cards_num, ds;
    fcs_cards_column_t col, dest_col;
#ifdef INDIRECT_STACK_STATES
    fcs_cards_column_t copy_stack_col;
    int copy_col_idx;
#endif
    fcs_card_t card, dest_card;
    int deck, suit;
    int sequences_are_built_by;
    int empty_fc_idx = -1;
    int empty_stack_idx = -1;

    /* TODO : the actual calculation. */
    sequences_are_built_by = FCS_SEQ_BUILT_BY_ALTERNATE_COLOR;

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
        else
        {
            empty_stack_idx = stack_idx;
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
        else
        {
            empty_fc_idx = fc_idx;
        }
    }

    /* Move stack card on top of a parent */
    for (stack_idx=0;stack_idx<LOCAL_STACKS_NUM;stack_idx++)
    {
        col = fcs_state_get_col(the_state, stack_idx);
        cards_num = fcs_col_len(col);
        if (cards_num > 0)
        {
            card = fcs_col_get_card(col, cards_num-1);

            for (ds=0;ds<LOCAL_STACKS_NUM;ds++)
            {
                if (ds == stack_idx)
                {
                    continue;
                }

                dest_col = fcs_state_get_col(the_state, ds);

                if (fcs_col_len(dest_col) > 0)
                {
                    dest_card = fcs_col_get_card(dest_col,
                            fcs_col_len(dest_col)-1);
                    if (fcs_is_parent_card(card, dest_card))
                    {
                        /* Let's move it */
                        BEGIN_NEW_STATE()

                        {
                            fcs_cards_column_t new_src_col;
                            fcs_cards_column_t new_dest_col;

                            new_src_col = fcs_state_get_col(new_state, stack_idx);
                            new_dest_col = fcs_state_get_col(new_state, ds);

                            fcs_col_pop_top(new_src_col);
                            fcs_col_push_card(new_dest_col, card);
                        }
                        
                        COMMIT_NEW_STATE(
                            COL2MOVE(stack_idx), COL2MOVE(ds)
                        )
                    }
                }
            }
        }
    }

    /* Move freecell card on top of a parent */
    for (fc_idx=0 ; fc_idx < LOCAL_FREECELLS_NUM ; fc_idx++)
    {
        card = fcs_freecell_card(the_state, fc_idx);
        if (fcs_card_card_num(card) != 0)
        {
            for (ds=0;ds<LOCAL_STACKS_NUM;ds++)
            {
                dest_col = fcs_state_get_col(the_state, ds);

                if (fcs_col_len(dest_col) > 0)
                {
                    dest_card = fcs_col_get_card(dest_col,
                            fcs_col_len(dest_col)-1);
                    if (fcs_is_parent_card(card, dest_card))
                    {
                        /* Let's move it */
                        BEGIN_NEW_STATE()

                        {
                            fcs_cards_column_t new_dest_col;

                            new_dest_col = fcs_state_get_col(new_state, ds);

                            fcs_col_push_card(new_dest_col, card);

                            fcs_empty_freecell(new_state, fc_idx);
                        }
                        
                        COMMIT_NEW_STATE(
                            FREECELL2MOVE(fc_idx), COL2MOVE(ds)
                        )
                    }
                }
            }
        }
    }

    if (empty_stack_idx >= 0)
    {
        /* Stack Card to Empty Stack */
        for(stack_idx=0;stack_idx<LOCAL_STACKS_NUM;stack_idx++)
        {
            col = fcs_state_get_col(the_state, stack_idx);
            cards_num = fcs_col_len(col);
            /* Bug fix: if there's only one card in a column, there's no 
             * point moving it to a new empty column.
             * */
            if (cards_num > 1)
            {
                card = fcs_col_get_card(col, cards_num-1);
                /* Let's move it */
                {
                    BEGIN_NEW_STATE()

                    {
                        fcs_cards_column_t new_src_col;
                        fcs_cards_column_t empty_stack_col;

                        new_src_col = fcs_state_get_col(new_state, stack_idx);

                        fcs_col_pop_top(new_src_col);

                        empty_stack_col = fcs_state_get_col(new_state, empty_stack_idx);
                        fcs_col_push_card(empty_stack_col, card);
                    }
                    COMMIT_NEW_STATE(
                        COL2MOVE(stack_idx), COL2MOVE(empty_stack_idx)
                    )
                }
            }
        }

        /* Freecell card -> Empty Stack. */
        for (fc_idx=0;fc_idx<LOCAL_FREECELLS_NUM;fc_idx++)
        {
            card = fcs_freecell_card(the_state, fc_idx);
            if (fcs_card_card_num(card) == 0)
            {
                continue;
            }

            {
                BEGIN_NEW_STATE()

                {
                    fcs_cards_column_t new_dest_col;
                    new_dest_col = fcs_state_get_col(new_state, empty_stack_idx);
                    fcs_col_push_card(new_dest_col, card);
                    fcs_empty_freecell(new_state, fc_idx);
                }

                COMMIT_NEW_STATE(
                    FREECELL2MOVE(fc_idx), COL2MOVE(empty_stack_idx)
                );
            }
        }
    }

    if (empty_fc_idx >= 0)
    {
        /* Stack Card to Empty Freecell */
        for (stack_idx=0;stack_idx<LOCAL_STACKS_NUM;stack_idx++)
        {
            col = fcs_state_get_col(the_state, stack_idx);
            cards_num = fcs_col_len(col);
            if (cards_num > 0)
            {
                card = fcs_col_get_card(col, cards_num-1);
                /* Let's move it */
                {
                    BEGIN_NEW_STATE()

                    {
                        fcs_cards_column_t new_src_col;

                        new_src_col = fcs_state_get_col(new_state, stack_idx);

                        fcs_col_pop_top(new_src_col);

                        fcs_put_card_in_freecell(new_state, empty_fc_idx, card);
                    }
                    COMMIT_NEW_STATE(
                        COL2MOVE(stack_idx), FREECELL2MOVE(empty_fc_idx)
                    )
                }
            }
        }
    }
#undef fc_idx

    /* Perform Horne's Prune on all the states. */
    {
        fcs_derived_state_t * derived_iter;

        for (derived_iter = (*derived_list);
            derived_iter ;
            derived_iter = derived_iter->next
        )
        {
            horne_prune(&(derived_iter->state));
        }
    }

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

    prev_item = item = NULL;
    queue_num_extracted_and_processed = 0;

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
                if (++instance->count_num_processed % 100000 == 0)
                {
                    printf ("Reached %ld.\n", instance->count_num_processed);
                    fflush(stdout);
                }
            }

            queue_num_extracted_and_processed =
                instance->queue_num_extracted_and_processed;
        }
        pthread_mutex_unlock(&instance->queue_lock);

        if (queue_solution_was_found || (! queue_num_extracted_and_processed))
        {
            break;
        }

        if (! item)
        {
            /* Sleep until more items become available in the
             * queue. */
            usleep(5000);
        }
        else
        {
        /* Handle item. */
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
        /* End handle item. */
        }
        /* End of main thread loop */
        prev_item = item;
    }

    fc_solve_compact_allocator_finish(&(derived_list_allocator));

    return NULL;
}

typedef struct {
    fcs_dbm_solver_thread_t thread;
    thread_arg_t arg;
    pthread_t id;
} main_thread_item_t;

#define USER_STATE_SIZE 2000

const char * move_to_string(unsigned char move, char * move_buffer)
{
    int iter, inspect;
    char * s;

    s = move_buffer;

    for (iter=0 ; iter < 2 ; iter++)
    {
        inspect = (move & 0xF);
        move >>= 4;
        
        if (inspect < 8)
        {
            s += sprintf(s, "Column %d", inspect);
        }
        else
        {
            inspect -= 8;
            if (inspect < 4)
            {
                s += sprintf(s, "Freecell %d", inspect);
            }
            else
            {
                inspect -= 4;
                s += sprintf(s, "Foundation %d", inspect);
            }
        }
        if (iter == 0)
        {
            s += sprintf(s, "%s", " -> ");
        }
    }
    
    return move_buffer;
}
/* Temporary main() function to make gcc happy. */
int main(int argc, char * argv[])
{
    fcs_dbm_solver_instance_t instance;
    long pre_cache_max_count;
    long caches_delta;
    const char * dbm_store_path;
    int num_threads;
    int arg;
    const char * filename;
    FILE * fh;
    char user_state[USER_STATE_SIZE];
    fc_solve_delta_stater_t * delta;
    fcs_state_keyval_pair_t init_state;
#ifdef INDIRECT_STACK_STATES
    dll_ind_buf_t init_indirect_stacks_buffer;
#endif
    fc_solve_bit_writer_t bit_w;

    pre_cache_max_count = 1000000;
    caches_delta = 1000000;
    dbm_store_path = "./fc_solve_dbm_store";
    num_threads = 2;

    for (arg=1;arg < argc; arg++)
    {
        if (!strcmp(argv[arg], "--pre-cache-max-count"))
        {
            arg++;
            if (arg == argc)
            {
                fprintf(stderr, "--pre-cache-max-count came without an argument!\n");
                exit(-1);
            }
            pre_cache_max_count = atol(argv[arg]);
            if (pre_cache_max_count < 1000)
            {
                fprintf(stderr, "--pre-cache-max-count must be at least 1,000.\n");
                exit(-1);
            }
        }
        else if (!strcmp(argv[arg], "--caches-delta"))
        {
            arg++;
            if (arg == argc)
            {
                fprintf(stderr, "--caches-delta came without an argument!\n");
                exit(-1);
            }
            caches_delta = atol(argv[arg]);
            if (caches_delta < 1000)
            {
                fprintf(stderr, "--caches-delta must be at least 1,000.\n");
                exit(-1);
            }
        }
        else if (!strcmp(argv[arg], "--num-threads"))
        {
            arg++;
            if (arg == argc)
            {
                fprintf(stderr, "--num-threads came without an argument!\n");
                exit(-1);
            }
            num_threads = atoi(argv[arg]);
            if (num_threads < 1)
            {
                fprintf(stderr, "--num-threads must be at least 1.\n");
                exit(-1);
            }
        }
        else if (!strcmp(argv[arg], "--dbm-store-path"))
        {
            arg++;
            if (arg == argc)
            {
                fprintf(stderr, "--dbm-store-path came without an argument.\n");
                exit(-1);
            }
            dbm_store_path = argv[arg];
        }
        else
        {
            break;
        }
    }

    if (arg < argc-1)
    {
        fprintf (stderr, "%s\n", "Junk arguments!");
        exit(-1);
    }
    else if (arg == argc)
    {
        fprintf (stderr, "%s\n", "No board specified.");
        exit(-1);
    }

    filename = argv[arg];

    instance_init(&instance, pre_cache_max_count, caches_delta, dbm_store_path);
    fh = fopen(filename, "r");
    if (fh == NULL)
    {
        fprintf (stderr, "Could not open file '%s' for input.\n", filename);
        exit(-1);
    }
    memset(user_state, '\0', sizeof(user_state));
    fread(user_state, sizeof(user_state[0]), USER_STATE_SIZE-1, fh);
    fclose(fh);

    fc_solve_initial_user_state_to_c(
        user_state,
        &init_state,
        FREECELLS_NUM,
        STACKS_NUM,
        DECKS_NUM
#ifdef INDIRECT_STACK_STATES
        , init_indirect_stacks_buffer
#endif
    );

    horne_prune(&init_state);

    delta = fc_solve_delta_stater_alloc(
            &init_state.s,
            STACKS_NUM,
            FREECELLS_NUM
#ifndef FCS_FREECELL_ONLY
            , FCS_SEQ_BUILT_BY_ALTERNATE_COLOR
#endif
    );

    fc_solve_delta_stater_set_derived(delta, &(init_state.s));
    
    {
        fcs_dbm_queue_item_t * first_item;
        fcs_encoded_state_buffer_t parent_and_move;

        first_item =
            (fcs_dbm_queue_item_t *)
                fcs_compact_alloc_ptr(
                    &(instance.queue_allocator),
                    sizeof(*first_item)
                );

        first_item->next = NULL;
        memset(first_item->key, '\0', sizeof(first_item->key));

        fc_solve_bit_writer_init(&bit_w, first_item->key+1);
        fc_solve_delta_stater_encode_composite(delta, &bit_w);
        first_item->key[0] =
            bit_w.current - bit_w.start + (bit_w.bit_in_char_idx > 0)
            ;

        /* The NULL parent and move for indicating this is the initial
         * state. */
        memset(parent_and_move, '\0', sizeof(parent_and_move));

        pre_cache_insert(&(instance.pre_cache), first_item->key, parent_and_move);
        instance.queue_head = instance.queue_tail = first_item;
    }
    {
        int i, check;
        main_thread_item_t * threads;

        threads = malloc(sizeof(threads[0]) * num_threads);

        for (i=0; i < num_threads ; i++)
        {
            threads[i].thread.instance = &(instance);
            threads[i].thread.delta_stater =
                fc_solve_delta_stater_alloc(
                    &(init_state.s),
                    STACKS_NUM,
                    FREECELLS_NUM
#ifndef FCS_FREECELL_ONLY
                    , FCS_SEQ_BUILT_BY_ALTERNATE_COLOR
#endif
                );
            threads[i].arg.thread = &(threads[i].thread);
            check = pthread_create(
                &(threads[i].id),
                NULL,
                instance_run_solver_thread,
                &(threads[i].arg)
            );

            if (check)
            {
                fprintf(stderr,
                        "Worker Thread No. %d Initialization failed!\n",
                        i
                       );
                exit(-1);
            }
        }

        for (i=0; i < num_threads ; i++)
        {
            pthread_join(threads[i].id, NULL);
            fc_solve_delta_stater_free(threads[i].thread.delta_stater);
        }
        free(threads);
    }

    if (instance.queue_solution_was_found)
    {
        fcs_encoded_state_buffer_t * trace;
        fcs_encoded_state_buffer_t key;
        int trace_num, trace_max_num;
        int i;
        fcs_state_keyval_pair_t state;
        fc_solve_bit_reader_t bit_r;
        unsigned char move;
        char * state_as_str;
        char move_buffer[500];
#ifdef INDIRECT_STACK_STATES
        dll_ind_buf_t indirect_stacks_buffer;
#endif
#ifdef FCS_WITHOUT_LOCS_FIELDS
        fcs_state_locs_struct_t locs;
#endif
        printf ("%s\n", "Success!");
        /* Now trace the solution */
#define GROW_BY 100
        trace_num = 0;
        trace = malloc(sizeof(trace[0]) * (trace_max_num = GROW_BY));
        memcpy(trace[trace_num], instance.queue_solution,
               sizeof(trace[0]));
        while (trace[trace_num][0])
        {
            memset(key, '\0', sizeof(key));
            /* Omit the move. */
            memcpy(key, trace[trace_num], trace[trace_num][0]+1);

            if ((++trace_num) == trace_max_num)
            {
                trace = realloc(trace, sizeof(trace[0]) * (trace_max_num += GROW_BY));
            }
            if (! pre_cache_lookup_parent_and_move(
                &(instance.pre_cache),
                key,
                trace[trace_num]
                ))
            {
                assert(fc_solve_dbm_store_lookup_parent_and_move(
                    instance.store,
                    key,
                    trace[trace_num]
                    ));
            }
        }
#ifdef FCS_WITHOUT_LOCS_FIELDS
        for (i=0 ; i < MAX_NUM_STACKS ; i++)
        {
            locs.stack_locs[i] = i;
        }
        for (i=0 ; i < MAX_NUM_FREECELLS ; i++)
        {
            locs.fc_locs[i] = i;
        }
#endif
        for (i = trace_num-1 ; i >= 0; i--)
        {
            fc_solve_state_init(&state, STACKS_NUM
#ifdef INDIRECT_STACK_STATES
                , indirect_stacks_buffer
#endif
            );

            fc_solve_bit_reader_init(&bit_r, &(trace[i][1]));
            fc_solve_delta_stater_decode(
                delta,
                &bit_r,
                &(state.s)
            );
            if (i > 0)
            {
                move = trace[i][1+trace[i][0]];
            }

            state_as_str =
                fc_solve_state_as_string(
#ifdef FCS_RCS_STATES
                        &(state.s),
                        &(state.info),
#else
                        &state,
#endif
#ifdef FCS_WITHOUT_LOCS_FIELDS
                        &locs,
#endif
                        FREECELLS_NUM,
                        STACKS_NUM,
                        DECKS_NUM,
                        1,
                        0,
                        1
                );

            printf("--------\n%s\n==\n%s\n",
                    state_as_str,
                    (i > 0 )
                        ? move_to_string(move, move_buffer)
                        : "END"
                  );

            free(state_as_str);
        }
        free (trace);
    }
    else
    {
        printf ("%s\n", "Could not solve successfully.");
    }
    
    instance_destroy(&instance);

    return 0;
}
