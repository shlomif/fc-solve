
void fc_solve_hash_free_with_callback(
    SFO_hash_t * hash,
    void (*function_ptr)(void * key, void * context)
    )
{
    int i;
    SFO_hash_symlink_item_t * item, * next_item;

    for(i=0;i<hash->size;i++)
    {
        item = hash->entries[i].first_item;
        while (item != NULL)
        {
            function_ptr(item->key, hash->context);
            next_item = item->next;

            item = next_item;
        }
    }

    fc_solve_hash_free(hash);
}

