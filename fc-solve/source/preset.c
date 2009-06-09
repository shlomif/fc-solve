/* Copyright (c) 2000 Shlomi Fish
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use,
 * copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following
 * conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 */
/*
 * preset.c - game presets management for Freecell Solver
 *
 */


#include <string.h>
#include <stdlib.h>

#include "instance.h"
#include "preset.h"

enum
{
    FCS_PRESET_BAKERS_DOZEN,
    FCS_PRESET_BAKERS_GAME,
    FCS_PRESET_CRUEL,
    FCS_PRESET_DER_KATZENSCHWANZ,
    FCS_PRESET_DIE_SCHLANGE,
    FCS_PRESET_EIGHT_OFF,
    FCS_PRESET_FAN,
    FCS_PRESET_FORECELL,
    FCS_PRESET_FREECELL,
    FCS_PRESET_GOOD_MEASURE,
    FCS_PRESET_KINGS_ONLY_BAKERS_GAME,
    FCS_PRESET_RELAXED_FREECELL,
    FCS_PRESET_RELAXED_SEAHAVEN_TOWERS,
    FCS_PRESET_SEAHAVEN_TOWERS,
    FCS_PRESET_SIMPLE_SIMON,
    FCS_PRESET_YUKON,
    FCS_PRESET_BELEAGUERED_CASTLE
};

static const fcs_preset_t fcs_presets[16] =
{
    {
        FCS_PRESET_BAKERS_DOZEN,
        0,
        13,
        1,

        FCS_SEQ_BUILT_BY_RANK,
        0,
        FCS_ES_FILLED_BY_NONE,

        "0123456789",
        "0123456789",
    },
    {
        FCS_PRESET_BAKERS_GAME,
        4,
        8,
        1,

        FCS_SEQ_BUILT_BY_SUIT,
        0,
        FCS_ES_FILLED_BY_ANY_CARD,

        "[01][23456789]",
        "0123456789",
    },
    {
        FCS_PRESET_BELEAGUERED_CASTLE,
        0,
        8,
        1,

        FCS_SEQ_BUILT_BY_RANK,
        0,
        FCS_ES_FILLED_BY_ANY_CARD,

        "[01][23456789]",
        "0123456789",
    },
    {
        FCS_PRESET_CRUEL,
        0,
        12,
        1,

        FCS_SEQ_BUILT_BY_SUIT,
        0,
        FCS_ES_FILLED_BY_NONE,

        "0123456789",
        "0123456789",
    },
    {
        FCS_PRESET_DER_KATZENSCHWANZ,
        8,
        9,
        2,

        FCS_SEQ_BUILT_BY_ALTERNATE_COLOR,
        1,
        FCS_ES_FILLED_BY_NONE,

        "[01][23456789]",
        "0123456789",
    },
    {
        FCS_PRESET_DIE_SCHLANGE,
        8,
        9,
        2,

        FCS_SEQ_BUILT_BY_ALTERNATE_COLOR,
        0,
        FCS_ES_FILLED_BY_NONE,

        "[01][23456789]",
        "0123456789",
    },
    {
        FCS_PRESET_EIGHT_OFF,
        8,
        8,
        1,

        FCS_SEQ_BUILT_BY_SUIT,
        0,
        FCS_ES_FILLED_BY_KINGS_ONLY,

        "[01][23456789]",
        "0123456789",
    },
    {
        FCS_PRESET_FAN,
        0,
        18,
        1,

        FCS_SEQ_BUILT_BY_SUIT,
        0,
        FCS_ES_FILLED_BY_KINGS_ONLY,

        "[01][23456789]",
        "0123456789",
    },
    {
        FCS_PRESET_FORECELL,
        4,
        8,
        1,

        FCS_SEQ_BUILT_BY_ALTERNATE_COLOR,
        0,
        FCS_ES_FILLED_BY_KINGS_ONLY,

        "[01][23456789]",
        "0123456789",
    },
    {
        FCS_PRESET_FREECELL,
        4,
        8,
        1,

        FCS_SEQ_BUILT_BY_ALTERNATE_COLOR,
        0,
        FCS_ES_FILLED_BY_ANY_CARD,

        "[01][23456789]",
        "0123456789",
    },
    {
        FCS_PRESET_GOOD_MEASURE,
        0,
        10,
        1,

        FCS_SEQ_BUILT_BY_RANK,
        0,
        FCS_ES_FILLED_BY_NONE,

        "0123456789",
        "0123456789",
    },
    {
        FCS_PRESET_KINGS_ONLY_BAKERS_GAME,
        4,
        8,
        1,

        FCS_SEQ_BUILT_BY_SUIT,
        0,
        FCS_ES_FILLED_BY_KINGS_ONLY,

        "[01][23456789]",
        "0123456789",
    },
    {
        FCS_PRESET_RELAXED_FREECELL,
        4,
        8,
        1,

        FCS_SEQ_BUILT_BY_ALTERNATE_COLOR,
        1,
        FCS_ES_FILLED_BY_ANY_CARD,

        "[01][23456789]",
        "0123456789",
    },
    {
        FCS_PRESET_RELAXED_SEAHAVEN_TOWERS,
        4,
        10,
        1,

        FCS_SEQ_BUILT_BY_SUIT,
        1,
        FCS_ES_FILLED_BY_KINGS_ONLY,

        "[01][23456789]",
        "0123456789",
    },
    {
        FCS_PRESET_SEAHAVEN_TOWERS,
        4,
        10,
        1,

        FCS_SEQ_BUILT_BY_SUIT,
        0,
        FCS_ES_FILLED_BY_KINGS_ONLY,

        "[01][23456789]",
        "0123456789",
    },
    {
        FCS_PRESET_SIMPLE_SIMON,
        0,
        10,
        1,

        FCS_SEQ_BUILT_BY_SUIT,
        0,
        FCS_ES_FILLED_BY_ANY_CARD,

        "abcdefgh",
        "abcdefgh",
    }
};

struct fcs_preset_name_struct
{
    const char name[32];
    int preset_id;
};

typedef struct fcs_preset_name_struct fcs_preset_name_t;

static const fcs_preset_name_t fcs_preset_names[23] =
{
    {
        "bakers_dozen",
        FCS_PRESET_BAKERS_DOZEN,
    },
    {
        "bakers_game",
        FCS_PRESET_BAKERS_GAME,
    },
    {
        "beleaguered_castle",
        FCS_PRESET_BELEAGUERED_CASTLE,
    },
    {
        "citadel",
        FCS_PRESET_BELEAGUERED_CASTLE,
    },
    {
        "cruel",
        FCS_PRESET_CRUEL,
    },
    {
        "der_katzenschwanz",
        FCS_PRESET_DER_KATZENSCHWANZ,
    },
    {
        "der_katz",
        FCS_PRESET_DER_KATZENSCHWANZ,
    },
    {
        "die_schlange",
        FCS_PRESET_DIE_SCHLANGE,
    },
    {
        "eight_off",
        FCS_PRESET_EIGHT_OFF,
    },
    {
        "fan",
        FCS_PRESET_FAN,
    },
    {
        "forecell",
        FCS_PRESET_FORECELL,
    },
    {
        "freecell",
        FCS_PRESET_FREECELL,
    },
    {
        "good_measure",
        FCS_PRESET_GOOD_MEASURE,
    },
    {
        "ko_bakers_game",
        FCS_PRESET_KINGS_ONLY_BAKERS_GAME,
    },
    {
        "kings_only_bakers_game",
        FCS_PRESET_KINGS_ONLY_BAKERS_GAME,
    },
    {
        "relaxed_freecell",
        FCS_PRESET_RELAXED_FREECELL,
    },
    {
        "relaxed_seahaven_towers",
        FCS_PRESET_RELAXED_SEAHAVEN_TOWERS,
    },
    {
        "relaxed_seahaven",
        FCS_PRESET_RELAXED_SEAHAVEN_TOWERS,
    },
    {
        "seahaven_towers",
        FCS_PRESET_SEAHAVEN_TOWERS,
    },
    {
        "seahaven",
        FCS_PRESET_SEAHAVEN_TOWERS,
    },
    {
        "simple_simon",
        FCS_PRESET_SIMPLE_SIMON,
    },
    {
        "streets_and_alleys",
        FCS_PRESET_BELEAGUERED_CASTLE,
    },
    {
        "yukon",
        FCS_PRESET_YUKON,
    },
};

static GCC_INLINE int fcs_get_preset_id_by_name(
    const char * name
)
{
    int a;
    int ret = -1;
    int num_elems;

    num_elems = ( (int) (sizeof(fcs_preset_names)/sizeof(fcs_preset_names[0])));
    for(a=0;a<num_elems;a++)
    {
        if (!strcmp(name, fcs_preset_names[a].name))
        {
            ret = fcs_preset_names[a].preset_id;
            break;
        }
    }

    return ret;
}

static int fc_solve_char_to_test_num(char c)
{
    if ((c >= '0') && (c <= '9'))
    {
        return c-'0';
    }
    else if ((c >= 'a') && (c <= 'h'))
    {
        return c-'a'+10;
    }
    else if ((c >= 'A') && (c <= 'Z'))
    {
        return c-'A'+18;
    }
    else
    {
        return 0;
    }
}

#ifndef min
#define min(a,b) (((a)<(b))?(a):(b))
#endif

int fc_solve_apply_tests_order(
    fcs_tests_order_t * tests_order,
    const char * string,
    char * * error_string
    )

{
    int a;
    int len;
    int test_index;
    int is_group, is_start_group;
    if (tests_order->tests)
    {
        free(tests_order->tests);
        tests_order->num = 0;
        tests_order->tests = malloc(sizeof(tests_order->tests[0])*TESTS_ORDER_GROW_BY);
    }

#if 0
    instance->tests_order_num = min(strlen(string), FCS_TESTS_NUM);
#endif
    len = strlen(string);
    test_index = 0;
    is_group = 0;
    is_start_group = 0;
    for(a=0;(a<len) ;a++)
    {
        if ((string[a] == '(') || (string[a] == '['))
        {
            if (is_group)
            {
                *error_string = strdup("There's a nested random group.");
                return 1;
            }
            is_group = 1;
            is_start_group = 1;
            continue;
        }

        if ((string[a] == ')') || (string[a] == ']'))
        {
            if (is_start_group)
            {
                *error_string = strdup("There's an empty group.");
                return 2;
            }
            if (! is_group)
            {
                *error_string = strdup("There's a renegade right parenthesis or bracket.");
                return 3;
            }
            is_group = 0;
            is_start_group = 0;
            continue;
        }

        if (! ((test_index) & (TESTS_ORDER_GROW_BY - 1)))
        {
            tests_order->tests =
                realloc(
                    tests_order->tests,
                    sizeof(tests_order->tests[0]) * (test_index+TESTS_ORDER_GROW_BY)
                );
        }

        tests_order->tests[test_index++] = (fc_solve_char_to_test_num(string[a])%FCS_TESTS_NUM) | (is_group ? FCS_TEST_ORDER_FLAG_RANDOM : 0) | (is_start_group ? FCS_TEST_ORDER_FLAG_START_RANDOM_GROUP : 0);

        is_start_group = 0;
    }
    if (a != len)
    {
        *error_string = strdup("The Input string is too long.");
        return 4;
    }

    tests_order->num = test_index;
    *error_string = NULL;

    return 0;
}

int fc_solve_apply_preset_by_ptr(
    fc_solve_instance_t * instance,
    const fcs_preset_t * preset_ptr
        )
{
    char * no_use;

#define preset (*preset_ptr)
    if (preset.freecells_num > MAX_NUM_FREECELLS)
    {
        return FCS_PRESET_CODE_FREECELLS_EXCEED_MAX;
    }
    if (preset.stacks_num > MAX_NUM_STACKS)
    {
        return FCS_PRESET_CODE_STACKS_EXCEED_MAX;
    }
    if (preset.decks_num > MAX_NUM_DECKS)
    {
        return FCS_PRESET_CODE_DECKS_EXCEED_MAX;
    }
#ifndef HARD_CODED_NUM_FREECELLS
    instance->freecells_num = preset.freecells_num;
#endif
#ifndef HARD_CODED_NUM_STACKS
    instance->stacks_num = preset.stacks_num;
#endif
#ifndef HARD_CODED_NUM_DECKS
    instance->decks_num = preset.decks_num;
#endif

    instance->sequences_are_built_by = preset.sequences_are_built_by;
    instance->unlimited_sequence_move = preset.unlimited_sequence_move;
    instance->empty_stacks_fill = preset.empty_stacks_fill;

    /*
     * This code makes sure that all the tests in all the existing
     * soft threads are acceptable by the new preset.
     * */

    {
        int ht_idx, st_idx;
        for(ht_idx = 0; ht_idx < instance->num_hard_threads ; ht_idx++)
        {
            for(st_idx = 0; st_idx < instance->hard_threads[ht_idx]->num_soft_threads; st_idx++)
            {
                fc_solve_soft_thread_t * soft_thread = instance->hard_threads[ht_idx]->soft_threads[st_idx];

                int num_valid_tests;
                const char * s;

                /* Check every test */

                for(num_valid_tests=0;num_valid_tests < soft_thread->tests_order.num; num_valid_tests++)
                {
                    for(s = preset.allowed_tests;*s != '\0';s++)
                    {
                        /* Check if this test corresponds to this character */
                        if ((soft_thread->tests_order.tests[num_valid_tests] & FCS_TEST_ORDER_NO_FLAGS_MASK) == ((fc_solve_char_to_test_num(*s)%FCS_TESTS_NUM)))
                        {
                            break;
                        }
                    }
                    /* If the end of the string was reached, it means
                     * this test is unacceptable by this preset. */
                    if (*s == '\0')
                    {
                        break;
                    }
                }
                if (num_valid_tests < soft_thread->tests_order.num)
                {
                    fc_solve_apply_tests_order(
                            &(soft_thread->tests_order),
                            preset.tests_order,
                            &no_use);
                }
            }
        }
    }

    /* Assign the master tests order */

    {
        fc_solve_apply_tests_order(
            &(instance->instance_tests_order),
            preset.tests_order,
            &no_use);
    }
#undef preset
    return FCS_PRESET_CODE_OK;
}

static GCC_INLINE int fcs_get_preset_by_id(
    int preset_id,
    const fcs_preset_t * * preset_ptr
    )
{
    int preset_index;
    int num_elems;

    num_elems = ( (int) (sizeof(fcs_presets)/sizeof(fcs_presets[0])));

    for(preset_index=0 ; preset_index < num_elems ; preset_index++)
    {
        if (fcs_presets[preset_index].preset_id == preset_id)
        {
            *preset_ptr = &(fcs_presets[preset_index]);
            return FCS_PRESET_CODE_OK;
        }
    }

    return FCS_PRESET_CODE_NOT_FOUND;
}

int fc_solve_get_preset_by_name(
    const char * name,
    const fcs_preset_t * * preset_ptr
    )
{
    int preset_id;

    preset_id = fcs_get_preset_id_by_name(name);
    if (preset_id >= 0)
    {
        return fcs_get_preset_by_id(
            preset_id,
            preset_ptr
            );
    }
    else
    {
        return FCS_PRESET_CODE_NOT_FOUND;
    }
}

int fc_solve_apply_preset_by_name(
    fc_solve_instance_t * instance,
    const char * name
    )
{
    int ret;
    const fcs_preset_t * preset_ptr;

    ret = fc_solve_get_preset_by_name(
        name,
        &preset_ptr
        );

    if (ret != FCS_PRESET_CODE_OK)
    {
        return ret;
    }

    return fc_solve_apply_preset_by_ptr(instance, preset_ptr);
}
