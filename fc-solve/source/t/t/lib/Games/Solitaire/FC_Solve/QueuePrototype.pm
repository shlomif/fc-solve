package Games::Solitaire::FC_Solve::QueuePrototype;

use strict;
use warnings;

use base 'Games::Solitaire::Verify::Base';

__PACKAGE__->mk_acc_ref([qw(
        _num_items_per_page
        _offload_dir_path
        _num_inserted
        _num_items_in_queue
        _temp_q
        )]);

sub _init
{
    my $self = shift;
    my $args = shift;

    my $num_items_per_page = int($args->{num_items_per_page});

    if (not $num_items_per_page >= 1)
    {
        die "num_items_per_page must be greater or equal to 1.";
    }
    $self->_num_items_per_page($num_items_per_page);

    my $offload_dir_path = $args->{offload_dir_path};

    if (! -d $offload_dir_path)
    {
        die "offload_directory_path does not exist";
    }
    $self->_offload_dir_path($offload_dir_path);

    $self->_num_inserted(0);
    $self->_num_items_in_queue(0);
    $self->_temp_q([]);

    return;
}

sub insert
{
    my ($self, $item) = @_;

    push @{$self->_temp_q}, $item;

    $self->_num_inserted($self->_num_inserted() + 1);
    $self->_num_items_in_queue($self->_num_items_in_queue() + 1);

    return;
}

sub get_num_inserted
{
    my $self = shift;

    return $self->_num_inserted;
}

sub get_num_items_in_queue
{
    my $self = shift;

    return $self->_num_items_in_queue();
}

sub extract
{
    my $self = shift;

    $self->_num_items_in_queue($self->_num_items_in_queue() - 1);

    return shift(@{$self->_temp_q});
}

1;


