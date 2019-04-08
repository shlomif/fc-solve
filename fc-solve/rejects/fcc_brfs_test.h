/*
 * fcc_brfs_test.h
 * Copyright (C) 2019 Shlomi Fish <shlomif@cpan.org>
 *
 * Distributed under terms of the MIT license.
 */

#ifndef FCC_BRFS_TEST_H
#define FCC_BRFS_TEST_H

typedef struct
{
    int count;
    fcs_fcc_move *moves;
    char *state_as_string;
} fcs_FCC_start_point_result;

DLLEXPORT int fc_solve_user_INTERNAL_find_fcc_start_points(fcs_dbm_variant_type,
    const char *, const int, const fcs_fcc_move *const,
    fcs_FCC_start_point_result **, long *const);

DLLEXPORT void fc_solve_user_INTERNAL_free_fcc_start_points(
    fcs_FCC_start_point_result *const);

DLLEXPORT int fc_solve_user_INTERNAL_is_fcc_new(fcs_dbm_variant_type,
    const char *, const char *,
    /* NULL-terminated */
    const char **,
    /* NULL-terminated */
    const char **, bool *const);


#endif /* !FCC_BRFS_TEST_H */
