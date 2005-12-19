package Shlomif::FCS::CalcMetaScan::CmdLine;

use strict;
use warnings;

use base 'Shlomif::FCS::CalcMetaScan::Base';

use Shlomif::FCS::CalcMetaScan;

use Getopt::Long;

use MyInput;

__PACKAGE__->mk_accessors(qw(
    arbitrator
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

sub get_used_scans
{
    my $self = shift;
    return [ grep { $_->{'used'} } @{$self->selected_scans()}];
}

sub get_lines_of_scan_defs
{
    my $self = shift;
    return 
        [map 
            { $_->{'cmd_line'} . " -step 500 --st-name " . $_->{'id'} } 
            @{$self->get_used_scans()}
        ];
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
            map { $self->format_prelude_iter($_) } @{$self->get_chosen_scans()}
        ) . "\"";
}

sub calc_script_text
{
    my $self = shift;
    return 
        join("",
            @{$self->line_ends_mapping(
                [
                    $self->get_line_of_command(),
                    @{$self->scan_def_line_mapping(
                        $self->get_lines_of_scan_defs()
                    )},
                    $self->get_line_of_prelude()
                ]
            )}
        );
}

sub write_script
{
    my $self = shift;
     
    $self->out_script(
        $self->calc_script_text()
    );
}

sub calc_scans_data
{
    my $self = shift;
    return
        MyInput::get_scans_data(
            $self->start_board(),
            $self->num_boards(), 
            $self->selected_scans()
        );
}

sub init_arbitrator
{
    my $self = shift;

    $self->selected_scans(
        MyInput::get_selected_scan_list(
            $self->start_board(),
            $self->num_boards(),
        )
    );

    return $self->arbitrator(
        Shlomif::FCS::CalcMetaScan->new(
            'quotas' => $self->get_quotas(),
            'selected_scans' => $self->selected_scans(),
            'num_boards' => $self->num_boards(),
            'scans_data' => $self->calc_scans_data(),
        )
    );
}

sub get_chosen_scans
{
    my $self = shift;
    return $self->arbitrator()->chosen_scans();
}

sub report_total_iters
{
    my $self = shift;
    printf("total_iters = %s\n", $self->arbitrator()->total_iters());
}

sub run
{
    my $self = shift;

    $self->init_arbitrator();

    $self->arbitrator()->calc_meta_scan();

    if ($self->rle())
    {
        $self->arbitrator()->do_rle();    
    }

    $self->report_total_iters();

    $self->write_script();

    # Analyze the results

    if ($self->trace())
    {
        foreach my $board (0 .. $self->num_boards()-1)
        {
            my $results = $self->arbitrator()->calc_board_iters($board);
            print "\@info=". join(",", @{$results->{per_scan_iters}}). "\n";
            print +($board+$self->start_board()) . ": ". $results->{board_iters} . "\n";
        }
    }

    return 0;
}

1;

