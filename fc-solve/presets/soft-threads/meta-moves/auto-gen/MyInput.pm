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

1;
