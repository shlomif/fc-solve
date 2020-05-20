package Games::Solitaire::Verify::FromOtherSolversBase;

use strict;
use warnings;
use autodie;

<<<<<<< HEAD
use List::Util qw(first);
=======
use List::MoreUtils qw(firstidx);
>>>>>>> a6759b51c5ba03266d1420faff00e086fd18f3e5
use Path::Tiny qw/ path /;

use parent 'Games::Solitaire::Verify::Base';

use Games::Solitaire::Verify::VariantsMap      ();
use Games::Solitaire::Verify::Solution         ();
use Games::Solitaire::Verify::State::LaxParser ();
use Games::Solitaire::Verify::Move             ();

use Getopt::Long qw(GetOptionsFromArray);

sub _init
{
    my ( $self, $args ) = @_;

    my $argv = $args->{'argv'};

    my $variant_map = Games::Solitaire::Verify::VariantsMap->new();

    my $variant_params = $variant_map->get_variant_by_id("freecell");

    GetOptionsFromArray(
        $argv,
        'g|game|variant=s' => sub {
            my ( undef, $game ) = @_;

            $variant_params = $variant_map->get_variant_by_id($game);

            if ( !defined($variant_params) )
            {
                die "Unknown variant '$game'!\n";
            }
        },
        'freecells-num=i' => sub {
            my ( undef, $n ) = @_;
            $variant_params->num_freecells($n);
        },
        'stacks-num=i' => sub {
            my ( undef, $n ) = @_;
            $variant_params->num_columns($n);
        },
        'decks-num=i' => sub {
            my ( undef, $n ) = @_;

            if ( !( ( $n == 1 ) || ( $n == 2 ) ) )
            {
                die "Decks should be 1 or 2.";
            }

            $variant_params->num_decks($n);
        },
        'sequences-are-built-by=s' => sub {
            my ( undef, $val ) = @_;

            my %seqs_build_by = (
                ( map { $_ => $_ } (qw(alt_color suit rank)) ),
                "alternate_color" => "alt_color",
            );

            my $proc_val = $seqs_build_by{$val};

            if ( !defined($proc_val) )
            {
                die "Unknown sequences-are-built-by '$val'!";
            }

            $variant_params->seqs_build_by($proc_val);
        },
        'empty-stacks-filled-by=s' => sub {
            my ( undef, $val ) = @_;

            my %empty_stacks_filled_by_map =
                ( map { $_ => 1 } (qw(kings any none)) );

            if ( !exists( $empty_stacks_filled_by_map{$val} ) )
            {
                die "Unknown empty stacks filled by '$val'!";
            }

            $variant_params->empty_stacks_filled_by($val);
        },
        'sequence-move=s' => sub {
            my ( undef, $val ) = @_;

            my %seq_moves = ( map { $_ => 1 } (qw(limited unlimited)) );

            if ( !exists( $seq_moves{$val} ) )
            {
                die "Unknown sequence move '$val'!";
            }

            $variant_params->sequence_move($val);
        },
    ) or die "Cannot process command line arguments";

    my $filename = shift(@$argv);

    if ( !defined($filename) )
    {
        $filename = "-";
    }

    my $sol_filename = shift(@$argv);

    if ( !defined($sol_filename) )
    {
        die "Solution filename not specified.";
    }

    $self->_variant_params($variant_params);
    $self->_filename($filename);
    $self->_sol_filename($sol_filename);

    my $s = '';
    $self->_buffer_ref( \$s );

    return;
}

sub _append
{
    my ( $self, $text ) = @_;

    ${ $self->_buffer_ref } .= $text;

    return;
}

sub _get_buffer
{
    my ($self) = @_;

    return ${ $self->_buffer_ref };
}

sub _read_initial_state
{
    my $self = shift;

    $self->_st(
        Games::Solitaire::Verify::State::LaxParser->new(
            {
                string           => path( $self->_filename )->slurp_raw,
                variant          => 'custom',
                'variant_params' => $self->_variant_params(),
            }
        )
    );

    $self->_append("-=-=-=-=-=-=-=-=-=-=-=-\n\n");

    $self->_out_running_state;

    return;
}

sub _out_running_state
{
    my ($self) = @_;

    $self->_append( $self->_st->to_string() . "\n\n====================\n\n" );

    return;
}

sub _perform_and_output_move
{
    my ( $self, $move_s ) = @_;

    $self->_append("$move_s\n\n");

    $self->_st->verify_and_perform_move(
        Games::Solitaire::Verify::Move->new(
            {
                fcs_string => $move_s,
                game       => $self->_st->_variant(),
            },
        )
    );
    $self->_out_running_state;

    return;
}

sub _find_col_card
{
    my ( $self, $card_s ) = @_;

<<<<<<< HEAD
    return first
=======
    return firstidx
>>>>>>> a6759b51c5ba03266d1420faff00e086fd18f3e5
    {
        my $col = $self->_st->get_column($_);
        ( $col->len == 0 ) ? 0 : $col->top->fast_s eq $card_s
    }
    ( 0 .. $self->_st->num_columns - 1 );
}

sub _find_empty_col
{
    my ($self) = @_;

<<<<<<< HEAD
    return first
=======
    return firstidx
>>>>>>> a6759b51c5ba03266d1420faff00e086fd18f3e5
    {
        $self->_st->get_column($_)->len == 0
    }
    ( 0 .. $self->_st->num_columns - 1 );
}

sub _find_fc_card
{
    my ( $self, $card_s ) = @_;
<<<<<<< HEAD
    return first
=======
    my $dest_fc_idx = firstidx
>>>>>>> a6759b51c5ba03266d1420faff00e086fd18f3e5
    {
        my $card = $self->_st->get_freecell($_);
        defined($card) ? ( $card->fast_s eq $card_s ) : 0;
    }
    ( 0 .. $self->_st->num_freecells - 1 );
}

sub _find_card_src_string
{
    my ( $self, $src_card_s ) = @_;

    my $src_col_idx = $self->_find_col_card($src_card_s);

<<<<<<< HEAD
    if ( not defined $src_col_idx )
    {
        my $src_fc_idx = $self->_find_fc_card($src_card_s);
        if ( not defined($src_fc_idx) )
=======
    if ( $src_col_idx < 0 )
    {
        my $src_fc_idx = $self->_find_fc_card($src_card_s);
        if ( $src_fc_idx < 0 )
>>>>>>> a6759b51c5ba03266d1420faff00e086fd18f3e5
        {
            die "Cannot find card <$src_card_s>.";
        }
        return ( "a card", "freecell $src_fc_idx" );
    }
    else
    {
        return ( "1 cards", "stack $src_col_idx" );
    }
}

sub run
{
    my ($self) = @_;

    $self->_process_main;

    print $self->_get_buffer;

    return;
}

1;

__END__

=head1 NAME

Games::Solitaire::Verify::FromOtherSolversBase - a modulino base
class for converters.

=head1 SYNOPSIS

    use parent 'Games::Solitaire::Verify::FromOtherSolversBase';

=head1 DESCRIPTION

A base class.

=head1 METHODS

=head2 run()

Actually execute the command-line application.

=cut
