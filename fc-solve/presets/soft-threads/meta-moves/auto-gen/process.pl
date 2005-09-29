#!/usr/bin/perl -w

use strict;
#use File::stat;

use PDL;
use PDL::IO::FastRaw;
use Getopt::Long;

use MyInput;


my $start_board = 1;
my $num_boards = 32000;
my $script_filename = "-";
my $trace = 0;
my $rle = 1;
my $quotas_expr = undef;

GetOptions(
    "o|output=s" => \$script_filename,
    "num-boards=i" => \$num_boards,
    "trace" => \$trace,
    "rle" => \$rle,
    "start-board=i" => \$start_board,
    "quotas-expr=s" => \$quotas_expr,
);

my $sel_scans = MyInput::get_selected_scan_list($start_board, $num_boards);
my @selected_scans = @$sel_scans;
    
#my $scans_data = [];
#my $scans_data = zeroes($num_boards, scalar(@selected_scans));

my $scans_data = MyInput::get_scans_data($start_board, $num_boards, \@selected_scans);

my $orig_scans_data = $scans_data->copy();

my @quotas;
if (defined($quotas_expr))
{
    @quotas = (eval"$quotas_expr");
}
else
{
    @quotas = ((350) x 5000);
}
my @chosen_scans = ();

my $total_boards_solved = 0;
my $iters_quota = 0;
my $total_iters = 0;

foreach my $q_more (@quotas)
{
    $iters_quota += $q_more;
    my (undef, $num_solved_in_iter, undef, $selected_scan_idx) =
        minmaximum(
            sumover(($scans_data <= $iters_quota) & ($scans_data > 0))
          );

    # If no boards were solved, then try with a larger quota
    if ($num_solved_in_iter == 0)
    {
        next;
    }

    push @chosen_scans, { 'q' => $iters_quota, 'ind' => $selected_scan_idx };
    $selected_scans[$selected_scan_idx]->{'used'} = 1;

    $total_boards_solved += $num_solved_in_iter;
    print "$iters_quota \@ $selected_scan_idx ($total_boards_solved solved)\n";

    my $this_scan_result = ($scans_data->slice(":,$selected_scan_idx"));
    $total_iters += sum((($this_scan_result <= $iters_quota) & ($this_scan_result > 0)) * $this_scan_result);
    my $indexes = which(($this_scan_result > $iters_quota) | ($this_scan_result < 0));
    
    $total_iters += ($indexes->nelem() * $iters_quota);
    if ($total_boards_solved == $num_boards)
    {
        print "Solved all!\n";
        last;
    }    
    
    $scans_data = $scans_data->dice($indexes, "X")->copy();
    $this_scan_result = $scans_data->slice(":,$selected_scan_idx")->copy();
    #$scans_data->slice(":,$selected_scan_idx") *= 0;
    $scans_data->slice(":,$selected_scan_idx") .= (($this_scan_result - $iters_quota) * ($this_scan_result > 0)) +
        ($this_scan_result * ($this_scan_result < 0));

    $iters_quota = 0;
}

# print "scans_data = " , $scans_data, "\n";
print "total_iters = $total_iters\n";

if ($script_filename eq "-")
{
    open SCRIPT, ">&STDOUT";
}
else
{
    open SCRIPT, ">$script_filename";    
}

if ($rle)
{
    my (@a);
    my $scan = shift(@chosen_scans);

    while (my $next_scan = shift(@chosen_scans))
    {
        if ($next_scan->{'ind'} == $scan->{'ind'})
        {
            $scan->{'q'} += $next_scan->{'q'};
        }
        else
        {
            push @a, $scan;
            $scan = $next_scan;
        }
    }
    push @a, $scan;
    @chosen_scans = @a;
}

# Construct the command line
my $cmd_line = "freecell-solver-range-parallel-solve $start_board " . ($start_board + $num_boards - 1) . " 1 \\\n" .
    join(" -nst \\\n", map { $_->{'cmd_line'} . " -step 500 --st-name " . $_->{'id'} } (grep { $_->{'used'} } @selected_scans)) . " \\\n" .
    "--prelude \"" . join(",", map { $_->{'q'} . "\@" . $selected_scans[$_->{'ind'}]->{'id'} } @chosen_scans) ."\"";
    
print SCRIPT $cmd_line;
print SCRIPT "\n\n\n";

close(SCRIPT);

# Analyze the results

if ($trace)
{
    foreach my $board (1 .. $num_boards)
    {
        my $total_iters = 0;
        my @info = list($orig_scans_data->slice(($board-1).",:"));
        print ("\@info=". join(",", @info). "\n");
        foreach my $s (@chosen_scans)
        {
            if (($info[$s->{'ind'}] > 0) && ($info[$s->{'ind'}] <= $s->{'q'}))
            {
                $total_iters += $info[$s->{'ind'}];
                last;
            }
            else
            {
                if ($info[$s->{'ind'}] > 0)
                {
                    $info[$s->{'ind'}] -= $s->{'q'};
                }
                $total_iters += $s->{'q'};
            }
        }
        print (($board+$start_board-1) . ": $total_iters\n");
    }
}
