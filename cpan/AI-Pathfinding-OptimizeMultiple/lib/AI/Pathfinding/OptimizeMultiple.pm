package AI::Pathfinding::OptimizeMultiple;

use strict;
use warnings;

use IO::Handle;

use AI::Pathfinding::OptimizeMultiple::IterState;
use AI::Pathfinding::OptimizeMultiple::Scan;
use AI::Pathfinding::OptimizeMultiple::ScanRun;
use AI::Pathfinding::OptimizeMultiple::SimulationResults;

use MooX qw/late/;

use PDL;

our $VERSION = '0.0.2';

has chosen_scans => (isa => 'ArrayRef', is => 'rw');
has _iter_idx => (isa => 'Int', is => 'rw', default => sub { 0; },);
has _num_boards => (isa => 'Int', is => 'ro', init_arg => 'num_boards',);
has _orig_scans_data => (isa => 'PDL', is => 'rw');
has _optimize_for => (isa => 'Str', is => 'ro', init_arg => 'optimize_for',);
has _scans_data   => (isa => 'PDL', is => 'rw');
has _selected_scans => (isa => 'ArrayRef', is => 'ro', init_arg => 'selected_scans',);
has _status => (isa => 'Str', is => 'rw');
has _quotas => (isa => 'ArrayRef[Int]', is => 'ro', init_arg => 'quotas');
has _total_boards_solved => (isa => 'Int', is => 'rw');
has _total_iters => (isa => 'Int', is => 'rw');
has _trace_cb => (isa => 'Maybe[CodeRef]', is => 'ro', init_arg => 'trace_cb');
has _scans_meta_data => (isa => 'ArrayRef', is => 'ro', init_arg => 'scans');
has _scans_iters_pdls => (isa => 'HashRef', is => 'rw', init_arg => 'scans_iters_pdls');

sub BUILD
{
    my $self = shift;

    my $args = shift;

    my $scans_data = PDL::cat(
        @{ $self->_scans_iters_pdls() }{
            map { $_->id() } @{$self->_selected_scans()}
        }
    );

    $self->_orig_scans_data($scans_data);
    $self->_scans_data($self->_orig_scans_data()->copy());

    return 0;
}

my $BOARDS_DIM = 0;
my $SCANS_DIM = 1;
my $STATISTICS_DIM = 2;

sub _next_iter_idx
{
    my $self = shift;

    my $ret = $self->_iter_idx();

    $self->_iter_idx($ret+1);

    return $ret;
}

sub _get_next_quota
{
    my $self = shift;

    my $iter = $self->_next_iter_idx();

    if (ref($self->_quotas()) eq "ARRAY")
    {
        return $self->_quotas()->[$iter];
    }
    else
    {
        return $self->_quotas()->($iter);
    }
}

sub _calc_get_iter_state_param_method
{
    my $self = shift;

    my $optimize_for = $self->_optimize_for();

    my %resolve =
    (
        len => "_get_iter_state_params_len",
        minmax_len => "_get_iter_state_params_minmax_len",
        speed => "_get_iter_state_params_speed",
    );

    return $resolve{$optimize_for};
}

sub _get_iter_state_params
{
    my $self = shift;

    my $method = $self->_calc_get_iter_state_param_method();

    return $self->$method();
}

sub _my_sum_over
{
    my $pdl = shift;

    return $pdl->sumover()->slice(":,(0)");
}

sub _my_xchg_sum_over
{
    my $pdl = shift;

    return _my_sum_over($pdl->xchg(0,1));
}

sub _get_iter_state_params_len
{
    my $self = shift;

    my $iters_quota = 0;
    my $num_solved_in_iter = 0;
    my $selected_scan_idx;

    # If no boards were solved, then try with a larger quota
    while ($num_solved_in_iter == 0)
    {
        my $q_more = $self->_get_next_quota();
        if (!defined($q_more))
        {
            AI::Pathfinding::OptimizeMultiple::Error::OutOfQuotas->throw(
                error => "No q_more",
            );
        }

        $iters_quota += $q_more;

        my $iters = $self->_scans_data()->slice(":,:,0");
        my $solved = (($iters <= $iters_quota) & ($iters > 0));
        my $num_moves = $self->_scans_data->slice(":,:,2");
        my $solved_moves = $solved * $num_moves;

        my $solved_moves_sums = _my_sum_over($solved_moves);
        my $solved_moves_counts = _my_sum_over($solved);
        my $solved_moves_avgs = $solved_moves_sums / $solved_moves_counts;

        (undef, undef, $selected_scan_idx, undef) =
            $solved_moves_avgs->minmaximum()
            ;

        $num_solved_in_iter = $solved_moves_counts->at($selected_scan_idx);
    }

    return
    {
        quota => $iters_quota,
        num_solved => $num_solved_in_iter,
        scan_idx => $selected_scan_idx,
    };
}

sub _get_iter_state_params_minmax_len
{
    my $self = shift;

    my $iters_quota = 0;
    my $num_solved_in_iter = 0;
    my $selected_scan_idx;

    # If no boards were solved, then try with a larger quota
    while ($num_solved_in_iter == 0)
    {
        my $q_more = $self->_get_next_quota();
        if (!defined($q_more))
        {
            AI::Pathfinding::OptimizeMultiple::Error::OutOfQuotas->throw(
                error => "No q_more",
            );
        }

        $iters_quota += $q_more;

        my $iters = $self->_scans_data()->slice(":,:,0");
        my $solved = (($iters <= $iters_quota) & ($iters > 0));
        my $num_moves = $self->_scans_data->slice(":,:,2");
        my $solved_moves = $solved * $num_moves;

        my $solved_moves_maxima = $solved_moves->maximum()->slice(":,(0),(0)");
        my $solved_moves_counts = _my_sum_over($solved);

        (undef, undef, $selected_scan_idx, undef) =
            $solved_moves_maxima->minmaximum()
            ;

        $num_solved_in_iter = $solved_moves_counts->at($selected_scan_idx);
    }

    return
    {
        quota => $iters_quota,
        num_solved => $num_solved_in_iter,
        scan_idx => $selected_scan_idx,
    };
}

sub _get_iter_state_params_speed
{
    my $self = shift;

    my $iters_quota = 0;
    my $num_solved_in_iter = 0;
    my $selected_scan_idx;

    # If no boards were solved, then try with a larger quota
    while ($num_solved_in_iter == 0)
    {
        my $q_more = $self->_get_next_quota();
        if (!defined($q_more))
        {
            AI::Pathfinding::OptimizeMultiple::Error::OutOfQuotas->throw(
                error => "No q_more"
            );
        }

        $iters_quota += $q_more;

        (undef, $num_solved_in_iter, undef, $selected_scan_idx) =
            PDL::minmaximum(
                PDL::sumover(
                    ($self->_scans_data() <= $iters_quota) &
                    ($self->_scans_data() > 0)
                )
              );
    }

    return
    {
        quota => $iters_quota,
        num_solved => $num_solved_in_iter,
        scan_idx => $selected_scan_idx,
    };
}

sub _get_selected_scan
{
    my $self = shift;

    my $iter_state =
        AI::Pathfinding::OptimizeMultiple::IterState->new(
            $self->_get_iter_state_params(),
        );

    $iter_state->attach_to($self);

    return $iter_state;
}

sub _inspect_quota
{
    my $self = shift;

    my $state = $self->_get_selected_scan();

    $state->register_params();

    $state->update_total_iters();

    if ($self->_total_boards_solved() == $self->_num_boards())
    {
        $self->_status("solved_all");
    }
    else
    {
        $state->update_idx_slice();
    }

    $state->detach();
}


sub calc_meta_scan
{
    my $self = shift;

    $self->chosen_scans([]);

    $self->_total_boards_solved(0);
    $self->_total_iters(0);

    $self->_status("iterating");
    # $self->_inspect_quota() throws ::Error::OutOfQuotas if
    # it does not have any available quotas.
    eval
    {
        while ($self->_status() eq "iterating")
        {
            $self->_inspect_quota();
        }
    };
    if (my $err = Exception::Class->caught('AI::Pathfinding::OptimizeMultiple::Error::OutOfQuotas'))
    {
        $self->_status("out_of_quotas");
    }
    else
    {
        $err = Exception::Class->caught();
        if (ref($err))
        {
            $err->rethrow;
        }
        elsif ($err)
        {
            die $err;
        }
    }

    return;
}


sub _get_num_scans
{
    my $self = shift;

    return (($self->_scans_data()->dims())[$SCANS_DIM]);
}

sub calc_flares_meta_scan
{
    my $self = shift;

    $self->chosen_scans([]);

    $self->_total_boards_solved(0);
    $self->_total_iters(0);

    $self->_status("iterating");

    my $iters_quota = 0;
    my $flares_num_iters = PDL::Core::pdl([(0) x $self->_get_num_scans()]);
    my $ones_constant = PDL::Core::pdl(
        [map { [1] } (1 .. $self->_get_num_scans())]
    );

    my $next_num_iters_for_each_scan_x_scan =
        (($ones_constant x $flares_num_iters));


    my $num_moves = $self->_scans_data->slice(":,:,1");

    # The number of moves for dimension 0,1,2 above.
    my $num_moves_repeat = $num_moves->clump(1..2)->xchg(0,1)->dummy(0,$self->_get_num_scans());

    my $selected_scan_idx;

    my $loop_iter_num = 0;

    my $UNSOLVED_NUM_MOVES_CONSTANT = 64 * 1024 * 1024;

    my $last_avg = $UNSOLVED_NUM_MOVES_CONSTANT;

    FLARES_LOOP:
    while (my $q_more = $self->_get_next_quota())
    {
        $iters_quota += $q_more;

        # Next number of iterations for each scan x scan combination.
        my $next_num_iters =
            (($ones_constant x $flares_num_iters) +
                (PDL::MatrixOps::identity($self->_get_num_scans())
                    * $iters_quota
                )
            );

        # print "\$next_num_iters = $next_num_iters\n";

        my $iters = $self->_scans_data()->slice(":,:,0");

        my $iters_repeat =
            $iters->dummy(0,$self->_get_num_scans())->xchg(1,2)->clump(2 .. 3);

        # print "\$iters_repeat =", join(",",$iters_repeat->dims()), "\n";

        my $next_num_iters_repeat =
            $next_num_iters->dummy(0,$self->_num_boards())->xchg(0,2);

        # print "\$next_num_iters_repeat =", join(",",$next_num_iters_repeat->dims()), "\n";

        # A boolean tensor of which boards were solved:
        # Dimension 0 - Which scan is it. - size - _get_num_scans()
        # Dimension 1 - Which scan we added the quota to
        #   - size - _get_num_scans()
        # Dimension 2 - Which board. - size - _num_boards()
        my $solved = ($iters_repeat >= 0) * ($iters_repeat < $next_num_iters_repeat);

        # print "\$num_moves_repeat =", join(",",$num_moves_repeat->dims()), "\n";



        my $num_moves_solved =
            ($solved * $num_moves_repeat) + ($solved->not() * $UNSOLVED_NUM_MOVES_CONSTANT);

        my $minimal_num_moves_solved = $num_moves_solved->xchg(0,1)->minimum();

        my $which_minima_are_solved =
            ($minimal_num_moves_solved != $UNSOLVED_NUM_MOVES_CONSTANT)
            ;

        my $minimal_with_zeroes =
            $which_minima_are_solved * $minimal_num_moves_solved;

        my $solved_moves_sums = _my_xchg_sum_over($minimal_with_zeroes);
        my $solved_moves_counts = _my_xchg_sum_over($which_minima_are_solved);
        my $solved_moves_avgs = $solved_moves_sums / $solved_moves_counts;

        # print join(",", $solved_moves_avgs->minmaximum()), "\n";

        my $min_avg;

        ($min_avg, undef, $selected_scan_idx, undef) =
            $solved_moves_avgs->minmaximum()
            ;

        $last_avg = $min_avg;

        push @{$self->chosen_scans()},
            AI::Pathfinding::OptimizeMultiple::ScanRun->new(
                {
                    iters => $iters_quota,
                    scan_idx => $selected_scan_idx,
                }
            );

        $flares_num_iters->set($selected_scan_idx, $flares_num_iters->at($selected_scan_idx)+$iters_quota);
        $self->_selected_scans()->[$selected_scan_idx]->mark_as_used();

        $iters_quota = 0;

        my $num_solved = $solved_moves_counts->at($selected_scan_idx);

        my $flares_num_iters_repeat =
            $flares_num_iters->dummy(0,$self->_num_boards());

        # A boolean tensor:
        # Dimension 0 - board.
        # Dimension 1 - scans.
        my $solved_with_which_iter =
            ($flares_num_iters_repeat >= $iters->clump(1 .. 2))
            & ($iters->clump(1 .. 2) >= 0)
            ;

        my $total_num_iters =
        (
            ($solved_with_which_iter * $flares_num_iters_repeat)->sum()
            + ($solved_with_which_iter->not()->andover()
                * $flares_num_iters->sum())->sum()
        );

        print "Finished ", $loop_iter_num++, " ; #Solved = $num_solved ; Iters = $total_num_iters ; Avg = $min_avg\n";
        STDOUT->flush();
    }
}


sub calc_board_iters
{
    my $self = shift;
    my $board = shift;

    my $board_iters = 0;

    my @info = PDL::list($self->_orig_scans_data()->slice("$board,:"));
    my @orig_info = @info;

    foreach my $s (@{$self->chosen_scans()})
    {
        if (($info[$s->scan_idx()] > 0) && ($info[$s->scan_idx()] <= $s->iters()))
        {
            $board_iters += $info[$s->iters()];
            last;
        }
        else
        {
            if ($info[$s->scan_idx()] > 0)
            {
                $info[$s->scan_idx()] -= $s->iters();
            }
            $board_iters += $s->iters();
        }
    }

    return
        {
            'per_scan_iters' => \@orig_info,
            'board_iters' => $board_iters,
        };
}


sub get_final_status
{
    my $self = shift;

    return $self->_status();
}


sub simulate_board
{
    my ($self, $board) = @_;

    my @info = PDL::list($self->_orig_scans_data()->slice("$board,:"));

    my $board_iters = 0;

    my @scan_runs;

    my $status = "Unsolved";

    my $add_new_scan_run = sub {
        my $scan_run = shift;

        push @scan_runs, $scan_run;

        $board_iters += $scan_run->iters();
    };

    SCANS_LOOP:
    foreach my $s (@{$self->chosen_scans()})
    {
        if (($info[$s->scan_idx()] > 0) && ($info[$s->scan_idx()] <= $s->iters()))
        {
            $add_new_scan_run->(
                AI::Pathfinding::OptimizeMultiple::ScanRun->new(
                    {
                        iters => $info[$s->scan_idx()],
                        scan_idx => $s->scan_idx(),
                    },
                )
            );

            $status = "Solved";
            last SCANS_LOOP;
        }
        else
        {
            if ($info[$s->scan_idx()] > 0)
            {
                $info[$s->scan_idx()] -= $s->iters();
            }

            $add_new_scan_run->(
                AI::Pathfinding::OptimizeMultiple::ScanRun->new(
                    {
                        iters => $s->iters(),
                        scan_idx => $s->scan_idx(),
                    },
                )
            );
        }
    }

    return
        AI::Pathfinding::OptimizeMultiple::SimulationResults->new(
            {
                status => $status,
                scan_runs => \@scan_runs,
                total_iters => $board_iters,
            }
        );
}

sub _trace
{
    my ($self, $args) = @_;

    if (my $trace_callback = $self->_trace_cb())
    {
        $trace_callback->($args);
    }

    return;
}


sub get_total_iters
{
    my $self = shift;

    return $self->_total_iters();
}

sub _add_to_total_iters
{
    my $self = shift;

    my $how_much = shift;

    $self->_total_iters($self->_total_iters() + $how_much);

    return;
}

sub _add_to_total_boards_solved
{
    my $self = shift;

    my $how_much = shift;

    $self->_total_boards_solved($self->_total_boards_solved() + $how_much);

    return;
}

1; # End of AI::Pathfinding::OptimizeMultiple

=pod

=head1 NAME

AI::Pathfinding::OptimizeMultiple - optimize path finding searches for a large
set of initial conditions (for better average performance).

=head1 SYNOPSIS

    use AI::Pathfinding::OptimizeMultiple

    my @scans =
    (
        {
            name => "first_search"
        },
        {
            name => "second_search",
        },
        {
            name => "third_search",
        },
    );

    my $obj = AI::Pathfinding::OptimizeMultiple->new(
        {
            scans => \@scans,
            num_boards => 32_000,
            optimize_for => 'speed',
            scans_iters_pdls =>
            {
                first_search => $first_search_pdl,
                second_search => $second_search_pdl,
            },
            quotas => [400, 300, 200],
            selected_scans =>
            [
                AI::Pathfinding::OptimizeMultiple::Scan->new(
                    id => 'first_search',
                    cmd_line => "--preset first_search",
                ),
                AI::Pathfinding::OptimizeMultiple::Scan->new(
                    id => 'second_search',
                    cmd_line => "--preset second_search",
                ),
                AI::Pathfinding::OptimizeMultiple::Scan->new(
                    id => 'third_search',
                    cmd_line => "--preset third_search",
                ),
            ],
        }
    );

    $obj->calc_meta_scan();

    foreach my $scan_alloc (@{$self->chosen_scans()})
    {
        printf "Run %s for %d iterations.\n",
            $scans[$scan_alloc->scan_idx], $scan_alloc->iters;
    }

=head1 DESCRIPTION

This CPAN distribution implements the algorithm described here:

=over 4

=item * L<https://groups.google.com/group/comp.ai.games/msg/41e899e9beea5583?dmode=source&output=gplain&noredirect>

=item * L<http://www.shlomifish.org/lecture/Perl/Lightning/Opt-Multi-Task-in-PDL/>

=back

Given statistics on the performance of several game AI searches (or scans)
across a representative number of initial cases, find a scan
that solves most deals with close-to-optimal performance, by using switch
tasking.

=head1 SUBROUTINES/METHODS

=head2 my $chosen_scans_array_ref = $self->chosen_scans()

Returns the scans that have been chosen to perform the iteration. Each one is
a AI::Pathfinding::OptimizeMultiple::ScanRun object.

=head2 $calc_meta_scan->calc_meta_scan()

Calculates the meta-scan after initialisation. See here for the details
of the algorithm:

L<http://www.shlomifish.org/lecture/Freecell-Solver/The-Next-Pres/slides/multi-tasking/best-meta-scan/>

=head2 $self->calc_flares_meta_scan()

This function calculates the flares meta-scan: i.e: assuming that all atomic
scans are run one after the other and the shortest solutions of all
successful scans are being picked.

=head2 $calc_meta_scan->calc_board_iters($board_idx)

Calculates the iterations of the board $board_idx in all the scans.

Returns a hash_ref containing the key 'per_scan_iters' for the iterations
per scan, and 'board_iters' for the total board iterations when ran in the
scans.

=head2 my $status = $calc_meta_scan->get_final_status()

Returns the status as string:

=over 4

=item * "solved_all"

=item * "iterating"

=item * "out_of_quotas"

=back

=head2 my $sim_results_obj = $calc_meta_scan->simulate_board($board_idx)

Simulates the board No $board_idx through the scan. Returns a
L<AI::Pathfinding::OptimizeMultiple::SimulationResults> object.

=head2 my $n = $calc_meta_scan->get_total_iters()

Returns the total iterations count so far.

=head2 BUILD()

Moo leftover. B<INTERNAL USE>.

=head1 SEE ALSO

=over 4

=item * L<Freecell Solver|http://fc-solve.shlomifish.org/>

For which this code was first written and used.

=item * L<Alternative Implementation in C#/.NET|https://bitbucket.org/shlomif/fc-solve/src/cc5b428ed9bad0132d7a7bc1a14fc6d3650edf45/fc-solve/presets/soft-threads/meta-moves/auto-gen/optimize-seq?at=master>

An Alternative implementation in C#/.NET, which was written because the
performance of the Perl/PDL code was too slow.

=item * L<PDL> - Perl Data Language

Used here.

=back

=head1 AUTHOR

Shlomi Fish, L<http://www.shlomifish.org/> .

=head1 ACKNOWLEDGEMENTS

B<popl> from Freenode's #perl for trying to dig some references to an existing
algorithm in the scientific literature.

=cut
