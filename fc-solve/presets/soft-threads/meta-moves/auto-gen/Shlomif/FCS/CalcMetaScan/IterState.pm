package Shlomif::FCS::CalcMetaScan::IterState;

use strict;
use warnings;

use PDL ();

use base 'Shlomif::FCS::CalcMetaScan::Base';

use vars (qw(@fields %fields_map));
@fields = (qw(
    _main
    num_solved
    quota
    scan_idx
));

use Exception::Class
(
    'Shlomif::FCS::CalcMetaScan::Error::OutOfQuotas'
);

%fields_map = (map { $_ => 1 } @fields);

__PACKAGE__->mk_acc_ref(\@fields);

sub _init
{
    my $self = shift;

    my $args = shift;

    %{$self} = (%{$self}, %$args);

    return 0;
}

sub attach_to
{
    my $self = shift;
    my $main_obj = shift;

    $self->_main($main_obj);

    return;
}

sub get_chosen_struct
{
    my $self = shift;
    return
        {
            'q' => $self->quota(), 
            'ind' => $self->scan_idx() 
        };    
}

sub detach
{
    my $self = shift;
    $self->_main(undef);
}

sub idx_slice : lvalue
{
    my $self = shift;

    my $scans_data = $self->_main()->scans_data();

    my @dims = $scans_data->dims();

    return $scans_data->slice(
        join(",",
            ":", $self->scan_idx(), (("(0)") x (@dims-2))
        )
    );
}

sub update_total_iters
{
    my $state = shift;

    # $r is the result of this scan.
    my $r = $state->idx_slice();
    
    # Add the total iterations for all the states that were solved by
    # this scan.
    $state->_main()->add('total_iters',
        PDL::sum((($r <= $state->quota()) & ($r > 0)) * $r)
    );
    
    # Find all the states that weren't solved.
    my $indexes = PDL::which(($r > $state->quota()) | ($r < 0));
    
    # Add the iterations for all the states that have not been solved
    # yet.
    $state->_main()->add('total_iters', ($indexes->nelem() * $state->quota()));
    
    # Keep only the states that have not been solved yet.
    $state->_main()->scans_data(
        $state->_main()->scans_data()->dice($indexes, "X")->copy()
    );
}

sub update_idx_slice
{
    my $state = shift;
    my $r = $state->idx_slice()->copy();
    # $r cannot be 0, because the ones that were 0, were already solved
    # in $state->update_total_iters().
    $state->idx_slice() .= 
        (($r > 0) * ($r - $state->quota())) + 
        (($r < 0) * ($r                  ));
}

sub _mark_as_used
{
    my $state = shift;
    $state->_main()->selected_scans()->[$state->scan_idx()]->mark_as_used();

    return;
}

sub _add_chosen
{
    my $state = shift;

    push @{$state->_main()->chosen_scans()}, $state->get_chosen_struct();

    return;
}

sub _update_total_boards_solved
{
    my $state = shift;

    $state->_main()->add('total_boards_solved', $state->num_solved());

    return;
}

sub _trace_wrapper
{
    my $state = shift;

    $state->_main()->trace(
        {
            'iters_quota' => $state->quota(),
            'selected_scan_idx' => $state->scan_idx(),
            'total_boards_solved' => $state->_main()->total_boards_solved(),
        }
    );

    return;
}

sub register_params
{
    my $state = shift;

    $state->_add_chosen();
    $state->_mark_as_used();
    $state->_update_total_boards_solved();
    $state->_trace_wrapper();

    return;
}

1;
