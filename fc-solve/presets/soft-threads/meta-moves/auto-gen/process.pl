#!/usr/bin/perl -w

use strict;
#use File::stat;

use PDL;

my $num_boards = 32000;

my $arg = shift(@ARGV);
my $script_filename = "-";
if ($arg eq "-o")
{
    $script_filename = shift;
}


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
        scalar(@stat) && ($stat[7] == 12+$num_boards*4);
    }
    @scans;
    
#my $scans_data = [];
my $scans_data = zeroes($num_boards, scalar(@selected_scans));
    
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
    my $b = $scans_data->slice(":,$scan_idx");    
    my $c = pdl(\@array);
    $b += $c->slice("3:".($num_boards+2));
    $scan_idx++;
}


my @quotas = ((100) x 5000);
my @chosen_scans = ();

my $total;
my $q = 0;
my $total_iters = 0;

foreach my $q_more (@quotas)
{
    $q += $q_more;
    my $num_solved = sumover(($scans_data <= $q) & ($scans_data > 0));
    my ($min, $max, $min_ind, $max_ind) = minmaximum($num_solved);
    if ($max == 0)
    {
        next;
    }
    push @chosen_scans, { 'q' => $q, 'ind' => $max_ind };
    $total += $max;
    print "$q \@ $max_ind ($total solved)\n";
    my $this_scan_result = ($scans_data->slice(":,$max_ind"));
    $total_iters += sum((($this_scan_result <= $q) & ($this_scan_result > 0)) * $this_scan_result);
    my $indexes = which(($this_scan_result > $q) | ($this_scan_result < 0));
    $total_iters += $indexes->nelem();
    $scans_data = $scans_data->dice($indexes, "X");
    $this_scan_result = $scans_data->slice(":,$max_ind");
    $this_scan_result = (($this_scan_result - $q) * ($this_scan_result > 0)) +
        ($this_scan_result * ($this_scan_result < 0));

    if ($total == $num_boards)
    {
        print "Solved all!\n";
        last;
    }
    $q = 0;
}

print "scans_data = " , $scans_data, "\n";
print "total_iters = $total_iters\n";

if ($script_filename eq "-")
{
    open SCRIPT, ">&STDOUT";
}
else
{
    open SCRIPT, ">$script_filename";    
}


# Construct the command line
my $cmd_line = "freecell-solver-range-parallel-solve 1 $num_boards 20 \\\n" .
    join("", map { $_->{'cmd_line'} . " -step 500 --st-name " . $_->{'id'} . " \\\n" } @selected_scans) .
    "--prelude \"" . join(",", map { $_->{'q'} . "\@" . $_->{'ind'} } @chosen_scans) ."\"";
    
print SCRIPT $cmd_line;

close(SCRIPT);



