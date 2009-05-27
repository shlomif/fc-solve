package MyInput;

use strict;
use warnings;

use Shlomif::FCS::CalcMetaScan::Structs;

use PDL;
use PDL::IO::FastRaw;

sub get_scans_data
{
    my $start_board = shift;
    my $num_boards = shift;
    my @selected_scans = @{shift()};
    my $scans_data = zeroes($num_boards, scalar(@selected_scans));
        
    my $scan_idx = 0;

    if (! -d "./.data-proc")
    {
        mkdir ("./.data-proc");
    }

    foreach my $scan (@selected_scans)
    {
        print "scan_idx=$scan_idx\n";
        {
            my @orig_stat = stat("./data/" . $scan->id() .  ".data.bin");
            my @proc_stat = stat("./.data-proc/" . $scan->id());
            if ((! scalar(@proc_stat)) || $orig_stat[9] > $proc_stat[9])
            {
                open I, "<./data/" . $scan->id() .  ".data.bin";
                my $data_s = join("", <I>);
                close(I);
                my @array = unpack("l*", $data_s);
                if (($array[0] != 1) || ($array[1] < $num_boards) || ($array[2] != 100000))
                {
                    die "Incorrect file format in scan " . $scan->{'id'} . "!\n";
                }
                
                my $c = pdl(\@array);
                
                writefraw($c, "./.data-proc/" . $scan->id());
            }
        }
        my $c = readfraw("./.data-proc/" . $scan->id());
        my $b = $scans_data->slice(":,$scan_idx");
        $b += $c->slice((2+$start_board).":".($num_boards+1+$start_board));
        $scan_idx++;
    }

    return $scans_data;

}

sub get_selected_scan_list
{
    my $start_board = shift;
    my $num_boards = shift;

    local(*I);
    my @scans;

    open I, "<scans.txt";
    while (my $line = <I>)
    {
        chomp($line);
        my ($id, $cmd_line) = split(/\t/, $line);
        push @scans, 
            Shlomif::FCS::CalcMetaScan::Structs::Scan->new( 
                id => $id, 
                cmd_line => $cmd_line
            );
    }
    close(I);

    my @selected_scans = 
        grep 
        { 
            my @stat = stat("./data/".$_->id().".data.bin");
            scalar(@stat) && ($stat[7] >= 12+($num_boards+$start_board-1)*4);
        }
        @scans;

    my %black_list = (map { $_ => 0 } (7,8));
    @selected_scans = 
        (grep 
            {
                !exists($black_list{$_->id()}) 
            }
            @selected_scans
        );
    return \@selected_scans;
}

sub get_next_id
{
    my $id;

    open my $in, "<", "next-id.txt";
    $id = <I>;
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
        qw(freecell-solver-range-parallel-solve), 
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
    my (@fcs_moves, @fc_pro_moves);
    while (my $line = <$from_cmd>)
    {
        chomp($line);
        if ($line =~ m{\A\[\[Num FCS Moves\]\]=(.*)\z}o)
        {
            push @fcs_moves, $1;
        }
        elsif ($line =~ m{\A\[\[Num FCPro Moves\]\]=(.*)\z}o)
        {
            push @fc_pro_moves, $1;
        }
    }
    close($from_cmd);
    if ((@fcs_moves != ($max_board - $min_board + 1))
            ||
        (@fc_pro_moves != ($max_board - $min_board + 1))
    )
    {
        die "Incorrect number of FCS Moves or FC-Pro Moves";
    }
    {
        open my $out, ">", "data/$id.fcs.moves.txt";
        print {$out} join("\n", @fcs_moves, "");
        close($out);
    }
    {
        open my $out, ">", "data/$id.fcpro.moves.txt";
        print {$out} join("\n", @fc_pro_moves, "");
        close($out);
    }
}

1;
