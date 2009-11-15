package MyInput;

use strict;
use warnings;

use File::Path;

use base 'Shlomif::FCS::CalcMetaScan::Base';

use Shlomif::FCS::CalcMetaScan::Structs;

use PDL;
use PDL::IO::FastRaw;

sub _init
{
    my $self = shift;

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
    my $filename = shift;
    my $text = _slurp($filename);

    return [split(/[\n\r]+/, $text)];
}

sub _get_scans_data_helper
{
    my $self = shift;

    my $start_board = shift;
    my $num_boards = shift;
    my $selected_scans = shift;

    my $scans_data = zeroes($num_boards, scalar(@$selected_scans));
    my $scans_lens_data = zeroes($num_boards, scalar(@$selected_scans), 3);
        
    my $scan_idx = 0;

    my $data_dir = ".data-proc";
    my $lens_dir = ".data-len-proc";

    mkpath($data_dir, $lens_dir);

    foreach my $scan (@$selected_scans)
    {
        print "scan_idx=$scan_idx\n";
        {
            my @orig_stat = stat("./data/" . $scan->id() .  ".data.bin");
            my @proc_stat = stat("$data_dir/" . $scan->id());
            if ((! scalar(@proc_stat)) || $orig_stat[9] > $proc_stat[9])
            {
                my $data_s = _slurp("./data/" . $scan->id() .  ".data.bin");
                my @array = unpack("l*", $data_s);
                if (($array[0] != 1) || ($array[1] < $num_boards) || ($array[2] != 100000))
                {
                    die "Incorrect file format in scan " . $scan->{'id'} . "!\n";
                }
                
                my $c = pdl(\@array);
                
                writefraw($c, "./.data-proc/" . $scan->id());
            }
        }
        {
            my $c = readfraw("./.data-proc/" . $scan->id());
            my $b = $scans_data->slice(":,$scan_idx");
            $b += $c->slice((2+$start_board).":".($num_boards+1+$start_board));
        }
        {
            my $src = "./data/" . $scan->id() .  ".data.bin";
            my $dest = "$lens_dir/" . $scan->id();

            my @orig_stat = stat($src);
            my @proc_stat = stat($dest);

            if ((! scalar(@proc_stat)) || $orig_stat[9] > $proc_stat[9])
            {
                my $data_s = _slurp($src);

                my @iters = unpack("l*", $data_s);
                if (($iters[0] != 1) || ($iters[1] < $num_boards) 
                    || ($iters[2] != 100000)
                )
                {
                    die "Incorrect file format in scan " . $scan->id() . "!\n";
                }

                # Remove the header
                splice @iters, 0, 3;

                my $c = pdl(
                    [\@iters, 
                    _read_text_ints_file(
                        "data/" . $scan->id() . ".fcs.moves.txt"
                    ),
                    _read_text_ints_file(
                        "data/" . $scan->id() . ".fcpro.moves.txt"
                    ),
                    ]
                );
                
                writefraw($c, $dest);
            }
        }
        {
            my $c = readfraw("$lens_dir/" . $scan->id());
            my $b = $scans_lens_data->slice(":,$scan_idx,:");
            $b += $c->slice(
                sprintf(
                    "%d:%d,:,*",
                    ($start_board-1),
                    (($num_boards-1)+($start_board-1))
                )
            )->xchg(1,2);
        }
        $scan_idx++;
    }

    return { 'scans' => $scans_data, 'with_lens' => $scans_lens_data };
}

sub get_scans_data
{
    my $self = shift;

    return $self->_get_scans_data_helper(@_)->{'scans'};
}

sub get_scans_lens_data
{
    my $self = shift;

    return $self->_get_scans_data_helper(@_)->{'with_lens'};
}

sub _filter_scans_based_on_black_list_ids
{
    my ($selected_scans, $black_list_ids) = @_;

    my %black_list = (map { /(\d+)/?($1 => 1) : () } @$black_list_ids);

    return
        [grep 
            {
                !exists($black_list{$_->id()}) 
            }
            @$selected_scans
        ];
}

sub _is_scan_suitable
{
    my ($self, $start_board, $num_boards, $scan) = @_;

    my @stat = stat("./data/".$scan->id().".data.bin");
    return (scalar(@stat) && ($stat[7] >= 12+($num_boards+$start_board-1)*4));
}

sub get_selected_scan_list
{
    my $self = shift;

    my $start_board = shift;
    my $num_boards = shift;

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

    my @selected_scans = 
        grep 
        { 
            $self->_is_scan_suitable($start_board, $num_boards, $_)
        }
        @scans;

    open my $black_list_fh, "<", "scans-black-list.txt";
    my @black_list_ids = <$black_list_fh>;
    chomp(@black_list_ids);
    close($black_list_fh);

    return _filter_scans_based_on_black_list_ids(
        \@selected_scans,
        \@black_list_ids,
        );
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

    return
    [
        qw(freecell-solver-fc-pro-range-solve), 
        $min_board, $max_board, "20",
        qw(--total-iterations-limit 100000 --binary-output-to), 
        "data/$id.data.bin",
        @$argv,
    ];
}

sub time_scan
{
    my $args = shift;

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

1;
