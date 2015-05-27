
#if 0
#include <functional>

namespace kazlib
{
    class dnode : public dnode_t {
    public:
        dnode() { dnode_init(this, 0); }
        dnode(const dnode &) { dnode_init(this, 0); }
        dnode &operator = (const dnode &) { return *this; }
        bool is_in_a_dict()
        {
            return dnode_is_in_a_dict(this);
        }
    };

    template <typename KEY>
    int default_compare(const KEY &left, const KEY &right)
    {
        if (left < right)
            return -1;
        else if (left == right)
            return 0;
        else
            return 1;
    }

    template <typename KEY, int (*COMP)(const KEY &, const KEY &)>
    struct compare_with_function {
        static int compare(const void *left, const void *right)
        {
            return COMP(*(const KEY *) left, *(const KEY *) right);
        }
    };

    struct dupes_allowed {
        static const int allowed = 1;
    };

    struct dupes_disallowed {
        static const int allowed = 0;
    };

    template <class ITEM, typename KEY, KEY ITEM::* KEY_OFFSET>
    struct key_is_member {
        typedef KEY KEY_TYPE;
        static void *item2key(ITEM *item)
        {
            return &(item->*KEY_OFFSET);
        }
    };

    template <class ITEM>
    struct key_is_base {
        typedef ITEM KEY_TYPE;
        static void *item2key(ITEM *item)
        {
            return item;
        }
    };

    template <class ITEM, dnode ITEM::* DNODE_OFFSET>
    class dnode_is_member {
    private:
        dnode_is_member(const dnode_is_member &);
        void operator = (const dnode_is_member &);
    public:
        typedef ITEM ITEM_TYPE;
        static ITEM *dnode2item(dnode_t *node)
        {
            if (node == 0)
                return 0;
            const ptrdiff_t offset = (char *) &(((ITEM *) 0)->*DNODE_OFFSET)
                                     - ((char *) 0);
            return (ITEM *) (((char *) node) - offset);
        }
        static dnode_t *item2dnode(ITEM *item)
        {
            return &(item->*DNODE_OFFSET);
        }
    };

    template <class ITEM>
    class dnode_is_base {
    private:
        dnode_is_base(const dnode_is_base &);
        void operator = (const dnode_is_base &);
    public:
        typedef ITEM ITEM_TYPE;
        static ITEM *dnode2item(dnode_t *node)
        {
            return static_cast<ITEM *>(node);
        }
        static dnode_t *item2dnode(ITEM *item)
        {
            return item;
        }
    };

    struct static_items {
        template <class CONTAINER>
        static void delete_all(CONTAINER &)
        {
        }
        template <class ITEM>
        static void delete_item(ITEM *)
        {
        }
    };

    struct dynamic_items {
        template <class CONTAINER>
        static void delete_all(CONTAINER &c)
        {
            c.delete_all();
        }
        template <class ITEM>
        static void delete_item(ITEM *item)
        {
            delete item;
        }
    };

    struct placement_items {
        template <class CONTAINER>
        static void delete_all(CONTAINER &c)
        {
            c.delete_all();
        }
        template <class ITEM>
        static void delete_item(ITEM *item)
        {
            item->~ITEM();
        }
    };

    struct dict_dfl_feat {
        typedef dupes_disallowed dupe_feature;
        typedef static_items alloc_feature;
    };

    template <typename FIRST, typename REST>
    struct trait_combinator : public FIRST, public REST {
    };

    template <typename REST, typename KEY, int (*F)(const KEY&, const KEY &)>
    struct trait_combinator<compare_with_function<KEY, F>, REST>
    : public REST {
        typedef compare_with_function<KEY, F> compare_feature;
    };

    template <typename REST>
    struct trait_combinator<dupes_allowed, REST> : public REST {
        typedef dupes_allowed dupe_feature;
    };

    template <typename REST>
    struct trait_combinator<dupes_disallowed, REST> : public REST {
        typedef dupes_disallowed dupe_feature;
    };

    template <typename REST, class ITEM, typename KEY, KEY ITEM::* KO>
    struct trait_combinator<key_is_member<ITEM, KEY, KO>, REST> : public REST {
        typedef key_is_member<ITEM, KEY, KO> key_feature;
        typedef compare_with_function<KEY, default_compare> compare_feature;
    };

    template <typename REST, typename KEY>
    struct trait_combinator<key_is_base<KEY>, REST> : public REST {
        typedef key_is_base<KEY> key_feature;
        typedef compare_with_function<KEY, default_compare> compare_feature;
    };

    template <typename REST, class ITEM, dnode ITEM::* DO>
    struct trait_combinator<dnode_is_member<ITEM, DO>, REST> : public REST {
        typedef dnode_is_member<ITEM, DO> dnode_feature;
    };

    template <typename REST, class ITEM>
    struct trait_combinator<dnode_is_base<ITEM>, REST> : public REST {
        typedef dnode_is_base<ITEM> dnode_feature;
    };

    template <typename REST>
    struct trait_combinator<static_items, REST> : public REST {
        typedef static_items alloc_feature;
    };

    template <typename REST>
    struct trait_combinator<dynamic_items, REST> : public REST {
        typedef dynamic_items alloc_feature;
    };

    template <typename REST>
    struct trait_combinator<placement_items, REST> : public REST {
        typedef placement_items alloc_feature;
    };

    template <typename T1 = dict_dfl_feat,
              typename T2 = dict_dfl_feat,
              typename T3 = dict_dfl_feat,
              typename T4 = dict_dfl_feat,
              typename T5 = dict_dfl_feat>
    struct traits
    : public trait_combinator<T5, traits<T4, T2, T2, T1> >
    {
    };

    template <>
    struct traits<dict_dfl_feat, dict_dfl_feat, dict_dfl_feat,
                  dict_dfl_feat, dict_dfl_feat>
    : public dict_dfl_feat
    {
    };

    template <typename T1>
    struct traits<T1, dict_dfl_feat, dict_dfl_feat,
                  dict_dfl_feat, dict_dfl_feat>
    : public trait_combinator<T1, dict_dfl_feat>
    {
    };

    template <typename T1, typename T2>
    struct traits<T1, T2, dict_dfl_feat, dict_dfl_feat, dict_dfl_feat>
    : public trait_combinator<T2, traits<T1> >
    {
    };

    template <typename T1, typename T2, typename T3>
    struct traits<T1, T2, T3, dict_dfl_feat, dict_dfl_feat>
    : public trait_combinator<T3, traits<T1, T2> >
    {
    };

    template <typename T1, typename T2, typename T3, typename T4>
    struct traits<T1, T2, T3, T4, dict_dfl_feat>
    : public trait_combinator<T4, traits<T1, T2, T3> >
    {
    };

    template <
        typename TRAIT1 = dict_dfl_feat,
        typename TRAIT2 = dict_dfl_feat,
        typename TRAIT3 = dict_dfl_feat,
        typename TRAIT4 = dict_dfl_feat,
        typename TRAIT5 = dict_dfl_feat
    >
    class dict : public dict_t
    {
    private:
        dict(const dict &);
        void operator = (const dict &);
    protected:
        typedef traits<TRAIT1, TRAIT2, TRAIT3, TRAIT4, TRAIT5> tr;
        typedef typename tr::key_feature::KEY_TYPE KEY;
        typedef typename tr::dnode_feature::ITEM_TYPE ITEM;

        static dnode_t *item2dnode(ITEM *item)
        {
            return tr::dnode_feature::item2dnode(item);
        }
        static ITEM *dnode2item(dnode_t *dnode)
        {
            return tr::dnode_feature::dnode2item(dnode);
        }
        static void *item2key(ITEM *item)
        {
            return tr::key_feature::item2key(item);
        }
        static void delete_item(dnode_t *dnode, void *)
        {
            ITEM *item = tr::dnode_feature::dnode2item(dnode);
            tr::alloc_feature::delete_item(item);
        }
    public:
        dict(dictcount_t count = DICTCOUNT_T_MAX)
        {
            dict_init_alloc(this, count, tr::compare_feature::compare, 0,
                            delete_item, 0);
            if (tr::dupe_feature::allowed)
                dict_allow_dupes(this);
        }
        ~dict()
        {
            tr::alloc_feature::delete_all(*this);
        }
        dictcount_t count()
        {
            return dict_count(this);
        }
        ITEM *insert(ITEM *pitem)
        {
            dict_insert(this, item2dnode(pitem), item2key(pitem));
            return pitem;
        }
        ITEM &insert(ITEM &item)
        {
            return *insert(&item);
        }
        ITEM *erase(ITEM *pitem)
        {
            dict_delete(this, item2dnode(pitem));
            return pitem;
        }
        ITEM &erase(ITEM &item)
        {
            return *erase(&item);
        }
        void delete_all()
        {
            dict_free(this);
        }
        ITEM *lookup(const KEY *pkey)
        {
            return dnode2item(dict_lookup(this, pkey));
        }
        ITEM *lookup(const KEY &key)
        {
            return lookup(&key);
        }
        ITEM *upper_bound(const KEY *pkey)
        {
            return dnode2item(dict_upper_bound(this, pkey));
        }
        ITEM *upper_bound(const KEY &key)
        {
            return upper_bound(&key);
        }
        ITEM *lower_bound(const KEY *pkey)
        {
            return dnode2item(dict_lower_bound(this, pkey));
        }
        ITEM *lower_bound(const KEY &key)
        {
            return lower_bound(&key);
        }
        ITEM *first()
        {
            return dnode2item(dict_first(this));
        }
        ITEM *last()
        {
            return dnode2item(dict_last(this));
        }
        ITEM *next(ITEM *pitem)
        {
            return dnode2item(dict_next(this, item2dnode(pitem)));
        }
        ITEM *next(ITEM &item)
        {
            return next(&item);
        }
        ITEM *prev(ITEM *pitem)
        {
            return dnode2item(dict_prev(this, item2dnode(pitem)));
        }
        ITEM *prev(ITEM &item)
        {
            return prev(&item);
        }
    };
}

#endif

#ifdef KAZLIB_TEST_MAIN

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdarg.h>

typedef char input_t[256];

static int tokenize(char *string, ...)
{
    char **tokptr;
    va_list arglist;
    int tokcount = 0;

    va_start(arglist, string);
    tokptr = va_arg(arglist, char **);
    while (tokptr) {
        while (*string && isspace((unsigned char) *string))
            string++;
        if (!*string)
            break;
        *tokptr = string;
        while (*string && !isspace((unsigned char) *string))
            string++;
        tokptr = va_arg(arglist, char **);
        tokcount++;
        if (!*string)
            break;
        *string++ = 0;
    }
    va_end(arglist);

    return tokcount;
}

static int comparef(const void *key1, const void *key2)
{
    return strcmp((const char *) key1, (const char *) key2);
}

static dnode_t *new_node(void *c)
{
    static dnode_t few[5];
    static int count;

    if (count < 5)
        return few + count++;

    return NULL;
}

static void del_node(dnode_t *n, void *c)
{
}

static int prompt = 0;

static void construct(dict_t *d)
{
    input_t in;
    int done = 0;
    dict_load_t dl;
    dnode_t *dn;
    char *tok1, *tok2, *val;
    const char *key;
    char *help =
        "p                      turn prompt on\n"
        "q                      finish construction\n"
        "a <key> <val>          add new entry\n";

    if (!dict_isempty(d))
        puts("warning: dictionary not empty!");

    dict_load_begin(&dl, d);

    while (!done) {
        if (prompt)
            putchar('>');
        fflush(stdout);

        if (!fgets(in, sizeof(input_t), stdin))
            break;

        switch (in[0]) {
            case '?':
                puts(help);
                break;
            case 'p':
                prompt = 1;
                break;
            case 'q':
                done = 1;
                break;
            case 'a':
                if (tokenize(in+1, &tok1, &tok2, (char **) 0) != 2) {
                    puts("what?");
                    break;
                }
                key = strdup(tok1);
                val = strdup(tok2);
                dn = dnode_create(val);

                if (!key || !val || !dn) {
                    puts("out of memory");
                    free((void *) key);
                    free(val);
                    if (dn)
                        dnode_destroy(dn);
                }

                dict_load_next(&dl, dn, key);
                break;
            default:
                putchar('?');
                putchar('\n');
                break;
        }
    }

    dict_load_end(&dl);
}

int main(void)
{
    input_t in;
    dict_t darray[10];
    dict_t *d = &darray[0];
    dnode_t *dn;
    size_t i;
    char *tok1, *tok2, *val;
    const char *key;

    char *help =
        "a <key> <val>          add value to dictionary\n"
        "d <key>                delete value from dictionary\n"
        "l <key>                lookup value in dictionary\n"
        "( <key>                lookup lower bound\n"
        ") <key>                lookup upper bound\n"
        "< <key>                lookup strict lower bound\n"
        "> <key>                lookup strict upper bound\n"
        "# <num>                switch to alternate dictionary (0-9)\n"
        "j <num> <num>          merge two dictionaries\n"
        "f                      free the whole dictionary\n"
        "k                      allow duplicate keys\n"
        "c                      show number of entries\n"
        "t                      dump whole dictionary in sort order\n"
        "m                      make dictionary out of sorted items\n"
        "p                      turn prompt on\n"
        "s                      switch to non-functioning allocator\n"
        "q                      quit";

    for (i = 0; i < sizeof darray / sizeof *darray; i++)
        dict_init(&darray[i], DICTCOUNT_T_MAX, comparef);

    for (;;) {
        if (prompt)
            putchar('>');
        fflush(stdout);

        if (!fgets(in, sizeof(input_t), stdin))
            break;

        switch(in[0]) {
            case '?':
                puts(help);
                break;
            case 'a':
                if (tokenize(in+1, &tok1, &tok2, (char **) 0) != 2) {
                    puts("what?");
                    break;
                }
                key = strdup(tok1);
                val = strdup(tok2);

                if (!key || !val) {
                    puts("out of memory");
                    free((void *) key);
                    free(val);
                }

                if (!dict_alloc_insert(d, key, val)) {
                    puts("dict_alloc_insert failed");
                    free((void *) key);
                    free(val);
                    break;
                }
                break;
            case 'd':
                if (tokenize(in+1, &tok1, (char **) 0) != 1) {
                    puts("what?");
                    break;
                }
                dn = dict_lookup(d, tok1);
                if (!dn) {
                    puts("dict_lookup failed");
                    break;
                }
                val = (char *) dnode_get(dn);
                key = (char *) dnode_getkey(dn);
                dict_delete_free(d, dn);

                free(val);
                free((void *) key);
                break;
            case 'f':
                dict_free_nodes(d);
                break;
            case 'l':
            case '(':
            case ')':
            case '<':
            case '>':
                if (tokenize(in+1, &tok1, (char **) 0) != 1) {
                    puts("what?");
                    break;
                }
                dn = 0;
                switch (in[0]) {
                case 'l':
                    dn = dict_lookup(d, tok1);
                    break;
                case '(':
                    dn = dict_lower_bound(d, tok1);
                    break;
                case ')':
                    dn = dict_upper_bound(d, tok1);
                    break;
                case '<':
                    dn = dict_strict_lower_bound(d, tok1);
                    break;
                case '>':
                    dn = dict_strict_upper_bound(d, tok1);
                    break;
                }
                if (!dn) {
                    puts("lookup failed");
                    break;
                }
                val = (char *) dnode_get(dn);
                puts(val);
                break;
            case 'm':
                construct(d);
                break;
            case 'k':
                dict_allow_dupes(d);
                break;
            case 'c':
                printf("%lu\n", (unsigned long) dict_count(d));
                break;
            case 't':
                for (dn = fc_solve_kaz_tree_first(d); dn; dn = dict_next(d, dn)) {
                    printf("%s\t%s\n", (char *) dnode_getkey(dn),
                            (char *) dnode_get(dn));
                }
                break;
            case 'q':
                exit(0);
                break;
            case '\0':
                break;
            case 'p':
                prompt = 1;
                break;
            case 's':
                dict_set_allocator(d, new_node, del_node, NULL);
                break;
            case '#':
                if (tokenize(in+1, &tok1, (char **) 0) != 1) {
                    puts("what?");
                    break;
                } else {
                    int dictnum = atoi(tok1);
                    if (dictnum < 0 || dictnum > 9) {
                        puts("invalid number");
                        break;
                    }
                    d = &darray[dictnum];
                }
                break;
            case 'j':
                if (tokenize(in+1, &tok1, &tok2, (char **) 0) != 2) {
                    puts("what?");
                    break;
                } else {
                    int dict1 = atoi(tok1), dict2 = atoi(tok2);
                    if (dict1 < 0 || dict1 > 9 || dict2 < 0 || dict2 > 9) {
                        puts("invalid number");
                        break;
                    }
                    dict_merge(&darray[dict1], &darray[dict2]);
                }
                break;
            default:
                putchar('?');
                putchar('\n');
                break;
        }
    }

    return 0;
}

#endif
