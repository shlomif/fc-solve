package Shlomif::FCS::CalcMetaScan::CmdLine;

use strict;
use warnings;

use base 'Shlomif::FCS::CalcMetaScan::Base';

use Getopt::Long;
use IO::File;

use Shlomif::FCS::CalcMetaScan;
use Shlomif::FCS::CalcMetaScan::PostProcessor;

use MyInput;

use Carp;

__PACKAGE__->mk_acc_ref(
    [qw(
    _arbitrator
    _chosen_scans
    _input_obj
    _num_boards
    _offset_quotas
    _optimize_for
    _output_filename
    _post_processor
    _quotas_are_cb
    _quotas_expr
    _should_rle_be_done
    _should_trace_be_done
    _simulate_to
    _start_board
    )]
);

sub _init
{
    my $self = shift;

    # Command line parameters
    my $_start_board = 1;
    my $num_boards = 32000;
    my $output_filename = "-";
    my $should_trace_be_done = 0;
    my $should_rle_be_done = 1;
    my $_quotas_expr = undef;
    my $quotas_are_cb = 0;
    my $optimize_for = "speed";
    my $offset_quotas = 0;
    my $simulate_to = undef;

    GetOptions(
        "o|output=s" => \$output_filename,
        "num-boards=i" => \$num_boards,
        "trace" => \$should_trace_be_done,
        "rle!" => \$should_rle_be_done,
        "start-board=i" => \$_start_board,
        "quotas-expr=s" => \$_quotas_expr,
        "quotas-are-cb" => \$quotas_are_cb,
        "offset-quotas" => \$offset_quotas,
        "opt-for=s" => \$optimize_for,
        "simulate-to=s" => \$simulate_to,
    ) or exit(1);

    $self->_start_board($_start_board);
    $self->_num_boards($num_boards);
    $self->_output_filename($output_filename);
    $self->_should_trace_be_done($should_trace_be_done);
    $self->_should_rle_be_done($should_rle_be_done);
    $self->_quotas_expr($_quotas_expr);
    $self->_quotas_are_cb($quotas_are_cb);
    $self->_optimize_for($optimize_for);
    $self->_offset_quotas($offset_quotas);
    $self->_simulate_to($simulate_to);

    $self->_input_obj(
        MyInput->new(
            {
                start_board => $self->_start_board(),
                num_boards => $self->_num_boards(),
            }
        )
    );

    $self->_post_processor(
        Shlomif::FCS::CalcMetaScan::PostProcessor->new(
            {
                do_rle => $self->_should_rle_be_done(),
                offset_quotas => $self->_offset_quotas(),
            }
        )
    );

    return;
}

sub _selected_scans
{
    my $self = shift;

    return $self->_input_obj->selected_scans();
}

sub _map_all_but_last
{
    my $self = shift;

    my ($cb, $arr_ref) = (@_);

    return [ (map {$cb->($_)} @$arr_ref[0 .. $#$arr_ref-1]), $arr_ref->[-1] ];
}

sub _get_quotas
{
    my $self = shift;
    if ($self->_quotas_are_cb())
    {
        return scalar(eval($self->_quotas_expr()));
    }
    elsif (defined($self->_quotas_expr()))
    {
        return [eval $self->_quotas_expr()];
    }
    else
    {
        return $self->_get_default_quotas();
    }
}

sub _get_default_quotas
{
    return [(350) x 5000];
}

sub _get_script_fh
{
    my $self = shift;
    return IO::File->new(
       ($self->_output_filename() eq "-") ?
           ">&STDOUT" :
           ($self->_output_filename(), "w")
       );
}

sub _get_script_terminator
{
    return "\n\n\n";
}

sub _out_script
{
    my $self = shift;
    my $cmd_line_string = shift;

    $self->_get_script_fh()->print(
        $cmd_line_string, 
        $self->_get_script_terminator($cmd_line_string)
    );
}

sub _get_line_of_command
{
    my $self = shift;
    
    my $args_string = 
        join(" ", 
            $self->_start_board(), 
            $self->_start_board() + $self->_num_boards() - 1, 
            1
        );
    return "freecell-solver-range-parallel-solve $args_string";
}

sub _line_ends_mapping
{
    my $self = shift;
    return $self->_map_all_but_last(sub { "$_[0] \\\n" }, shift);
}

sub _get_used_scans
{
    my $self = shift;
    return [ grep { $_->is_used() } @{$self->_selected_scans()}];
}

sub _get_lines_of_scan_defs
{
    my $self = shift;
    return 
        [map 
            { $_->{'cmd_line'} . " -step 500 --st-name " . $_->{'id'} } 
            @{$self->_get_used_scans()}
        ];
}

sub _scan_def_line_mapping
{
    my $self = shift;
    return $self->_map_all_but_last(sub { "$_[0] -nst" }, shift);
}

sub _calc_iter_quota
{
    my $self = shift;
    my $quota = shift;

    if ($self->_offset_quotas())
    {
        return $quota+1;
    }
    else
    {
        return $quota;
    }
}

sub _map_scan_idx_to_id
{
    my $self = shift;
    my $index = shift;

    return $self->_selected_scans()->[$index]->id();
}

sub _format_prelude_iter
{
    my $self = shift;

    my $iter = shift;

    return $iter->iters() . '@'
        . $self->_map_scan_idx_to_id($iter->scan())
        ;
}

sub _get_line_of_prelude
{
    my $self = shift;
    return "--prelude \"" .
        join(",",
            map { $self->_format_prelude_iter($_) } 
                @{$self->_chosen_scans()}
        ) . "\"";
}

sub _calc_script_lines
{
    my $self = shift;
    return
        [
            $self->_get_line_of_command(),
            @{$self->_scan_def_line_mapping(
                $self->_get_lines_of_scan_defs()
            )},
            $self->_get_line_of_prelude()
        ];
}

sub _calc_script_text
{
    my $self = shift;
    return 
        join("",
            @{$self->_line_ends_mapping(
                $self->_calc_script_lines()
            )}
        );
}

sub _write_script
{
    my $self = shift;
     
    $self->_out_script(
        $self->_calc_script_text()
    );
}

sub _calc_scans_data
{
    my $self = shift;

    my $method = 
        (($self->_optimize_for() =~ m{len})
            ? "get_scans_lens_data"
            : "get_scans_data"
        );

    return $self->_input_obj->$method();
}

sub _arbitrator_trace_cb
{
    my $args = shift;
    printf("%s \@ %s (%s solved)\n",
        @$args{qw(iters_quota selected_scan_idx total_boards_solved)}
    );
}

sub _init_arbitrator
{
    my $self = shift;

    return $self->_arbitrator(
        Shlomif::FCS::CalcMetaScan->new(
            {
                'quotas' => $self->_get_quotas(),
                'selected_scans' => $self->_selected_scans(),
                'num_boards' => $self->_num_boards(),
                'scans_data' => $self->_calc_scans_data(),
                'trace_cb' => \&_arbitrator_trace_cb,
                'optimize_for' => $self->_optimize_for(),
            }
        )
    );
}

sub _report_total_iters
{
    my $self = shift;
    if ($self->_arbitrator()->status() eq "solved_all")
    {
        print "Solved all!\n";
    }
    printf("total_iters = %s\n", $self->_arbitrator()->total_iters());
}

sub _arbitrator_process
{
    my $self = shift;

    $self->_arbitrator()->calc_meta_scan();

    my $scans = $self->_post_processor->process(
        $self->_arbitrator->chosen_scans()
    );

    $self->_chosen_scans($scans);
}

sub _do_trace_for_board
{
    my $self = shift;
    my $board = shift;

    my $results = $self->_arbitrator()->calc_board_iters($board);
    print "\@info=". join(",", @{$results->{per_scan_iters}}). "\n";
    print +($board+$self->_start_board()) . ": ". $results->{board_iters} . "\n";
}

sub _real_do_trace
{
    my $self = shift;
    foreach my $board (0 .. $self->_num_boards()-1)
    {
        $self->_do_trace_for_board($board);
    }
}

sub _do_trace
{
    my $self = shift;
    # Analyze the results

    if ($self->_should_trace_be_done())
    {
        $self->_real_do_trace();
    }
}

sub _get_run_string
{
    my $self = shift;
    my $results = shift;

    return join("",
        map 
        { 
            sprintf('%i@%i,', 
                $_->iters(), 
                $self->_map_scan_idx_to_id($_->scan())
            )
        }
        @{$self->_post_processor->process($results->scan_runs())},
    );
}

sub _do_simulation_for_board
{
    my ($self, $board) = @_;

    my $results = $self->_arbitrator()->simulate_board($board);

    my $scan_mapper = sub {
        my $index = shift;

        return $self->_map_scan_idx_to_id($index);
    };

    return 
        sprintf("%i:%s:%s:%i",
            $board+1,
            $results->get_status(),
            $self->_get_run_string($results),
            $results->get_total_iters(),
        );
}

sub _real_do_simulation
{
    my $self = shift;

    open my $simulate_out_fh, ">", $self->_simulate_to()
        or Carp::confess("Could not open " . $self->_simulate_to() . " - $!");

    foreach my $board (0 .. $self->_num_boards()-1)
    {
        print {$simulate_out_fh} $self->_do_simulation_for_board($board), "\n";
    }

    close($simulate_out_fh);

    return;
}


sub _do_simulation
{
    my $self = shift;
    # Analyze the results

    if (defined($self->_simulate_to()))
    {
        $self->_real_do_simulation();
    }

    return;
}

sub run
{
    my $self = shift;

    $self->_init_arbitrator();
    $self->_arbitrator_process();
    $self->_report_total_iters();
    $self->_write_script();
    $self->_do_trace();
    $self->_do_simulation();

    return 0;
}

1;

