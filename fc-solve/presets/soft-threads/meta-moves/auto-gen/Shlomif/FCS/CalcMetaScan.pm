package Shlomif::FCS::CalcMetaScan;

use strict;
use warnings;

use Shlomif::FCS::CalcMetaScan::IterState;
use Shlomif::FCS::CalcMetaScan::Structs;

use base 'Shlomif::FCS::CalcMetaScan::Base';

use vars (qw(@fields %fields_map));

@fields = (qw(
    chosen_scans
    iter_idx
    num_boards
    orig_scans_data
    optimize_for
    scans_data
    selected_scans
    status
    quotas
    total_boards_solved
    total_iters
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

    my (%args) = (@_);

    $self->quotas($args{'quotas'}) or
        die "Quotas not specified!";

    if (!exists($args{'scans_data'}))
    {
        die "scans_data not specified!";
    }

    $self->orig_scans_data($args{'scans_data'}->copy());
    $self->scans_data($self->orig_scans_data()->copy());

    $self->selected_scans($args{'selected_scans'}) or
        die "selected_scans not specified!";

    $self->num_boards($args{'num_boards'}) or
        die "num_boards not specified!";

    $self->trace_cb($args{'trace_cb'});

    $self->iter_idx(0);

    $self->optimize_for($args{'optimize_for'});

    return 0;
}

sub _next_iter_idx
{
    my $self = shift;

    my $ret = $self->iter_idx();

    $self->iter_idx($ret+1);

    return $ret;
}

sub scans_rle
{
    my $self = shift;
    my %args = (@_);
    my $scans_list = [@{$args{'scans'}}];

    my $scan = shift(@$scans_list);

    my (@a);
    while (my $next_scan = shift(@$scans_list))
    {
        if ($next_scan->{'ind'} == $scan->{'ind'})
        {
            $scan->{'q'} += $next_scan->{'q'};
        }
        else
        {
            push @a, $scan;
            $scan = $next_scan;
        }
    }
    push @a, $scan;
    return \@a;
}

sub get_next_quota
{
    my $self = shift;

    my $iter = $self->_next_iter_idx();

    if (ref($self->quotas()) eq "ARRAY")
    {
        return $self->quotas()->[$iter];
    }
    else
    {
        return $self->quotas()->($iter);
    }
}

sub get_iter_state_params
{
    my $self = shift;

    my $method = 'get_iter_state_params_' . $self->optimize_for();

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

        my $iters = $self->scans_data()->slice(":,:,0");
        my $solved = (($iters <= $iters_quota) & ($iters > 0));
        my $num_moves = $self->scans_data->slice(":,:,2");
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

        my $iters = $self->scans_data()->slice(":,:,0");
        my $solved = (($iters <= $iters_quota) & ($iters > 0));
        my $num_moves = $self->scans_data->slice(":,:,2");
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
                    ($self->scans_data() <= $iters_quota) & 
                    ($self->scans_data() > 0)
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
            %{$self->get_iter_state_params()},
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
    
    if ($self->total_boards_solved() == $self->num_boards())
    {
        $self->status("solved_all");
    }
    else
    {
        $state->update_idx_slice();
    }

    $state->detach();
}

sub get_quotas
{
    my $self = shift;
    return $self->quotas();
}

sub calc_meta_scan
{
    my $self = shift;

    $self->chosen_scans([]);

    $self->total_boards_solved(0);
    $self->total_iters(0);

    $self->status("iterating");
    # $self->inspect_quota() throws ::Error::OutOfQuotas if
    # it does not have any available quotas.
    eval
    {
        while ($self->status() eq "iterating")
        {
            $self->inspect_quota();
        }
    };
    if (my $err = Exception::Class->caught('Shlomif::FCS::CalcMetaScan::Error::OutOfQuotas'))
    {
        # Do nothing - continue.
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

sub do_rle
{
    my $self = shift;
    $self->chosen_scans(
        $self->scans_rle('scans' => $self->chosen_scans())
    );
}

sub calc_board_iters
{
    my $self = shift;
    my $board = shift;
    my $board_iters = 0;

    my @info = PDL::list($self->orig_scans_data()->slice("$board,:"));
    my @orig_info = @info;

    foreach my $s (@{$self->chosen_scans()})
    {
        if (($info[$s->{'ind'}] > 0) && ($info[$s->{'ind'}] <= $s->{'q'}))
        {
            $board_iters += $info[$s->{'ind'}];
            last;
        }
        else
        {
            if ($info[$s->{'ind'}] > 0)
            {
                $info[$s->{'ind'}] -= $s->{'q'};
            }
            $board_iters += $s->{'q'};
        }
    }

    return
        {
            'per_scan_iters' => \@orig_info,
            'board_iters' => $board_iters,
        };
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

1;

