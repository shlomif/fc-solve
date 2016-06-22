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
 * fc_pro_range_solver.c - the FC-Pro range solver. Solves a range of
 * boards and displays the moves counts and the number of moves in their
 * solution.
 */
#include "output_to_file.h"
#include "range_solvers_gen_ms_boards.h"
#include "range_solvers_binary_output.h"
#include "fc_pro_iface_pos.h"

static GCC_INLINE void fc_pro_get_board(long gamenumber, fcs_state_string_t state_string, fcs_state_keyval_pair_t * pos IND_BUF_T_PARAM(indirect_stacks_buffer))
{
    get_board(gamenumber, state_string);
    fc_solve_initial_user_state_to_c(
        state_string, pos, 4, 8, 1, indirect_stacks_buffer
    );
}

static void my_iter_handler(
    void * const user_instance,
    const fcs_int_limit_t iter_num,
    const int depth,
    void * const ptr_state,
    const fcs_int_limit_t parent_iter_num,
    void * lp_context
    )
{
    const fc_solve_display_information_context_t * const display_context = (const fc_solve_display_information_context_t * const)lp_context;

    fprintf(stdout, "Iteration: %li\n", (long)iter_num);
    fprintf(stdout, "Depth: %i\n", depth);
    if (display_context->display_parent_iter_num)
    {
        fprintf(stdout, "Parent Iteration: %li\n", (long)parent_iter_num);
    }
    fprintf(stdout, "\n");


    if (display_context->debug_iter_state_output)
    {
        char state_string[1000];
            freecell_solver_user_iter_state_stringify(
                user_instance,
                state_string,
                ptr_state
                FC_SOLVE__PASS_PARSABLE(display_context->parseable_output)
                , display_context->canonized_order_output
                FC_SOLVE__PASS_T(display_context->display_10_as_t)
                );
        printf("%s\n---------------\n\n\n", state_string);
    }
}

#include "cl_callback_range.h"

int main(int argc, char * argv[])
{
    const char * variant = "freecell";
    fcs_portable_time_t mytime;
    fcs_int64_t total_num_iters = 0;
    fcs_bool_t was_total_iterations_limit_per_board_set = FALSE;
    fcs_int_limit_t total_iterations_limit_per_board = -1;
    binary_output_t binary_output = {.filename = NULL,};
    int arg = 1;

    if (argc < 4)
    {
        fprintf(stderr, "Not Enough Arguments!\n");
        print_help();
        exit(-1);
    }
    int start_board = atoi(argv[arg++]);
    int end_board = atoi(argv[arg++]);
    const int stop_at = atoi(argv[arg++]);
    if (stop_at <= 0)
    {
        fprintf(stderr, "print_step (the third argument) must be greater than 0.\n");
        print_help();
        exit(-1);

    }

    for (;arg < argc; arg++)
    {
        if (!strcmp(argv[arg], "--variant"))
        {
            arg++;
            if (arg == argc)
            {
                fprintf(stderr, "--variant came without an argument!\n");
                print_help();
                exit(-1);
            }
            variant = argv[arg];

            if (strlen(variant) > 50)
            {
                fprintf(stderr, "--variant's argument is too long!\n");
                print_help();
                exit(-1);
            }
        }
        else if (!strcmp(argv[arg], "--binary-output-to"))
        {
            arg++;
            if (arg == argc)
            {
                fprintf(stderr, "--binary-output-to came without an argument!\n");
                print_help();
                exit(-1);
            }
            binary_output.filename = argv[arg];
        }
        else if (!strcmp(argv[arg], "--total-iterations-limit"))
        {
            arg++;
            if (arg == argc)
            {
                fprintf(stderr, "--total-iterations-limit came without an argument!\n");
                print_help();
                exit(-1);
            }
            was_total_iterations_limit_per_board_set = TRUE;
            total_iterations_limit_per_board = atol(argv[arg]);
        }
        else
        {
            break;
        }
    }

    FCS_PRINT_STARTED_AT(mytime);
    fflush(stdout);

    fc_solve_display_information_context_t display_context = INITIAL_DISPLAY_CONTEXT;
    void * const instance = alloc_instance_and_parse(
        argc,
        argv,
        &arg,
        known_parameters,
        cmd_line_callback,
        &display_context,
        TRUE
    );

    bin_init(&binary_output, &start_board, &end_board, &total_iterations_limit_per_board);
    const fcs_bool_t variant_is_freecell = (!strcmp(variant, "freecell"));
    freecell_solver_user_apply_preset(instance, variant);

    if (was_total_iterations_limit_per_board_set)
    {
        freecell_solver_user_limit_iterations_long(instance, total_iterations_limit_per_board);
    }

#define BUF_SIZE 2000
    char buffer[BUF_SIZE];
    for (int board_num = start_board ; board_num <= end_board ; board_num++)
    {
        fcs_state_keyval_pair_t pos;
        DECLARE_IND_BUF_T(indirect_stacks_buffer)
        if (variant_is_freecell)
        {
            fc_pro_get_board(board_num, buffer, &(pos)
                PASS_IND_BUF_T(indirect_stacks_buffer));
        }
        else
        {
            char command[1000];

            sprintf(command, "make_pysol_freecell_board.py -F -t %d %s",
                    board_num,
                    variant
                   );

            FILE * const from_make_pysol = popen(command, "r");
            fread(buffer, sizeof(buffer[0]), BUF_SIZE-1, from_make_pysol);
            pclose(from_make_pysol);
#undef BUF_SIZE
        }

        buffer[COUNT(buffer)-1] = '\0';

        int num_iters;
        int num_moves;
        int num_fcpro_moves;
        fcs_moves_processed_t fc_pro_moves = {.moves = NULL};

        switch(
            freecell_solver_user_solve_board(
                instance,
                buffer
                )
        )
        {
            case FCS_STATE_SUSPEND_PROCESS:
            FCS_PRINT_INTRACTABLE_BOARD(mytime, board_num);
            num_iters = num_moves = num_fcpro_moves = -1;
            break;

            case FCS_STATE_IS_NOT_SOLVEABLE:
            FCS_PRINT_UNSOLVED_BOARD(mytime, board_num);
            num_iters = num_moves = num_fcpro_moves = -2;
            break;

            default:
            num_iters = (int)freecell_solver_user_get_num_times_long(instance);
            num_moves = freecell_solver_user_get_moves_left(instance);

            if (variant_is_freecell)
            {
                fcs_moves_sequence_t moves_seq;

                freecell_solver_user_get_moves_sequence(instance, &moves_seq);
                fc_solve_moves_processed_gen(&fc_pro_moves, &pos, 4, &moves_seq);

                num_fcpro_moves = fc_solve_moves_processed_get_moves_left(&fc_pro_moves);

                if (moves_seq.moves)
                {
                    free (moves_seq.moves);
                    moves_seq.moves = NULL;
                }
            }
            else
            {
                num_fcpro_moves = num_moves;
            }
            break;
        }
        print_int(&binary_output, num_iters);
        printf("[[Num Iters]]=%d\n[[Num FCS Moves]]=%d\n[[Num FCPro Moves]]=%d\n", num_iters, num_moves, num_fcpro_moves);

        printf("%s\n", "[[Start]]");
        if (fc_pro_moves.moves)
        {
            fcs_extended_move_t move;
            int len = 0;
            while (! fc_solve_moves_processed_get_next_move(&fc_pro_moves, &move))
            {
                char temp_str[10];
                fc_solve_moves_processed_render_move(move, temp_str);
                printf("%s%c", temp_str,
                    ((((++len) % 10) == 0) ? '\n' : ' ')
                );
            }
            fc_solve_moves_processed_free(&fc_pro_moves);
        }
        printf("\n%s\n", "[[End]]");
        fflush(stdout);
        total_num_iters += freecell_solver_user_get_num_times_long(instance);

        if (board_num % stop_at == 0)
        {
            FCS_PRINT_REACHED_BOARD(mytime, board_num, total_num_iters);
            fflush(stdout);
        }

        freecell_solver_user_recycle(instance);
    }

    freecell_solver_user_free(instance);
    bin_close(&binary_output);

    return 0;
}
