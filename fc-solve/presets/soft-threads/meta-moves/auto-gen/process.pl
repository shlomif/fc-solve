#!/usr/bin/perl -w

use strict;
#use File::stat;

# use PDL;

my @scans;

my $num_boards = 32000;

open I, "<scans.txt";
while (my $line = <I>)
{
    my ($id, $cmd_line) = split(/\t/, $line);
    push @scans, { 'id' => $id, 'cmd_line' => $cmd_line };
}
close(I);

my @selected_scans = 
    grep 
    { 
        my @stat = stat("./data/".$_->{'id'}.".data.bin");
        scalar(@stat) && ($stat[7] == 12+$num_boards*4);
    }
    @scans;
    
my $scans_data = [];
    
my $scan_idx = 0;
foreach my $scan (@selected_scans)
{
    print "scan_idx=$scan_idx\n";
    open I, "<./data/" . $scan->{'id'} .  ".data.bin";
    my $data_s = join("", <I>);
    close(I);
    my @array = unpack("l*", $data_s);
    if (($array[0] != 1) || ($array[1] != $num_boards) || ($array[2] != 100000))
    {
        die "Incorrect file format in scan " . $scan->{'id'} . "!\n";
    }
    #my $b = $scans_data->slice("$scan_idx,:");
    #my @b_dims = $b->dims();
    #print "dims=" . join(",", @b_dims) . "\n";
    #my $c = pdl(\@array);
    #$b += $c->slice("3:".($num_boards+2));
    #$scan_idx++;
    push @$scans_data, [ @array[3 .. $#array] ];
    $scan_idx++;
}


my @quotas = ((500) x 1000);

my $q = $quotas[0];
print join(",", map { my $sd = $_; scalar(grep { ($_ >= 0) && ($_ < $q) } @{$sd}) } @$scans_data);

