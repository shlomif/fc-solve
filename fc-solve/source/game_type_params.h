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
 * game_type_params.h: provide fcs_game_type_params_t
 */
#ifndef FC_SOLVE__GAME_TYPE_PARAMS_H
#define FC_SOLVE__GAME_TYPE_PARAMS_H

#include "config.h"
#include "game_type_limit.h"

typedef struct {
    /*
     * The number of Freecells, Stacks and Foundations present in the game.
     *
     * freecells_num and stacks_num are variable and may be specified at
     * the beginning of the execution of the algorithm. However, there
     * is a maximal limit to them which is set in config.h.
     *
     * decks_num can be 1 or 2 .
     * */

#define SET_INSTANCE_GAME_PARAMS(inst) \
    const fcs_game_type_params_t game_params = (inst)->game_params

#define SET_GAME_PARAMS() \
    SET_INSTANCE_GAME_PARAMS(instance)

#ifndef HARD_CODED_NUM_FREECELLS
    fcs_game_limit_t freecells_num;
#define INSTANCE_FREECELLS_NUM (instance->game_params.freecells_num)
#define LOCAL_FREECELLS_NUM (game_params.freecells_num)
#else
#define INSTANCE_FREECELLS_NUM HARD_CODED_NUM_FREECELLS
#define LOCAL_FREECELLS_NUM HARD_CODED_NUM_FREECELLS
#endif

#ifndef HARD_CODED_NUM_STACKS
    fcs_game_limit_t stacks_num;
#define INSTANCE_STACKS_NUM (instance->game_params.stacks_num)
#define LOCAL_STACKS_NUM (game_params.stacks_num)
#else
#define INSTANCE_STACKS_NUM HARD_CODED_NUM_STACKS
#define LOCAL_STACKS_NUM HARD_CODED_NUM_STACKS
#endif

#ifndef HARD_CODED_NUM_DECKS
    fcs_game_limit_t decks_num;
#define INSTANCE_DECKS_NUM (instance->game_params.decks_num)
#define LOCAL_DECKS_NUM (game_params.decks_num)
#else
#define INSTANCE_DECKS_NUM HARD_CODED_NUM_DECKS
#define LOCAL_DECKS_NUM HARD_CODED_NUM_DECKS
#endif

#ifndef FCS_FREECELL_ONLY
    /* sequences_are_built_by - (bits 0:1) - what two adjacent cards in the
     * same sequence can be.
     *
     * empty_stacks_fill (bits 2:3) - with what cards can empty stacks be
     * filled with.
     *
     * unlimited_sequence_move - (bit 4) - whether an entire sequence can be
     * moved from one place to the other regardless of the number of
     * unoccupied Freecells there are.
     * */
    fcs_game_limit_t game_flags;

#define INSTANCE_GAME_FLAGS  (instance->game_params.game_flags)
#define GET_INSTANCE_SEQUENCES_ARE_BUILT_BY(instance) \
    ((instance)->game_params.game_flags & 0x3)

#define INSTANCE_UNLIMITED_SEQUENCE_MOVE  (INSTANCE_GAME_FLAGS & (1 << 4))
#define INSTANCE_EMPTY_STACKS_FILL   ((INSTANCE_GAME_FLAGS >> 2) & 0x3)

#endif

} fcs_game_type_params_t;

#endif /* #ifndef FC_SOLVE__GAME_TYPE_PARAMS_H */

