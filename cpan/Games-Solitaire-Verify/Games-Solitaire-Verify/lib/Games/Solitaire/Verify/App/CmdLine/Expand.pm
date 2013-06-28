package Games::Solitaire::Verify::App::CmdLine::Expand;

use strict;
use warnings;

use parent 'Games::Solitaire::Verify::Base';

our $VERSION = '0.1200';

use Data::Dumper qw(Dumper);

use Getopt::Long qw(GetOptionsFromArray);

use Games::Solitaire::Verify::VariantsMap;
use Games::Solitaire::Verify::Solution::ExpandMultiCardMoves;

__PACKAGE__->mk_acc_ref(
    [
        qw(
            _filename
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

    my $filename = shift(@$argv);

    if (!defined($filename))
    {
        $filename = "-";
    }

    $self->_variant_params($variant_params);
    $self->_filename($filename);

    return;
}

sub run
{
    my $self = shift;

    my $filename = $self->_filename();
    my $variant_params = $self->_variant_params();

    my $fh;

    if ($filename eq "-")
    {
        $fh = *STDIN;
    }
    else
    {
        open $fh, "<", $filename
            or die "Cannot open '$filename' - $!";
    }

    my $solution = Games::Solitaire::Verify::Solution::ExpandMultiCardMoves->new(
        {
            input_fh => $fh,
            variant => "custom",
            variant_params => $variant_params,
            output_fh => \*STDOUT,
        },
    );

    my $verdict = $solution->verify();
    if (!$verdict)
    {
        exit(0);
    }
    else
    {
        print STDERR Dumper($verdict);
        print STDERR "Solution is Wrong.\n";
        exit(-1);
    }
}

1;

=head1 NAME

Games::Solitaire::Verify::App::CmdLine::Expand - a modulino for expanding
multi-card moves.

=head1 SYNOPSIS

    $ perl -MGames::Solitaire::Verify::App::CmdLine::Expand -e 'Games::Solitaire::Verify::App::CmdLine::Expand->new({argv => \@ARGV})->run()' -- [ARGS]

=head1 DESCRIPTION

This is a module implementing a standalone command line app for expanding
multi-card moves.

=head1 EXPORTS

=head2 run()

Actually execute the command-line application.

=head1 AUTHOR

Shlomi Fish, L<http://www.shlomifish.org/>

=head1 ACKNOWLEDGEMENTS

=head1 COPYRIGHT & LICENSE

Copyright 2007 Shlomi Fish, all rights reserved.

This program is released under the MIT X11 License.

=cut

