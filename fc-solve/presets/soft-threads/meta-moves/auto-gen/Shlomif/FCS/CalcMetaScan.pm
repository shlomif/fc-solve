package Shlomif::FCS::CalcMetaScan::Error::OutOfQuotas;

use base 'Error';

package Shlomif::FCS::CalcMetaScan::IterState;

use strict;
use warnings;

use PDL ();

use base 'Shlomif::FCS::CalcMetaScan::Base';

use vars (qw(@fields %fields_map));
@fields = (qw(
    main
    num_solved
    quota
    scan_idx
));

%fields_map = (map { $_ => 1 } @fields);

__PACKAGE__->mk_accessors(@fields);

sub initialize
{
    my $self = shift;
    %$self = (%$self, @_);

    return 0;
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
    $self->main(undef);
}

sub idx_slice : lvalue
{
    my $self = shift;
    return $self->main()->scans_data()->slice(":,".$self->scan_idx())
}

sub update_total_iters
{
    my $state = shift;

    # $r is the result of this scan.
    my $r = $state->idx_slice();
    
    # Add the total iterations for all the states that were solved by
    # this scan.
    $state->main()->add('total_iters',
        PDL::sum((($r <= $state->quota()) & ($r > 0)) * $r)
    );
    
    # Find all the states that weren't solved.
    my $indexes = PDL::which(($r > $state->quota()) | ($r < 0));
    
    # Add the iterations for all the states that have not been solved
    # yet.
    $state->main()->add('total_iters', ($indexes->nelem() * $state->quota()));
    
    # Keep only the states that have not been solved yet.
    $state->main()->scans_data(
        $state->main()->scans_data()->dice($indexes, "X")->copy()
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

sub mark_as_used
{
    my $state = shift;
    $state->main()->selected_scans()->[$state->scan_idx()]->mark_as_used();
}

sub add_chosen
{
    my $state = shift;

    push @{$state->main()->chosen_scans()}, $state->get_chosen_struct();
}

sub update_total_boards_solved
{
    my $state = shift;
    $state->main()->add('total_boards_solved', $state->num_solved());
}

sub trace_wrapper
{
    my $state = shift;
    $state->main()->trace(
        {
            'iters_quota' => $state->quota(),
            'selected_scan_idx' => $state->scan_idx(),
            'total_boards_solved' => $state->main()->total_boards_solved(),
        }
    );
}

sub register_params
{
    my $state = shift;

    $state->add_chosen();
    $state->mark_as_used();
    $state->update_total_boards_solved();
    $state->trace_wrapper();
}

package Shlomif::FCS::CalcMetaScan;

use strict;
use warnings;

use Error qw(:try);

use Shlomif::FCS::CalcMetaScan::Structs;

use base 'Shlomif::FCS::CalcMetaScan::Base';

use vars (qw(@fields %fields_map));

@fields = (qw(
    chosen_scans
    num_boards
    orig_scans_data
    scans_data
    selected_scans
    status
    quotas
    total_boards_solved
    total_iters
    trace_cb
));

%fields_map = (map { $_ => 1 } @fields);

__PACKAGE__->mk_accessors(@fields);

sub add
{
    my ($self, $field, $diff) = @_;
    if (!exists($fields_map{$field}))
    {
        die "Cannot add to field \"$field\"!";
    }
    $self->set($field, $self->get($field) + $diff);
    return $self->get($field);
}

sub initialize
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

    return 0;
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

sub get_iter_state_params
{
    my ($self, $quotas) = @_;

    my $iters_quota = 0;
    my $num_solved_in_iter = 0;
    my $selected_scan_idx;

    # If no boards were solved, then try with a larger quota
    while ($num_solved_in_iter == 0)
    {
        my $q_more = shift(@$quotas);
        if (!defined($q_more))
        {
            throw Shlomif::FCS::CalcMetaScan::Error::OutOfQuotas;
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
        (
            quota => $iters_quota,
            num_solved => $num_solved_in_iter,
            scan_idx => $selected_scan_idx,
        );
}

sub get_selected_scan
{
    my ($self, $quotas) = @_;

    return
        Shlomif::FCS::CalcMetaScan::IterState->new(
            main => $self,
            $self->get_iter_state_params($quotas),
        );
}

sub inspect_quota
{
    my ($self, $quotas) = @_;

    my $state = $self->get_selected_scan($quotas);

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
    my $quotas = [ @{$self->get_quotas()} ];
    # $self->inspect_quota() throws ::Error::OutOfQuotas if
    # it does not have any available quotas.
    try
    {
        while ($self->status() eq "iterating")
        {
            $self->inspect_quota($quotas);
        }
    }
    catch Shlomif::FCS::CalcMetaScan::Error::OutOfQuotas with
    {
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

