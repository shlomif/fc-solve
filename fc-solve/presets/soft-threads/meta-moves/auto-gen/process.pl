#!/usr/bin/perl

use strict;
use warnings;

use Shlomif::FCS::CalcMetaScan;

use Getopt::Long;

use MyInput;

# Command line parameters
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

sub get_quotas
{    
    if (defined($quotas_expr))
    {
        return [eval"$quotas_expr"];
    }
    else
    {
        return [(350) x 5000];
    }
}

sub out_script
{
    my $cmd_line_string = shift;

    my $fh;
    if ($script_filename eq "-")
    {
        open $fh, ">&STDOUT";
    }
    else
    {
        open $fh, ">", $script_filename;
    }
    print {$fh} $cmd_line_string, "\n\n\n";

    close($fh);
}

my $selected_scans = MyInput::get_selected_scan_list($start_board, $num_boards);

my $scans_data = MyInput::get_scans_data($start_board, $num_boards, $selected_scans);

my $orig_scans_data = $scans_data->copy();

my $meta_scanner =
    Shlomif::FCS::CalcMetaScan->new(
        'quotas' => get_quotas(),
        'selected_scans' => $selected_scans,
        'num_boards' => $num_boards,
        'scans_data' => $scans_data,
    );

$meta_scanner->calc_meta_scan();

if ($rle)
{
    $meta_scanner->do_rle();    
}

my $chosen_scans = $meta_scanner->chosen_scans();
my $total_iters = $meta_scanner->total_iters();
    

# print "scans_data = " , $scans_data, "\n";
print "total_iters = $total_iters\n";



# Construct the command line
my $cmd_line = "freecell-solver-range-parallel-solve $start_board " . ($start_board + $num_boards - 1) . " 1 \\\n" .
    join(" -nst \\\n", map { $_->{'cmd_line'} . " -step 500 --st-name " . $_->{'id'} } (grep { $_->{'used'} } @$selected_scans)) . " \\\n" .
    "--prelude \"" . join(",", map { $_->{'q'} . "\@" . $selected_scans->[$_->{'ind'}]->{'id'} } @$chosen_scans) ."\"";
 
out_script($cmd_line);

# Analyze the results

if ($trace)
{
    foreach my $board (1 .. $num_boards)
    {
        my $total_iters = 0;
        my @info = PDL::list($orig_scans_data->slice(($board-1).",:"));
        print ("\@info=". join(",", @info). "\n");
        foreach my $s (@$chosen_scans)
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
