package Games::Solitaire::Verify::Golf;

use strict;
use warnings;
use autodie;

=head1 NAME

Games::Solitaire::Verify::Golf - verify an entire solution
of black-hole-solve (or a similar solver)

=head1 SYNOPSIS

    my ( $board_fn, $solution_fn ) = @ARGV;
    my $verifier = Games::Solitaire::Verify::Golf->new(
        {
            board_string => path($board_fn)->slurp_raw(),
            variant => "all_in_a_row",
        }
    );

    open my $fh, '<', $solution_fn;
    $verifier->process_solution( sub { my $l = <$fh>; chomp $l; return $l; } );
    print "Solution is OK.\n";
    exit(0);

=head1 METHODS

=head2 Games::Solitaire::Verify::Golf->new({board_string=>$str, variant =>"golf"|"all_in_a_row"})

Construct a new validator / verifier for the variant and the initial board string.

For golf one can specify:

=over 4

=item * wrap_ranks

=item * queens_on_kings

=back

=head2 $obj->process_solution($line_iter_cb)

Process the solution with the line iterator. Throws an exception if there is an error in it.

=cut

use Carp ();
use List::Util qw/ all /;

use Games::Solitaire::Verify::Card      ();
use Games::Solitaire::Verify::Column    ();
use Games::Solitaire::Verify::Freecells ();

use parent 'Games::Solitaire::Verify::Base';

__PACKAGE__->mk_acc_ref(
    [
        qw(
            _columns
            _foundation
            _place_queens_on_kings
            _talon
            _variant
            _wrap_ranks
            )
    ]
);

my $MAX_RANK  = 13;
my $NUM_SUITS = 4;

sub _is_golf
{
    my $self = shift;

    return $self->_variant eq 'golf';
}

sub _init
{
    my ( $self, $args ) = @_;

    my $variant = $self->_variant( $args->{variant} );
    if ( not exists { golf => 1, all_in_a_row => 1 }->{$variant} )
    {
        Carp::confess("Unknown variant '$variant'!");
    }
    $self->_place_queens_on_kings( $args->{queens_on_kings} // '' );
    $self->_wrap_ranks( $args->{wrap_ranks}                 // '' );
    $self->_foundation(
        Games::Solitaire::Verify::Freecells->new( { count => 1 } ) );
    my $board_string = $args->{board_string};

    my @lines = split( /\n/, $board_string );

    my $foundation_str = shift(@lines);
    if ( $self->_variant eq 'golf' )
    {
        if ( $foundation_str !~ s#\ATalon: ((?:\S{2} ){15}\S{2})#$1# )
        {
            die "improper talon line <$foundation_str>!";
        }
        $self->_talon(
            [
                map { Games::Solitaire::Verify::Card->new( { string => $_ } ) }
                    split / /,
                $foundation_str
            ]
        );

        $foundation_str = shift(@lines);
        if ( my ($card_s) = $foundation_str =~ m#\AFoundations: (\S{2})\z# )
        {
            $self->_set_found(
                Games::Solitaire::Verify::Card->new( { string => $card_s } ) );
        }
        else
        {
            Carp::confess("Foundations str is '$foundation_str'");
        }

    }
    else
    {
        $self->_talon( [] );
        if ( $foundation_str ne "Foundations: -" )
        {
            Carp::confess("Foundations str is '$foundation_str'");
        }

    }

    $self->_columns(
        [
            map {
                Games::Solitaire::Verify::Column->new(
                    {
                        string => ": $_",
                    }
                )
            } @lines
        ]
    );
    if ( $self->_wrap_ranks )
    {
        $self->_place_queens_on_kings(1);
    }

    return;
}

sub _set_found
{
    my ( $self, $card ) = @_;
    $self->_foundation->assign( 0, $card );
    return;
}

sub process_solution
{
    my ( $self, $next_line_iter ) = @_;

    my $line_num = 0;

    my $get_line = sub {
        my $ret = $next_line_iter->();
        return ( $ret, ++$line_num );
    };

    my $assert_empty_line = sub {
        my ( $s, $line_idx ) = $get_line->();

        if ( $s ne '' )
        {
            die "Line '$line_idx' is not empty, but '$s'";
        }

        return;
    };

    my ( $l, $first_l ) = $get_line->();

    if ( $l ne "Solved!" )
    {
        die "First line is '$l' instead of 'Solved!'";
    }

    # As many moves as the number of cards.
MOVES:
    for my $move_idx ( 0 .. ( $MAX_RANK * $NUM_SUITS - 1 ) )
    {
        my ( $move_line, $move_line_idx ) = $get_line->();

        my $card;
        if (    $self->_is_golf
            and $move_line =~ m/\ADeal talon\z/ )
        {
            if ( !@{ $self->_talon } )
            {
                die "Talon is empty on line no. $move_line_idx";
            }
            $card = shift @{ $self->_talon };
        }
        elsif ( $move_line !~
            m/\AMove a card from stack ([0-9]+) to the foundations\z/ )
        {
            die
"Incorrect format for move line no. $move_line_idx - '$move_line'";
        }

        my $col_idx = $1;

        if ( !defined $card )
        {
            if ( ( $col_idx < 0 ) or ( $col_idx >= $MAX_RANK ) )
            {
                die "Invalid column index '$col_idx' at $move_line_idx";
            }
        }

        $assert_empty_line->();

        my ( $info_line, $info_line_idx ) = $get_line->();

        if ( $info_line !~ m/\AInfo: Card moved is ([A23456789TJQK][HCDS])\z/ )
        {
            die
"Invalid format for info line no. $info_line_idx - '$info_line'";
        }

        my $moved_card_str = $1;

        $assert_empty_line->();
        $assert_empty_line->();

        my ( $sep_line, $sep_line_idx ) = $get_line->();

        if ( $sep_line !~ m/\A=+\z/ )
        {
            die
"Invalid format for separator line no. $sep_line_idx - '$sep_line'";
        }

        $assert_empty_line->();

        if ( defined $card )
        {
            my $top_card_moved_str = $card->to_string();
            if ( $top_card_moved_str ne $moved_card_str )
            {
                die
"Card moved should be '$top_card_moved_str', but the info says it is '$moved_card_str' at line $info_line_idx";
            }
        }
        else
        {
            my $col                = $self->_columns->[$col_idx];
            my $top_card           = $col->top();
            my $top_card_moved_str = $top_card->to_string();

            if ( $top_card_moved_str ne $moved_card_str )
            {
                die
"Card moved should be '$top_card_moved_str', but the info says it is '$moved_card_str' at line $info_line_idx";
            }

            my $found_card = $self->_foundation->cell(0);
            if ( defined($found_card) )
            {
                my $found_rank = $found_card->rank();
                my $src_rank   = $top_card->rank();

                my $delta = abs( $src_rank - $found_rank );
                if (
                    not( $delta == 1 or $delta == ( $MAX_RANK - 1 ) )
                    or (
                            $self->_is_golf
                        and ( !$self->_wrap_ranks )
                        and (
                            (
                                $self->_place_queens_on_kings
                                ? ( $found_rank == $MAX_RANK )
                                : 0
                            )
                            or $delta != 1
                        )
                    )
                    )
                {
                    die "Cannot put "
                        . $top_card->to_string()
                        . " in the foundations that contain "
                        . $found_card->to_string();
                }
            }
            $card = $col->pop;
        }

        $self->_set_found($card);
        if ( $self->_is_golf )
        {
            if ( all { $_->len == 0 } @{ $self->_columns } )
            {
                last MOVES;
            }
        }
    }
    return;
}

1;
