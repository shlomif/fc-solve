#include <stdio.h>

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

/* Temporary main() function to make gcc happy. */
int main(int argc, char * argv[])
{
    printf("%s\n", "Hello");

    return 0;
}
