#!/usr/bin/perl

package Shlomif::FCS::CalcMetaScan::CmdLine;

use strict;
use warnings;

use base 'Shlomif::FCS::CalcMetaScan::Base';

use Shlomif::FCS::CalcMetaScan;

use Getopt::Long;

use MyInput;

__PACKAGE__->mk_accessors(qw(
    chosen_scans
    num_boards
    output_filename
    quotas_expr
    rle
    selected_scans
    start_board
    trace
));

sub initialize
{
    my $self = shift;

    # Command line parameters
    my $start_board = 1;
    my $num_boards = 32000;
    my $output_filename = "-";
    my $trace = 0;
    my $rle = 1;
    my $quotas_expr = undef;

    GetOptions(
        "o|output=s" => \$output_filename,
        "num-boards=i" => \$num_boards,
        "trace" => \$trace,
        "rle" => \$rle,
        "start-board=i" => \$start_board,
        "quotas-expr=s" => \$quotas_expr,
    );

    $self->start_board($start_board);
    $self->num_boards($num_boards);
    $self->output_filename($output_filename);
    $self->trace($trace);
    $self->rle($rle);
    $self->quotas_expr($quotas_expr);    
}


sub map_all_but_last
{
    my $self = shift;
    my ($cb, $a) = (@_);
    return [ (map {$cb->($_)} @$a[0 .. $#$a-1]), $a->[-1] ];
}

sub get_quotas
{
    my $self = shift;
    if (defined($self->quotas_expr()))
    {
        return [eval $self->quotas_expr()];
    }
    else
    {
        return $self->get_default_quotas();
    }
}

sub get_default_quotas
{
    return [(350) x 5000];
}

sub out_script
{
    my $self = shift;
    my $cmd_line_string = shift;

    my $fh;
    if ($self->output_filename() eq "-")
    {
        open $fh, ">&STDOUT";
    }
    else
    {
        open $fh, ">", $self->output_filename();
    }
    print {$fh} $cmd_line_string, "\n\n\n";

    close($fh);
}

sub get_line_of_command
{
    my $self = shift;
    
    my $args_string = 
        join(" ", 
            $self->start_board(), 
            $self->start_board() + $self->num_boards() - 1, 
            1
        );
    return "freecell-solver-range-parallel-solve $args_string";
}

sub line_ends_mapping
{
    my $self = shift;
    return $self->map_all_but_last(sub { "$_[0] \\\n" }, shift);
}

sub get_lines_of_scan_defs
{
    my $self = shift;
    return [map { $_->{'cmd_line'} . " -step 500 --st-name " . $_->{'id'} } (grep { $_->{'used'} } @{$self->selected_scans()})];
}


sub scan_def_line_mapping
{
    my $self = shift;
    return $self->map_all_but_last(sub { "$_[0] -nst" }, shift);
}

sub format_prelude_iter
{
    my $self = shift;
    my $iter = shift;
    return $iter->{'q'} . '@' . $self->selected_scans()->[$iter->{'ind'}]->{'id'};
}

sub get_line_of_prelude
{
    my $self = shift;
    return "--prelude \"" .
        join(",",
            map { $self->format_prelude_iter($_) } @{$self->chosen_scans()}
        ) . "\"";
}

sub run
{
    my $self = shift;

    $self->selected_scans(
        MyInput::get_selected_scan_list(
            $self->start_board(),
            $self->num_boards(),
        )
    );

    my $scans_data = 
        MyInput::get_scans_data(
            $self->start_board(),
            $self->num_boards(), 
            $self->selected_scans()
        );

    my $meta_scanner =
        Shlomif::FCS::CalcMetaScan->new(
            'quotas' => $self->get_quotas(),
            'selected_scans' => $self->selected_scans(),
            'num_boards' => $self->num_boards(),
            'scans_data' => $scans_data,
        );

    $meta_scanner->calc_meta_scan();

    if ($self->rle())
    {
        $meta_scanner->do_rle();    
    }

    $self->chosen_scans($meta_scanner->chosen_scans());
    # print "scans_data = " , $scans_data, "\n";
    printf("total_iters = %s\n", $meta_scanner->total_iters());

    # Construct the command line
    my $cmd_line =
        join("",
            @{$self->line_ends_mapping(
                [
                    $self->get_line_of_command(),
                    @{$self->scan_def_line_mapping($self->get_lines_of_scan_defs())},
                    $self->get_line_of_prelude()
                ]
            )}
        );
     
    $self->out_script($cmd_line);

    # Analyze the results

    if ($self->trace())
    {
        foreach my $board (0 .. $self->num_boards()-1)
        {
            my $results = $meta_scanner->calc_board_iters($board);
            print ("\@info=". join(",", @{$results->{per_scan_iters}}). "\n");
            print (($board+$self->start_board()) . ": ". $results->{board_iters} . "\n");
        }
    }

    return 0;
}

1;

package main;

use strict;
use warnings;

my $iface = Shlomif::FCS::CalcMetaScan::CmdLine->new();
$iface->run();

