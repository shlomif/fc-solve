package FC_Solve::DepthMultiQueuePrototype;

use strict;
use warnings;
use Games::Solitaire::FC_Solve::QueueInC;

use parent 'Games::Solitaire::Verify::Base';

__PACKAGE__->mk_acc_ref([qw(
        _num_items_per_page
        _offload_dir_path
        _num_inserted
        _num_items_in_queue
        _num_extracted
        _min_depth
        _max_depth
        _queues_by_depth
        _next_queue_id
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
    $self->_num_extracted(0);

    my $first_depth = $args->{first_depth};
    my $first_item = $args->{first_item};

    $self->_next_queue_id(0);

    $self->_min_depth($first_depth);

    $self->_queues_by_depth(
        [
            $self->_new_queue(),
        ]
    );

    $self->_max_depth($first_depth);

    $self->insert($first_depth, $first_item);

    return;
}

sub _get_next_queue_id
{
    my ($self) = @_;

    my $ret = $self->_next_queue_id();

    $self->_next_queue_id($ret + 1);

    return $ret;
}

sub _new_queue
{
    my ($self) = @_;

    return Games::Solitaire::FC_Solve::QueueInC->new(
        {
            num_items_per_page => $self->_num_items_per_page,
            offload_dir_path => $self->_offload_dir_path,
            queue_id => $self->_get_next_queue_id(),
        }
    );
}

sub insert
{
    my ($self, $depth, $item) = @_;

    while ($depth > $self->_max_depth())
    {
        push @{$self->_queues_by_depth()}, $self->_new_queue();
        $self->_max_depth($self->_max_depth + 1);
    }

    $self->_queues_by_depth->[$depth-$self->_min_depth()]->insert($item);

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

sub get_num_extracted
{
    my $self = shift;

    return $self->_num_extracted();
}

sub extract
{
    my $self = shift;

    if ($self->_num_items_in_queue() == 0)
    {
        return;
    }

    while ($self->_queues_by_depth->[0]->get_num_items_in_queue() == 0)
    {
        my $save_queue = shift(@{ $self->_queues_by_depth });
        push @{$self->_queues_by_depth()}, $save_queue;
        $self->_min_depth($self->_min_depth + 1);
        $self->_max_depth($self->_max_depth + 1);
    }

    $self->_num_items_in_queue($self->_num_items_in_queue() - 1);
    $self->_num_extracted($self->_num_extracted() + 1);

    return ($self->_min_depth, $self->_queues_by_depth->[0]->extract());
}

1;

