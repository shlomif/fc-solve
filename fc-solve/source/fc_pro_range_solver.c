/*
 * This file is part of Freecell Solver. It is subject to the license terms in
 * the COPYING.txt file found in the top-level directory of this distribution
 * and at http://fc-solve.shlomifish.org/docs/distro/COPYING.html . No part of
 * Freecell Solver, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the COPYING file.
 *
 * Copyright (c) 2000 Shlomi Fish
 */
// fc_pro_range_solver.c - the FC-Pro range solver. Solves a range of
// boards and displays the moves counts and the number of moves in their
// solution.
#include "default_iter_handler.h"
#include "range_solvers_gen_ms_boards.h"
#include "range_solvers_binary_output.h"
#include "fc_pro_iface_pos.h"
#include "try_param.h"

static inline void fc_pro_get_board(const long long deal_idx,
    fcs_state_string state_string,
    fcs_state_keyval_pair *const pos IND_BUF_T_PARAM(indirect_stacks_buffer))
{
    get_board_l(deal_idx, state_string);
    fc_solve_initial_user_state_to_c(
        state_string, pos, 4, 8, 1, indirect_stacks_buffer);
}

#include "cl_callback_range.h"
#include "range_solvers.h"

static inline int range_solvers_main(int argc, char *argv[], int arg,
    long long start_board, long long end_board, const long long stop_at)
{
    const char *variant = "freecell";
    long long total_num_iters = 0;
#ifndef FCS_WITHOUT_MAX_NUM_STATES
    bool was_total_iterations_limit_per_board_set = FALSE;
#endif
    fcs_int_limit_t total_iterations_limit_per_board = -1;
    fcs_binary_output binary_output = INIT_BINARY_OUTPUT;

    for (; arg < argc; arg++)
    {
        const char *param;
        if ((param = TRY_P("--variant")))
        {
            if (strlen(variant = param) > 50)
            {
#ifndef FCS_WITHOUT_CMD_LINE_HELP
                help_err("--variant's argument is too long!\n");
#else
                return -1;
#endif
            }
        }
        else if ((param = TRY_P("--binary-output-to")))
        {
            binary_output.filename = param;
        }
        else if ((param = TRY_P("--total-iterations-limit")))
        {
#ifndef FCS_WITHOUT_MAX_NUM_STATES
            was_total_iterations_limit_per_board_set = TRUE;
            total_iterations_limit_per_board = atol(param);
#endif
        }
        else
        {
            break;
        }
    }

    fc_solve_print_started_at();
    fflush(stdout);

    fc_solve_display_information_context display_context =
        INITIAL_DISPLAY_CONTEXT;
    void *const instance = alloc_instance_and_parse(argc, argv, &arg,
        known_parameters, cmd_line_callback, &display_context, TRUE);

    bin_init(&binary_output, &start_board, &end_board,
        &total_iterations_limit_per_board);
    const bool variant_is_freecell = (!strcmp(variant, "freecell"));
#ifndef FCS_FREECELL_ONLY
    if (!variant_is_freecell)
    {
        freecell_solver_user_apply_preset(instance, variant);
    }
#endif

#ifndef FCS_WITHOUT_MAX_NUM_STATES
    if (was_total_iterations_limit_per_board_set)
    {
        freecell_solver_user_limit_iterations_long(
            instance, total_iterations_limit_per_board);
    }
#endif

    char buffer[2000];
    for (long long board_num = start_board; board_num <= end_board; board_num++)
    {
        fcs_state_keyval_pair pos;
        DECLARE_IND_BUF_T(indirect_stacks_buffer)
        if (variant_is_freecell)
        {
            fc_pro_get_board(board_num, buffer,
                &(pos)PASS_IND_BUF_T(indirect_stacks_buffer));
        }
        else
        {
            char command[1000];

            sprintf(command,
                "make_pysol_freecell_board.py -F -t " FCS_LL_FMT "%s",
                board_num, variant);

            FILE *const from_make_pysol = popen(command, "r");
            fread(
                buffer, sizeof(buffer[0]), COUNT(buffer) - 1, from_make_pysol);
            pclose(from_make_pysol);
        }

        LAST(buffer) = '\0';

        int num_iters;
        int num_moves;
        int num_fcpro_moves;
        fcs_moves_processed fc_pro_moves = {.moves = NULL};

        switch (freecell_solver_user_solve_board(instance, buffer))
        {
        case FCS_STATE_SUSPEND_PROCESS:
            fc_solve_print_intractable(board_num);
            num_iters = num_moves = num_fcpro_moves = -1;
            break;

        case FCS_STATE_IS_NOT_SOLVEABLE:
            fc_solve_print_unsolved(board_num);
            num_iters = num_moves = num_fcpro_moves = -2;
            break;

        default:
            num_iters = (int)freecell_solver_user_get_num_times_long(instance);
#ifdef FCS_WITH_MOVES
            num_moves = freecell_solver_user_get_moves_left(instance);

            if (variant_is_freecell)
            {
                fcs_moves_sequence_t moves_seq;

                freecell_solver_user_get_moves_sequence(instance, &moves_seq);
                fc_solve_moves_processed_gen(
                    &fc_pro_moves, &pos, 4, &moves_seq);

                num_fcpro_moves =
                    fc_solve_moves_processed_get_moves_left(&fc_pro_moves);

                if (moves_seq.moves)
                {
                    free(moves_seq.moves);
                    moves_seq.moves = NULL;
                }
            }
            else
            {
                num_fcpro_moves = num_moves;
            }
#else
            num_fcpro_moves = num_moves = -1;
#endif
            break;
        }
        print_int(&binary_output, num_iters);
        printf(
            "[[Num Iters]]=%d\n[[Num FCS Moves]]=%d\n[[Num FCPro Moves]]=%d\n",
            num_iters, num_moves, num_fcpro_moves);

        printf("%s\n", "[[Start]]");
        if (fc_pro_moves.moves)
        {
#ifdef FCS_WITH_MOVES
            fcs_extended_move move;
            int len = 0;
            while (
                !fc_solve_moves_processed_get_next_move(&fc_pro_moves, &move))
            {
                char temp_str[10];
                fc_solve_moves_processed_render_move(move, temp_str);
                printf("%s%c", temp_str, ((((++len) % 10) == 0) ? '\n' : ' '));
            }
#endif
            fc_solve_moves_processed_free(&fc_pro_moves);
        }
        printf("\n%s\n", "[[End]]");
        fflush(stdout);
        total_num_iters += freecell_solver_user_get_num_times_long(instance);

        if (board_num % stop_at == 0)
        {
            fc_solve_print_reached(board_num, total_num_iters);
        }
        freecell_solver_user_recycle(instance);
    }

    freecell_solver_user_free(instance);
    bin_close(&binary_output);

    return 0;
}
