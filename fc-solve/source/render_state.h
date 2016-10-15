#pragma once

#define FCS__RENDER_STATE(state_str, state_ptr, locs)                          \
    fc_solve_state_as_string(state_str, state_ptr,                             \
        locs PASS_FREECELLS(FREECELLS_NUM) PASS_STACKS(STACKS_NUM)             \
            PASS_DECKS(DECKS_NUM) FC_SOLVE__PASS_PARSABLE(TRUE),               \
        FALSE FC_SOLVE__PASS_T(TRUE))
#define FCS__OUTPUT_STATE(out_fh, prefix, state_ptr, locs)                     \
    {                                                                          \
        char state_str[2000];                                                  \
        FCS__RENDER_STATE(state_str, state_ptr, locs);                         \
        fprintf(out_fh, "%s<<<\n%s>>>\n\n", prefix, state_str);                \
        fflush(out_fh);                                                        \
    }
