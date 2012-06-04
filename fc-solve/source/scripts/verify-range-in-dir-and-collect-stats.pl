package Games::Solitaire::Verify::App::CmdLine;

use strict;
use warnings;

use base 'Games::Solitaire::Verify::Base';

use Getopt::Long qw(GetOptionsFromArray);

use Games::Solitaire::Verify::VariantsMap;
use Games::Solitaire::Verify::Solution;

use List::Util qw(min max);

use Math::BigInt lib => 'GMP';
use Storable qw(nstore retrieve);

use Fcntl qw(:flock);

my $IDX_SOL_FORMAT = "%09d";
my $MAX_MOMENT = 7;

$SIG{__WARN__} = sub { Carp::confess $_[0]; };

__PACKAGE__->mk_acc_ref(
    [
        qw(
            _max_idx
            _min_idx
            _solutions_dir
            _summary_file
            _stats_file
            _summary_lock
            _variant_params
            _should_verify
        )
    ]
);

sub _init
{
    my ($self, $args) = @_;

    my $argv = $args->{'argv'};

    my $variant_map = Games::Solitaire::Verify::VariantsMap->new();

    my $variant_params = $variant_map->get_variant_by_id("freecell");

    my ($min_idx, $max_idx, $summary_lock, $summary_file, $stats_file);

    my $should_verify = 1;

    GetOptionsFromArray(
        $argv,
        'min-idx=i' => \$min_idx,
        'max-idx=i' => \$max_idx,
        'summary-lock=s' => \$summary_lock,
        'summary-file=s' => \$summary_file,
        'summary-stats-file=s' => \$stats_file,
        'verify!' => \$should_verify,
        'g|game|variant=s' => sub {
            my (undef, $game) = @_;

            $variant_params = $variant_map->get_variant_by_id($game);

            if (!defined($variant_params))
            {
                die "Unknown variant '$game'!\n";
            }
        },
        'freecells-num=i' => sub {
            my (undef, $n) = @_;
            $variant_params->num_freecells($n);
        },
        'stacks-num=i' => sub {
            my (undef, $n) = @_;
            $variant_params->num_columns($n);
        },
        'decks-num=i' => sub {
            my (undef, $n) = @_;

            if (! ( ($n == 1) || ($n == 2) ) )
            {
                die "Decks should be 1 or 2.";
            }

            $variant_params->num_decks($n);
        },
        'sequences-are-built-by=s' => sub {
            my (undef, $val) = @_;

            my %seqs_build_by =
            (
                (map { $_ => $_ }
                    (qw(alt_color suit rank))
                ),
                "alternate_color" => "alt_color",
            );

            my $proc_val = $seqs_build_by{$val};

            if (! defined($proc_val))
            {
                die "Unknown sequences-are-built-by '$val'!";
            }

            $variant_params->seqs_build_by($proc_val);
        },
        'empty-stacks-filled-by=s' => sub {
            my (undef, $val) = @_;

            my %empty_stacks_filled_by_map =
            (map { $_ => 1 } (qw(kings any none)));

            if (! exists($empty_stacks_filled_by_map{$val}))
            {
                die "Unknown empty stacks filled by '$val'!";
            }

            $variant_params->empty_stacks_filled_by($val);
        },
        'sequence-move=s' => sub {
            my (undef, $val) = @_;

            my %seq_moves = (map { $_ => 1 } (qw(limited unlimited)));

            if (! exists ($seq_moves{$val}) )
            {
                die "Unknown sequence move '$val'!";
            }

            $variant_params->sequence_move($val);
        },
    )
        or die "Cannot process command line arguments";

    if (!@$argv)
    {
        push @$argv, "-";
    }

    if (! $self->_min_idx($min_idx) )
    {
        Carp::confess ("min-idx was not supplied.");
    }
    if (! $self->_max_idx($max_idx) )
    {
        Carp::confess ("max-idx was not supplied.");
    }
    if (! $self->_summary_lock($summary_lock))
    {
        Carp::confess ("summary-lock was not supplied.");
    }
    if (! $self->_summary_file($summary_file))
    {
        Carp::confess ("summary-file was not supplied.");
    }
    if (! $self->_stats_file($stats_file))
    {
        Carp::confess ("summary-stats-file was not supplied.");
    }
    $self->_variant_params($variant_params);
    $self->_should_verify($should_verify);

    $self->_solutions_dir(shift(@$argv));

    return;
}

sub _slurp
{
    my $filename = shift;

    open my $in, "<", $filename
        or die "Cannot open '$filename' for slurping - $!";

    local $/;
    my $contents = <$in>;

    close($in);

    return $contents;
}

use Data::Dumper;

sub _verify_contents
{
    my ($self, $filename, $contents) = @_;

    my $variant_params = $self->_variant_params();
    open my $text_fh, '<', \$contents;

    my $solution = Games::Solitaire::Verify::Solution->new(
        {
            input_fh => $text_fh,
            variant => "custom",
            variant_params => $variant_params,
        },
    );

    my $verdict = $solution->verify();

    # print "== $filename\n";
    if ($verdict)
    {
        print STDERR Dumper($verdict);
        print "Solution for $filename is Wrong.\n";
        exit(-1);
    }
    close($text_fh);

    return;
}

sub run
{
    my $self = shift;


    my $solutions_dir = $self->_solutions_dir;

    my $min_idx = $self->_min_idx;
    my $max_idx = $self->_max_idx;

    my $get_data_struct = sub {
        return +{
            moments => [map { Math::BigInt->new(0) } (0 .. $MAX_MOMENT)] };
    };

    my $get_for_solved = sub {
        return +{ map { $_ => $get_data_struct->() } qw(iters gen_states) };
    };

    my %stats =
    (
        solved => { map { $_ => +{} } qw(gen_states iters sol_lens) },
        unsolved => { map { $_ => +{} } qw(gen_states iters) },
    );

    foreach my $idx ($min_idx .. $max_idx)
    {
        my $filename = sprintf("%s/$IDX_SOL_FORMAT.sol", $solutions_dir, $idx);
        my $contents = _slurp($filename);

        my $solved = $contents =~ m{solveable};

        my %current_data;

        if ($solved)
        {
            $current_data{'sol_lens'} = () = ($contents =~ m{^====}gms);
            if ($self->_should_verify)
            {
                $self->_verify_contents($filename, $contents);
            }
        }

        if (my ($iters) = ($contents =~ m{^Total number of states checked is (\d+)\.$}ms)
                and
            my ($gen_states) = ($contents =~ m{^This scan generated (\d+) states\.$}ms))
        {
            $current_data{'iters'} = $iters;
            $current_data{'gen_states'} = $gen_states;
        }
        else
        {
            Carp::confess
                "Cannot match format for iters/gen_states at $filename";
        }

        my $record = $stats{ $solved ? 'solved' : 'unsolved' };

        # Collect the statistics
        foreach my $key (keys(%current_data))
        {
            $record->{$key}->{$current_data{$key}}++;
        }
    }

    {
        open my $lock_fh, ">", $self->_summary_lock()
            or Carp::confess ("Cannot lock summary-lock - $!");

        flock ($lock_fh, LOCK_EX)
            or Carp::confess("Cannot lock summary lock - $!");

        my $log_string
            = sprintf("Solved Range: Start=%d ; End=%d\n", $min_idx, $max_idx)
            ;

        if (! -e $self->_stats_file)
        {
            nstore(
                {
                    counts =>
                    { solved =>
                        { iters => {}, gen_states => {},
                            sol_lens => {},
                        },
                        unsolved => {
                            iters => {},
                            gen_states => {}
                        }
                    }
                },
                $self->_stats_file
            );
        }

        my $total_data = retrieve($self->_stats_file);

        foreach my $status (qw(solved unsolved))
        {
            my $solved_rec = $stats{ $status };
            my $out_solved_rec = $total_data->{'counts'}->{$status};

            foreach my $type (keys(%$solved_rec))
            {
                my $local_counts_hash = $solved_rec->{$type};
                my $out_counts_hash = $out_solved_rec->{$type};

                foreach my $datum (keys ( %{$local_counts_hash} ) )
                {
                    ($out_counts_hash->{$datum} ||= 0)
                        += $local_counts_hash->{$datum};
                }
            }
        }

        nstore($total_data, $self->_stats_file);

        open my $summary_out_fh, ">>", $self->_summary_file()
            or Carp::confess("Cannot open summary out file for appending - $!");

        print {$summary_out_fh} $log_string;

        close ($summary_out_fh);

        flock ($lock_fh, LOCK_UN)
            or Carp::confess("Cannot unlock summary lock - $!");

        close ($lock_fh);
    }
    exit(0);
}

1;

package main;

Games::Solitaire::Verify::App::CmdLine->new({ argv => [@ARGV] })->run();

1;

=head1 NAME

Games::Solitaire::Verify::App::CmdLine - a module implementing a standalone
command line app for verifying the solutions of Solitaire games.

=head1 SYNOPSIS

    $ perl -MGames::Solitaire::Verify::App::CmdLine -e 'Games::Solitaire::Verify::App::CmdLine->new({argv => \@ARGV})->run()' -- [ARGS]

=head1 DESCRIPTION

This is a module implementing a standalone command line app for verifying the
solutions of Solitaire games.

=head1 EXPORTS

=head2 run()

Actually executed the command-line application.

=head1 AUTHOR

Shlomi Fish, C<< <shlomif@iglu.org.il> >>

=head1 BUGS

Please report any bugs or feature requests to
C<bug-test-run-cmdline@rt.cpan.org>, or through the web interface at
L<http://rt.cpan.org/NoAuth/ReportBug.html?Queue=Test-Run-CmdLine>.
I will be notified, and then you'll automatically be notified of progress on
your bug as I make changes.

=head1 ACKNOWLEDGEMENTS

=head1 COPYRIGHT & LICENSE

Copyright 2007 Shlomi Fish, all rights reserved.

This program is released under the MIT X11 License.

=cut

