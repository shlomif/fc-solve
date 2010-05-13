package Shlomif::FCS::CalcMetaScan;

use strict;
use warnings;

use Shlomif::FCS::CalcMetaScan::IterState;
use Shlomif::FCS::CalcMetaScan::Structs;

use base 'Shlomif::FCS::CalcMetaScan::Base';

use PDL ();

__PACKAGE__->mk_acc_ref([qw(
    chosen_scans
    _iter_idx
    _num_boards
    _orig_scans_data
    _optimize_for
    _scans_data
    _selected_scans
    _status
    _quotas
    _total_boards_solved
    _total_iters
    _trace_cb
)]);

sub _init
{
    my $self = shift;

    my $args = shift;

    $self->_quotas($args->{'quotas'}) or
        die "Quotas not specified!";

    if (!exists($args->{'scans_data'}))
    {
        die "scans_data not specified!";
    }

    $self->_orig_scans_data($args->{'scans_data'}->copy());
    $self->_scans_data($self->_orig_scans_data()->copy());

    $self->_selected_scans($args->{'selected_scans'}) or
        die "selected_scans not specified!";

    $self->_num_boards($args->{'num_boards'}) or
        die "num_boards not specified!";

    $self->_trace_cb($args->{'trace_cb'});

    $self->_iter_idx(0);

    $self->_optimize_for($args->{'optimize_for'});

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
            Shlomif::FCS::CalcMetaScan::Error::OutOfQuotas->throw(
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
            Shlomif::FCS::CalcMetaScan::Error::OutOfQuotas->throw(
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
            Shlomif::FCS::CalcMetaScan::Error::OutOfQuotas->throw(
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
        Shlomif::FCS::CalcMetaScan::IterState->new(
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

=head2 my $chosen_scans_array_ref = $self->chosen_scans()

Returns the scans that have been chosen to perform the iteration. Each one is
a Shlomif::FCS::CalcMetaScan::ScanRun object.

=head2 $calc_meta_scan->calc_meta_scan()

Calculates the meta-scan after initialisation. See here for the details
of the algorithm:

L<http://www.shlomifish.org/lecture/Freecell-Solver/The-Next-Pres/slides/multi-tasking/best-meta-scan/>

=cut

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
    if (my $err = Exception::Class->caught('Shlomif::FCS::CalcMetaScan::Error::OutOfQuotas'))
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
        else
        {
            die $err;
        }
    }
}

=head2 $self->calc_flares_meta_scan()

This function calculates the flares meta-scan: i.e: assuming that all atomic
scans are run one after the other and the shortest solutions of all
successful scans are being picked.

=cut

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

        if ($min_avg > $last_avg)
        {
            next FLARES_LOOP;
        }

        $last_avg = $min_avg;

        push @{$self->chosen_scans()}, 
            Shlomif::FCS::CalcMetaScan::ScanRun->new(
                {
                    iters => $iters_quota, 
                    scan => $selected_scan_idx,
                }
            );

        $flares_num_iters->set($selected_scan_idx, $flares_num_iters->at($selected_scan_idx)+$iters_quota);
        $self->_selected_scans()->[$selected_scan_idx]->mark_as_used();

        $iters_quota = 0;

        print "Finished ", $loop_iter_num++, " ; Avg = $min_avg\n";
    }
}

=head2 $calc_meta_scan->calc_board_iters($board_idx)

Calculates the iterations of the board $board_idx in all the scans.

Returns a hash_ref containing the key 'per_scan_iters' for the iterations
per scan, and 'board_iters' for the total board iterations when ran in the
scans.

=cut

sub calc_board_iters
{
    my $self = shift;
    my $board = shift;

    my $board_iters = 0;

    my @info = PDL::list($self->_orig_scans_data()->slice("$board,:"));
    my @orig_info = @info;

    foreach my $s (@{$self->chosen_scans()})
    {
        if (($info[$s->scan()] > 0) && ($info[$s->scan()] <= $s->iters()))
        {
            $board_iters += $info[$s->iters()];
            last;
        }
        else
        {
            if ($info[$s->scan()] > 0)
            {
                $info[$s->scan()] -= $s->iters();
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

=head2 my $status = $calc_meta_scan->get_final_status()

Returns the status as string:

=over 4

=item * "solved_all"

=item * "iterating"

=item * "out_of_quotas"

=back

=cut

sub get_final_status
{
    my $self = shift;

    return $self->_status();
}

=head2 my $sim_results_obj = $calc_meta_scan->simulate_board($board_idx)

Simulates the board No $board_idx through the scan. Returns a 
L<Shlomif::FCS::CalcMetaScan::SimulationResults> object.

=cut

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
        if (($info[$s->scan()] > 0) && ($info[$s->scan()] <= $s->iters()))
        {
            $add_new_scan_run->(
                Shlomif::FCS::CalcMetaScan::ScanRun->new(
                    {
                        iters => $info[$s->scan()],
                        scan => $s->scan(),
                    },
                )
            );

            $status = "Solved";
            last SCANS_LOOP;
        }
        else
        {
            if ($info[$s->scan()] > 0)
            {
                $info[$s->scan()] -= $s->iters();
            }

            $add_new_scan_run->( 
                Shlomif::FCS::CalcMetaScan::ScanRun->new(
                    {
                        iters => $s->iters(),
                        scan => $s->scan(),
                    },
                )
            );
        }
    }

    return
        Shlomif::FCS::CalcMetaScan::SimulationResults->new(
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

=head2 my $n = $calc_meta_scan->get_total_iters()

Returns the total iterations count so far.

=cut

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

package Shlomif::FCS::CalcMetaScan::ScanRun;

use base 'Shlomif::FCS::CalcMetaScan::Base';

__PACKAGE__->mk_acc_ref([qw(iters scan)]);

sub _init
{
    my ($self, $args) = @_;

    $self->iters($args->{iters});
    $self->scan($args->{scan});

    return;
}

sub clone
{
    my $self = shift;

    return ref($self)->new({iters => $self->iters(), scan => $self->scan()});
}

package Shlomif::FCS::CalcMetaScan::SimulationResults;

use base 'Shlomif::FCS::CalcMetaScan::Base';

__PACKAGE__->mk_acc_ref([qw(status scan_runs total_iters)]);

sub _init
{
    my ($self, $args) = @_;

    $self->status($args->{status});
    $self->scan_runs($args->{scan_runs});
    $self->total_iters($args->{total_iters});

    return;
}

sub get_total_iters
{
    return shift->total_iters();
}

sub get_status
{
    return shift->status();
}

=head1 COPYRIGHT AND LICENSE

Copyright (c) 2010 Shlomi Fish

Permission is hereby granted, free of charge, to any person
obtaining a copy of this software and associated documentation
files (the "Software"), to deal in the Software without
restriction, including without limitation the rights to use,
copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the
Software is furnished to do so, subject to the following
conditions:

The above copyright notice and this permission notice shall be
included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
OTHER DEALINGS IN THE SOFTWARE.

=cut

1;

