package Shlomif::FCS::CalcMetaScan;

use strict;
use warnings;

use Shlomif::FCS::CalcMetaScan::IterState;
use Shlomif::FCS::CalcMetaScan::Structs;

use base 'Shlomif::FCS::CalcMetaScan::Base';

use vars (qw(@fields %fields_map));

@fields = (qw(
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
    trace_cb
));

use PDL ();

%fields_map = (map { $_ => 1 } @fields);

__PACKAGE__->mk_acc_ref(\@fields);

sub add
{
    my ($self, $field, $diff) = @_;
    if (!exists($fields_map{$field}))
    {
        die "Cannot add to field \"$field\"!";
    }
    return $self->$field($self->$field() + $diff);
}

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

    $self->trace_cb($args->{'trace_cb'});

    $self->_iter_idx(0);

    $self->_optimize_for($args->{'optimize_for'});

    return 0;
}

sub _next_iter_idx
{
    my $self = shift;

    my $ret = $self->_iter_idx();

    $self->_iter_idx($ret+1);

    return $ret;
}

sub get_next_quota
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

sub get_iter_state_params
{
    my $self = shift;

    my $method = 'get_iter_state_params_' . $self->_optimize_for();

    return $self->$method();
}

sub _my_sum_over
{
    my $pdl = shift;

    return $pdl->sumover()->slice(":,(0)");
}

sub get_iter_state_params_len
{
    my $self = shift;

    my $iters_quota = 0;
    my $num_solved_in_iter = 0;
    my $selected_scan_idx;

    # If no boards were solved, then try with a larger quota
    while ($num_solved_in_iter == 0)
    {
        my $q_more = $self->get_next_quota();
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

sub get_iter_state_params_minmax_len
{
    my $self = shift;

    my $iters_quota = 0;
    my $num_solved_in_iter = 0;
    my $selected_scan_idx;

    # If no boards were solved, then try with a larger quota
    while ($num_solved_in_iter == 0)
    {
        my $q_more = $self->get_next_quota();
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

sub get_iter_state_params_speed
{
    my $self = shift;

    my $iters_quota = 0;
    my $num_solved_in_iter = 0;
    my $selected_scan_idx;

    # If no boards were solved, then try with a larger quota
    while ($num_solved_in_iter == 0)
    {
        my $q_more = $self->get_next_quota();
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

sub get_selected_scan
{
    my $self = shift;

    my $iter_state = 
        Shlomif::FCS::CalcMetaScan::IterState->new(
            $self->get_iter_state_params(),
        );

    $iter_state->attach_to($self);

    return $iter_state;
}

sub inspect_quota
{
    my $self = shift;

    my $state = $self->get_selected_scan();

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

=cut

sub calc_meta_scan
{
    my $self = shift;

    $self->chosen_scans([]);

    $self->_total_boards_solved(0);
    $self->_total_iters(0);

    $self->_status("iterating");
    # $self->inspect_quota() throws ::Error::OutOfQuotas if
    # it does not have any available quotas.
    eval
    {
        while ($self->_status() eq "iterating")
        {
            $self->inspect_quota();
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

sub trace
{
    my ($self, $args) = @_;
    my $trace_cb = $self->trace_cb();
    if (UNIVERSAL::isa($trace_cb, "CODE"))
    {
        $trace_cb->($args);
    }
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

    $self->add('_total_iters', $how_much);

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

1;

