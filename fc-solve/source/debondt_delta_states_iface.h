/*
 * This file is part of Freecell Solver. It is subject to the license terms in
 * the COPYING.txt file found in the top-level directory of this distribution
 * and at http://fc-solve.shlomifish.org/docs/distro/COPYING.html . No part of
 * Freecell Solver, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the COPYING file.
 *
 * Copyright (c) 2000 Shlomi Fish
 */
/*
 * debondt_delta_states_iface.h - some public interface functions for
 * debondt_delta_state.c
 * (without all the baggage).
 */
#pragma once

#ifdef __cplusplus
extern "C" {
#endif

extern char *fc_solve_user_INTERNAL_debondt_delta_states_enc_and_dec(
    fcs_dbm_variant_type, const char *, const char *);

#ifdef __cplusplus
}
#endif
