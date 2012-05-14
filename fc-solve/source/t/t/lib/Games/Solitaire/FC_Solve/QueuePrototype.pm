package Games::Solitaire::FC_Solve::QueuePrototype;

use strict;
use warnings;

use base 'Games::Solitaire::Verify::Base';

__PACKAGE__->mk_acc_ref([qw(
        _num_items_per_page
        _offload_dir_path
        _num_inserted
        _num_items_in_queue
        _num_extracted
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
    $self->_num_extracted(0);

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

    if ($self->_num_items_in_queue() == 0)
    {
        return;
    }

    $self->_num_items_in_queue($self->_num_items_in_queue() - 1);
    $self->_num_extracted($self->_num_extracted() + 1);

    return shift(@{$self->_temp_q});
}

sub get_num_extracted
{
    my $self = shift;

    return $self->_num_extracted();
}

package Games::Solitaire::FC_Solve::QueuePrototype::Page;

use strict;
use warnings;

use Storable qw(nstore retrieve);

use base 'Games::Solitaire::Verify::Base';

__PACKAGE__->mk_acc_ref([qw(
        _num_items_per_page
        _page_index
        _write_to_idx
        _read_from_idx
        _data
        )]);

sub _init
{
    my $self = shift;
    my $args = shift;

    $self->_num_items_per_page($args->{num_items_per_page});
    $self->_page_index($args->{page_index});

    $self->_write_to_idx(0);
    $self->_read_from_idx(0);

    $self->_data([(undef) x $self->_num_items_per_page()]);

    return;
}

sub can_extract
{
    my $self = shift;

    return $self->_read_from_idx < $self->_write_to_idx;
}

sub extract
{
    my $self = shift;

    my $read_from_idx = $self->_read_from_idx();
    $self->_read_from_idx( $read_from_idx + 1);

    return $self->_data->[$read_from_idx];
}

sub can_insert
{
    my $self = shift;

    return ($self->_write_to_idx < $self->_num_items_per_page);
}

sub insert
{
    my ($self, $item) = @_;

    my $write_to_idx = $self->_write_to_idx;

    $self->_data->[$write_to_idx] = $item;

    $self->_write_to_idx($write_to_idx + 1);

    return;
}

sub get_page_index
{
    my $self = shift;

    return $self->_page_index;
}

sub _calc_filename
{
    my ($self, $offload_dir_path) = @_;

    return File::Spec->catfile($offload_dir_path, 
        sprintf("fcs_queue_%020X.page", $self->get_page_index())
    );
}

sub read_from_disk
{
    my ($class, $args, $offload_dir_path) = @_;

    my $self = $class->new($args);

    my $page_filename =$self->_calc_filename($offload_dir_path);

    $self->_data( retrieve( $page_filename ));

    unlink($page_filename);

    return $self;
}

sub offload
{
    my $self = shift;
    my $offload_dir_path = shift;

    my $page_filename = $self->_calc_filename($offload_dir_path);

    nstore($self->_data(), $page_filename);

    return;
}

1;

