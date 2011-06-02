package MyInput;

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
            my @orig_stat = stat("./data/" . $scan->{'id'} .  ".data.bin");
            my @proc_stat = stat("./.data-proc/" . $scan->{'id'});
            if ((! scalar(@proc_stat)) || $orig_stat[9] > $proc_stat[9])
            {
                open I, "<./data/" . $scan->{'id'} .  ".data.bin";
                my $data_s = join("", <I>);
                close(I);
                my @array = unpack("l*", $data_s);
                if (($array[0] != 1) || ($array[1] < $num_boards) || ($array[2] != 100000))
                {
                    die "Incorrect file format in scan " . $scan->{'id'} . "!\n";
                }
                
                my $c = pdl(\@array);
                
                writefraw($c, "./.data-proc/" . $scan->{'id'});
            }
        }
        my $c = readfraw("./.data-proc/" . $scan->{'id'});
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
        push @scans, { 'id' => $id, 'cmd_line' => $cmd_line };
    }
    close(I);

    my @selected_scans = 
        grep 
        { 
            my @stat = stat("./data/".$_->{'id'}.".data.bin");
            scalar(@stat) && ($stat[7] >= 12+($num_boards+$start_board-1)*4);
        }
        @scans;

    my %black_list = (map { $_ => 0 } ());
    @selected_scans = 
        (grep 
            { 
                !exists($black_list{$_->{'id'}}) 
            } 
            @selected_scans
        );
    return \@selected_scans;
}

1;
