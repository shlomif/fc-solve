#!/usr/bin/perl -w

package Shlomif::FCS::CalcMetaScan;

use strict;
use warnings;

use PDL ();

use base 'Class::Accessor';

my @fields = (qw(
    chosen_scans
    iters_quota
    num_boards
    scans_data
    selected_scans
    status
    quotas
    total_boards_solved
    total_iters
));

my %fields_map = (map { $_ => 1 } @fields);

__PACKAGE__->mk_accessors(@fields);

sub new
{
    my $class = shift;
    my $self = {};
    bless $self, $class;
    $self->initialize(@_);
    return $self;
}

sub add
{
    my ($self, $field, $diff) = @_;
    if (!exists($fields_map{$field}))
    {
        die "Cannot add to field \"$field\"!";
    }
    $self->set($field, $self->get($field) + $diff);
    return $self->get($field);
}

sub initialize
{
    my $self = shift;

    my (%args) = (@_);

    $self->quotas($args{'quotas'}) or
        die "Quotas not specified!";

    if (!exists($args{'scans_data'}))
    {
        die "scans_data not specified!";
    }

    $self->scans_data($args{'scans_data'}->copy());

    $self->selected_scans($args{'selected_scans'}) or
        die "selected_scans not specified!";

    $self->num_boards($args{'num_boards'}) or
        die "num_boards not specified!";

    return 0;
}

sub scans_rle
{
    my $self = shift;
    my %args = (@_);
    my $scans_list = [@{$args{'scans'}}];

    my $scan = shift(@$scans_list);

    my (@a);
    while (my $next_scan = shift(@$scans_list))
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
    return \@a;
}

sub inspect_quota
{
    my $self = shift;
    my %args = (@_);
    my $q_more = $args{'q_more'};

    my $iters_quota = $self->iters_quota($self->iters_quota() + $q_more);

    my (undef, $num_solved_in_iter, undef, $selected_scan_idx) =
        PDL::minmaximum(
            PDL::sumover(
                ($self->scans_data() <= $iters_quota) & 
                ($self->scans_data() > 0)
            )
          );

    # If no boards were solved, then try with a larger quota
    if ($num_solved_in_iter == 0)
    {
        return;
    }

    push @{$self->chosen_scans()}, { 'q' => $iters_quota, 'ind' => $selected_scan_idx };
    $self->selected_scans()->[$selected_scan_idx]->{'used'} = 1;

    my $total_boards_solved = 
        $self->add('total_boards_solved', $num_solved_in_iter);
    print "$iters_quota \@ $selected_scan_idx ($total_boards_solved solved)\n";

    my $this_scan_result = ($self->scans_data()->slice(":,$selected_scan_idx"));
    $self->add('total_iters', PDL::sum((($this_scan_result <= $iters_quota) & ($this_scan_result > 0)) * $this_scan_result));
    my $indexes = PDL::which(($this_scan_result > $iters_quota) | ($this_scan_result < 0));
    
    $self->add('total_iters', ($indexes->nelem() * $iters_quota));
    if ($total_boards_solved == $self->num_boards())
    {
        print "Solved all!\n";
        $self->status("solved_all");
        return;
    }    
    
    $self->scans_data(
        $self->scans_data()->dice($indexes, "X")->copy()
    );
    $this_scan_result = $self->scans_data()->slice(":,$selected_scan_idx")->copy();
    #$scans_data->slice(":,$selected_scan_idx") *= 0;
    $self->scans_data()->slice(":,$selected_scan_idx") .= (($this_scan_result - $iters_quota) * ($this_scan_result > 0)) +
        ($this_scan_result * ($this_scan_result < 0));

    $self->iters_quota(0);
}

sub get_quotas
{
    my $self = shift;
    return $self->quotas();
}

sub calc_meta_scan
{
    my $self = shift;

    $self->chosen_scans([]);

    $self->total_boards_solved(0);
    $self->iters_quota(0);
    $self->total_iters(0);

    $self->status("iterating");
    QUOTA_LOOP: foreach my $q_more (@{$self->get_quotas()})
    {
        $self->inspect_quota('q_more' => $q_more);
        if ($self->status() eq "solved_all")
        {
            last QUOTA_LOOP;
        }
    }
}

sub do_rle
{
    my $self = shift;
    $self->chosen_scans(
        $self->scans_rle('scans' => $self->chosen_scans())
    );
}

package main;

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

if ($script_filename eq "-")
{
    open SCRIPT, ">&STDOUT";
}
else
{
    open SCRIPT, ">$script_filename";    
}


# Construct the command line
my $cmd_line = "freecell-solver-range-parallel-solve $start_board " . ($start_board + $num_boards - 1) . " 1 \\\n" .
    join(" -nst \\\n", map { $_->{'cmd_line'} . " -step 500 --st-name " . $_->{'id'} } (grep { $_->{'used'} } @$selected_scans)) . " \\\n" .
    "--prelude \"" . join(",", map { $_->{'q'} . "\@" . $selected_scans->[$_->{'ind'}]->{'id'} } @$chosen_scans) ."\"";
    
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
