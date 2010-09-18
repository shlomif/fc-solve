package MyInput;

use strict;
use warnings;

use File::Path;
use List::Util ();

use base 'Shlomif::FCS::CalcMetaScan::Base';

use Shlomif::FCS::CalcMetaScan::Structs;

use PDL;
use PDL::IO::FastRaw;

__PACKAGE__->mk_acc_ref(
    [qw(
        start_board
        num_boards
        selected_scans
        _scans_ids_to_indexes_map
    )],
);

sub _init
{
    my $self = shift;
    my $args = shift;

    $self->start_board($args->{'start_board'});
    $self->num_boards($args->{'num_boards'});

    $self->_calc_selected_scan_list();

    return;
}

sub _slurp
{
    my $filename = shift;

    open my $in, "<", $filename
        or die "Could not open $filename";
    
    binmode $in;
    local $/;
    my $content = <$in>;
    close($in);
    return $content;
}

sub _read_text_ints_file
{
    my $self = shift;

    my $filename = shift;

    my $text = _slurp($filename);

    return [split(/[\n\r]+/, $text)];
}

# Number of selected scans.
sub _num_sel_scans
{
    my $self = shift;

    return scalar(@{$self->selected_scans()});
}

sub _gen_initial_scans_tensor
{
    my $self = shift;
    my $extra_dims = shift || [];

    return zeroes($self->num_boards(), $self->_num_sel_scans, @$extra_dims);
}

sub _should_update
{
    my ($self, $src_path, $dest_path) = @_;

    my @orig_stat = stat($src_path);
    my @proc_stat = stat($dest_path);

    return ((! @proc_stat) || ($orig_stat[9] > $proc_stat[9]));
}

# Number of numbers in the header of the solutions' iteration counts
my $NUM_NUMBERS_IN_HEADER = 3;

my $HEADER_START_BOARD_IDX = 0;
my $HEADER_NUM_BOARDS = 1;
my $HEADER_ITERATIONS_LIMIT = 2;

sub _get_scans_data_helper
{
    my $self = shift;

    my $selected_scans = $self->selected_scans();

    my $start_board = $self->start_board();

    my $scans_data = $self->_gen_initial_scans_tensor();
    my $scans_lens_data = $self->_gen_initial_scans_tensor([3]);
        
    my $scan_idx = 0;

    my $data_dir = ".data-proc";
    my $lens_dir = ".data-len-proc";

    foreach my $dir ($data_dir, $lens_dir)
    {
        mkpath($dir);
    }

    foreach my $scan (@$selected_scans)
    {
        {
            my $dest_path = $data_dir . "/" . $scan->id();
            if ($self->_should_update($scan->data_file_path(), $dest_path))
            {
                my $data_s = _slurp($scan->data_file_path());
                my @array = unpack("l*", $data_s);
                if (($array[$HEADER_START_BOARD_IDX] != 1) || 
                    ($array[$HEADER_NUM_BOARDS] < $self->num_boards) || 
                    ($array[$HEADER_ITERATIONS_LIMIT] != 100000)
                   )
                {
                    die "Incorrect file format in scan " . $scan->{'id'} . "!\n";
                }
                
                my $c = pdl(\@array);
                
                writefraw($c, $dest_path);
            }
        }
        {
            my $scan_vec = readfraw("./.data-proc/" . $scan->id());
            my $scans_data_slice = $scans_data->slice(":,$scan_idx");
            # Board No. 1 starts at index 0.
            my $start_idx = $NUM_NUMBERS_IN_HEADER + ($start_board - 1);
            $scans_data_slice += $scan_vec->slice(
                $start_idx.":".($start_idx + $self->num_boards()-1)
            );
        }
        {
            my $src = $scan->data_file_path();
            my $dest = "$lens_dir/" . $scan->id();

            if ($self->_should_update($src, $dest))
            {
                my $data_s = _slurp($src);

                my @iters = unpack("l*", $data_s);
                if (($iters[0] != 1) || ($iters[1] < $self->num_boards()) 
                    || ($iters[2] != 100000)
                )
                {
                    die "Incorrect file format in scan " . $scan->id() . "!\n";
                }

                # Remove the header
                splice @iters, 0, $NUM_NUMBERS_IN_HEADER;

                my $c = pdl(
                    [\@iters, 
                    $self->_read_text_ints_file(
                        "data/" . $scan->id() . ".fcs.moves.txt"
                    ),
                    $self->_read_text_ints_file(
                        "data/" . $scan->id() . ".fcpro.moves.txt"
                    ),
                    ]
                );
                
                writefraw($c, $dest);
            }
        }
        {
            my $scan_vec = readfraw("$lens_dir/" . $scan->id());
            my $scans_data_slice = $scans_lens_data->slice(":,$scan_idx,:");
            $scans_data_slice += $scan_vec->slice(
                sprintf(
                    "%d:%d,:,*",
                    ($start_board-1),
                    (($self->num_boards()-1)+($start_board-1))
                )
            )->xchg(1,2);
        }
    }
    continue
    {
        $scan_idx++;
    }

    return { 'scans' => $scans_data, 'with_lens' => $scans_lens_data };
}

sub _get_scans_data_generic
{
    my ($self, $id) = @_;

    return $self->_get_scans_data_helper()->{$id};
}

sub get_scans_data
{
    my $self = shift;

    return $self->_get_scans_data_generic('scans');
}

sub get_scans_lens_data
{
    my $self = shift;

    return $self->_get_scans_data_generic('with_lens');
}

sub _filter_scans_based_on_black_list_ids
{
    my ($scans, $black_list_ids) = @_;

    my %black_list = (map { /(\d+)/?($1 => 1) : () } @$black_list_ids);

    return
        [grep 
            {
                !exists($black_list{$_->id()}) 
            }
            @$scans
        ];
}

sub _is_scan_suitable
{
    my ($self, $scan) = @_;

    my @stat = stat($scan->data_file_path());
    return
    (
        scalar(@stat)
            && 
        ($stat[7] >= 12 + ($self->num_boards() + $self->start_board() -1) * 4)
    );
}

sub _get_all_scans_list_from_file
{
    my $self = shift;

    my @scans;

    open my $scans_fh, "<", "scans.txt"
        or die "Could not open 'scans.txt' - $!.";
    while (my $line = <$scans_fh>)
    {
        chomp($line);
        my ($id, $cmd_line) = split(/\t/, $line);
        push @scans, 
            Shlomif::FCS::CalcMetaScan::Structs::Scan->new( 
                id => $id, 
                cmd_line => $cmd_line
            );
    }
    close($scans_fh);

    return \@scans;
}

sub _black_list_ids_list
{
    my $self = shift;
    
    open my $black_list_fh, "<", "scans-black-list.txt"
        or die "Could not open 'scans-black-list.txt'! $!.";
    my @black_list_ids = <$black_list_fh>;
    chomp(@black_list_ids);
    close($black_list_fh);

    return \@black_list_ids;
}

sub _suitable_scans_list
{
    my $self = shift;

    return
    [
        grep
        {
            $self->_is_scan_suitable($_)
        }
        @{$self->_get_all_scans_list_from_file()}
    ];
}

sub _selected_scans_indexes
{
    my $self = shift;

    return (0 .. $#{$self->selected_scans});
}


sub _calc_selected_scan_list
{
    my $self = shift;

    $self->selected_scans(
        _filter_scans_based_on_black_list_ids(
            $self->_suitable_scans_list(),
            $self->_black_list_ids_list(),
        )
    );

    $self->_scans_ids_to_indexes_map(
        {
            map { $self->selected_scans->[$_]->id() => $_ } 
            $self->_selected_scans_indexes()
        }
    );

    return;
}

=head2 $input->get_scan_index_by_id($id)

Returns the scan index by its ID.

=cut

sub get_scan_index_by_id
{
    my ($self, $id) = @_;

    return $self->_scans_ids_to_indexes_map->{$id};
}

sub get_next_id
{
    my $id;

    open my $in, "<", "next-id.txt";
    $id = <$in>;
    chomp($id);
    close($in);
    open my $out, ">", "next-id.txt";
    print $out ($id+1);
    close($out);

    return $id;
}

sub get_prev_scans
{
    my @prev_scans;
    open my $in, "<", "scans.txt";
    while (my $line = <$in>)
    {
        chomp($line);
        my ($scan_id, $cmd_line) = split(/\t/, $line);
        push @prev_scans, { 'id' => $scan_id, 'cmd_line' => $cmd_line };
    }
    close($in);

    return \@prev_scans;
}

sub get_scan_cmd_line
{
    my $args = shift;

    my $min_board = $args->{'min'} || 1;
    my $max_board = $args->{'max'} || 32_000;
    my $id = $args->{'id'};
    my $argv = $args->{'argv'};
    my @fc_num = 
        (exists($args->{'freecells_num'})
            ?  ("--freecells-num" , $args->{'freecells_num'}) 
            : ()
        );

    return
    [
        qw(freecell-solver-fc-pro-range-solve), 
        $min_board, $max_board, "20",
        qw(--total-iterations-limit 100000 --binary-output-to), 
        "data/$id.data.bin",
        @$argv,
        @fc_num,
    ];
}

sub time_scan
{
    my ($self, $args) = @_;

    my $min_board = $args->{'min'} || 1;
    my $max_board = $args->{'max'} || 32_000;
    my $id = $args->{'id'};

    my $cmd_line = get_scan_cmd_line($args);

    open my $from_cmd, "-|", @$cmd_line
        or die "Could not start '@$cmd_line'";
    open my $fcs_out, ">", "data/$id.fcs.moves.txt";
    open my $fc_pro_out, ">", "data/$id.fcpro.moves.txt";
    $fcs_out->autoflush(1);
    $fc_pro_out->autoflush(1);
    while (my $line = <$from_cmd>)
    {
        print $line;
        chomp($line);
        if ($line =~ m{\A\[\[Num FCS Moves\]\]=(.*)\z}o)
        {
            print {$fcs_out} "$1\n";
        }
        elsif ($line =~ m{\A\[\[Num FCPro Moves\]\]=(.*)\z}o)
        {
            print {$fc_pro_out} "$1\n";
        }
    }
    close($from_cmd);
    close($fcs_out);
    close($fc_pro_out);
}

=head2 $input_obj->scan_cmd_line_by_index($index)

Looks up the scan's command line by its index.

=cut

sub scan_cmd_line_by_index
{
    my ($self, $index) = @_;

    return $self->selected_scans->[$index]->cmd_line();
}


=head2 $input_obj->scan_id_by_index($index)

Looks up the scan's ID by its index.

=cut

sub scan_id_by_index
{
    my ($self, $index) = @_;

    return $self->selected_scans->[$index]->id();
}

=head2 my $index = $input_obj->get_scan_index_by_its_cmd_line($cmd_line)

Finds if there's a scan with the identical command line of $cmd_line .

=cut

sub get_scan_index_by_its_cmd_line
{
    my ($self, $cmd_line) = @_;

    return
    (
        List::Util::first { $self->selected_scans->[$_]->cmd_line() eq $cmd_line }
        $self->_selected_scans_indexes()
    );
}

1;
