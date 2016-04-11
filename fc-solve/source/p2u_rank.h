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
 * p2u_rank.h - header file for the p2u_rank function.
 *
 */
#ifndef FC_SOLVE__P2U_RANK_H
#define FC_SOLVE__P2U_RANK_H

#ifdef __cplusplus
extern "C" {
#endif

#include <string.h>

#include "config.h"

#include "rinutils.h"
/*
 * Those strings contain the string representations of the different cards.
 * If CARD_DEBUG_PRES is defined then an asterisk is printed as an empty card.
 *
 * Notice that there are two of them: one prints 10 and one prints T for the
 * 10 card.
 *
 * */
#ifdef CARD_DEBUG_PRES
#define CARD_ZERO() "*"
#else
#define CARD_ZERO() " "
#endif

/*
 * Converts a rank from its internal representation to a string.
 *
 * rank_idx - the rank
 * str - the string to output to.
 * t - whether 10 should be printed as T or not.
 * */
#define GEN_CARD_MAP(t_card) { CARD_ZERO(), "A", "2", "3", "4", "5", "6", "7", "8", "9", t_card, "J", "Q", "K" }

#ifndef FCS_IMPLICIT_T_RANK
static const char card_map_3_10[14][4] = GEN_CARD_MAP("10");

static const char card_map_3_T[14][4] = GEN_CARD_MAP("T");
#else
static const char cards_char_map[15] = ( CARD_ZERO() "A23456789TJQK" );
#endif

static GCC_INLINE void fc_solve_p2u_rank(
    const int rank_idx,
    char * const str
    PASS_T(const fcs_bool_t t)
)
{
    {
#define INDEX() (rank_idx)
#ifdef FCS_IMPLICIT_T_RANK
        str[0] = cards_char_map[INDEX()];
        str[1] = '\0';
#else
        strcpy(str,
            (t ? card_map_3_T : card_map_3_10)
            [INDEX()]);
#endif
#undef INDEX
    }
}

#endif /* FC_SOLVE__P2U_RANK_H */
