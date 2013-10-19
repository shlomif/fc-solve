package main;

use strict;
use warnings;

package Games::Solitaire::Verify::App::CmdLine;

use strict;
use warnings;

use base 'Games::Solitaire::Verify::Base';

use Getopt::Long qw(GetOptionsFromArray);

use Games::Solitaire::Verify::VariantsMap;
use Games::Solitaire::Verify::Solution;

__PACKAGE__->mk_acc_ref(
    [
        qw(
            _filenames
            _variant_params
        )
    ]
);

sub _init
{
    my ($self, $args) = @_;

    my $argv = $args->{'argv'};

    my $variant_map = Games::Solitaire::Verify::VariantsMap->new();

    my $variant_params = $variant_map->get_variant_by_id("freecell");

    GetOptionsFromArray(
        $argv,
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

    $self->_variant_params($variant_params);

    $self->_filenames($argv);

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

sub run
{
    my $self = shift;

    my $variant_params = $self->_variant_params();

    foreach my $filename (@{$self->_filenames()})
    {
        my $contents = _slurp($filename);

        if ($contents =~ m{solveable})
        {
            open my $text_fh, '<', \$contents;
            my $solution = Games::Solitaire::Verify::Solution->new(
                {
                    input_fh => $text_fh,
                    variant => "custom",
                    variant_params => $variant_params,
                },
            );

            my $verdict = $solution->verify();

            print "== $filename\n";
            if (!$verdict)
            {
                print "Solution is OK.\n";
            }
            else
            {
                print STDERR Dumper($verdict);
                print "Solution is Wrong.\n";
                exit(-1);
            }
            close($text_fh);
        }
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

Shlomi Fish, L<http://www.shlomifish.org/>

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

