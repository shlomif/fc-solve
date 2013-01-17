package AI::Pathfinding::OptimizeMultiple::CmdLine;

use strict;
use warnings;

use MooX qw/late/;

use Getopt::Long;
use IO::File;

use AI::Pathfinding::OptimizeMultiple;
use AI::Pathfinding::OptimizeMultiple::PostProcessor;

# TODO : restore later.
# use MyInput;

use Carp;

has _arbitrator => (is => 'rw');
has _add_horne_prune => (isa => 'Bool', is => 'rw');
has _chosen_scans => (isa => 'ArrayRef', is => 'rw');
has input_obj_class => (isa => 'Str', is => 'rw');
has _input_obj => (is => 'rw');
has _is_flares => (is => 'rw', isa => 'Bool', default => sub { 0; },);
has _num_boards => (isa => 'Int', is => 'rw');
has _offset_quotas => (isa => 'Int', is => 'rw');
has _optimize_for => (isa => 'Str', is => 'rw');
has _output_filename => (isa => 'Str', is => 'rw');
has _post_processor => (isa => 'Maybe[AI::Pathfinding::OptimizeMultiple::PostProcessor]', is => 'rw');
has _quotas_are_cb => (isa => 'Bool', is => 'rw');
has _quotas_expr => (isa => 'Maybe[Str]', is => 'rw');
has _should_rle_be_done => (isa => 'Bool', is => 'rw');
has _should_trace_be_done => (isa => 'Bool', is => 'rw');
has _simulate_to => (isa => 'Maybe[Str]', is => 'rw');
has _start_board => (isa => 'Int', is => 'rw');

my $_component_re = qr/[A-Za-z][A-Za-z0-9_]*/;
my $_module_re = qr/$_component_re(?:::$_component_re)*/;

sub BUILD
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
    my $_add_horne_prune = 0;
    my $input_obj_class = 'AI::Pathfinding::OptimizeMultiple::DataInputObj';

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
        "sprtf" => \$_add_horne_prune,
        "input-class=s" => \$input_obj_class,
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
    $self->_add_horne_prune($_add_horne_prune);
    $self->input_obj_class($input_obj_class);

    {
        my $class = $self->input_obj_class();
        if ($class !~ m{\A$_module_re\z})
        {
            Carp::confess(
                "Input object class does not seem like a good class:"
                . $self->input_obj_class()
            );
        }
        eval "require $class;";
        if ($@)
        {
            die "Could not load '$class' - <<$@>>";
        }

        # TODO : Restore later.
        $self->_input_obj(
            $class->new(
                {
                    start_board => $self->_start_board(),
                    num_boards => $self->_num_boards(),
                }
            )
        );
    }

    $self->_post_processor(
        AI::Pathfinding::OptimizeMultiple::PostProcessor->new(
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

sub _get_scan_line
{
    my ($self, $line) = @_;

    return $line->{'cmd_line'} . " -step 500 "
        . join(" ", map { $_, $line->{'id'} }
            ("--st-name", ($self->_is_flares() ? "--flare-name" : ()))
        );
}

sub _get_lines_of_scan_defs
{
    my $self = shift;
    return
        [map
            { $self->_get_scan_line($_) }
            @{$self->_get_used_scans()}
        ];
}

sub _scan_def_line_mapping
{
    my ($self, $lines_aref) = @_;

    return $self->_map_all_but_last(
        sub
        {
            my ($line) = @_;

            return $line . ' ' . ($self->_is_flares() ? "-nf" : "-nst");
        },
        [
            map
            {
                my $line = $_;
                # Add the -sp r:tf flag to each scan if specified - it enhances
                # performance, but timing the scans with it makes the total
                # scan sub-optimal.
                if ($self->_add_horne_prune())
                {
                    $line =~ s/( --st-name)/ -sp r:tf$1/;
                }
                $line;
            }
            @$lines_aref
        ],
    );
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

    return ($self->_is_flares() ? "Run:" : "") . $iter->iters() . '@'
        . $self->_map_scan_idx_to_id($iter->scan_idx())
        ;
}

sub _get_line_of_prelude
{
    my $self = shift;
    return +($self->_is_flares() ? "--flares-plan" : "--prelude") . qq{ "} .
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

sub _calc_scans_iters_pdls
{
    my $self = shift;

    my $method =
        (($self->_optimize_for() =~ m{len})
            ? "get_scans_lens_iters_pdls"
            : "get_scans_iters_pdls"
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
        AI::Pathfinding::OptimizeMultiple->new(
            {
                'scans' =>
                [
                    map { +{ name => $_->id() } }
                    @{$self->_input_obj->_suitable_scans_list()},
                ],
                'quotas' => $self->_get_quotas(),
                'selected_scans' => $self->_selected_scans(),
                'num_boards' => $self->_num_boards(),
                'scans_iters_pdls' => $self->_calc_scans_iters_pdls(),
                'trace_cb' => \&_arbitrator_trace_cb,
                'optimize_for' => $self->_optimize_for(),
            }
        )
    );
}

sub _report_total_iters
{
    my $self = shift;
    if ($self->_arbitrator()->get_final_status() eq "solved_all")
    {
        print "Solved all!\n";
    }
    printf("total_iters = %s\n", $self->_arbitrator()->get_total_iters());
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
                $self->_map_scan_idx_to_id($_->scan_idx())
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


sub run_flares
{
    my $self = shift;

    $self->_optimize_for("len");
    $self->_is_flares(1);

    $self->_init_arbitrator();

    $self->_arbitrator()->calc_flares_meta_scan();

    my $scans = $self->_post_processor->process(
        $self->_arbitrator->chosen_scans()
    );

    $self->_chosen_scans($scans);
    $self->_report_total_iters();
    $self->_write_script();
    $self->_do_trace();
    $self->_do_simulation();

    return 0;
}


1;

__END__

=pod

=head1 NAME

AI::Pathfinding::OptimizeMultiple::CmdLine - the command line application class.

=head1 VERSION

Version 0.0.1

=for foo $self->_input_obj(
        MyInput->new(
            {
                start_board => $self->_start_board(),
                num_boards => $self->_num_boards(),
            }
        )
    );

=head2 $self->run()

For internal use.

=head2 $self->run_flares()

For internal use.

=head2 $self->input_obj_class()

The class to handle the input data - by default -
L<AI::Pathfinding::OptimizeMultiple::DataInputObj>.

=head2 BUILD()

Moo leftover. B<INTERNAL USE>.

=head1 COPYRIGHT AND LICENSE

Copyright (c) 2010 Shlomi Fish

Permission is hereby granted, free of charge, to any person
obtaining a copy of this software and associated documentation
files (the "Software"), to deal in the Software without
restriction, including without limitation the rights to use,
copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the
Software is furnished to do so, subject to the following
conditions:

The above copyright notice and this permission notice shall be
included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
OTHER DEALINGS IN THE SOFTWARE.

=cut
