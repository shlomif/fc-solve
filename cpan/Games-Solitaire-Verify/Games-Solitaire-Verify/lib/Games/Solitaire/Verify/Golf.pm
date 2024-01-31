package Games::Solitaire::Verify::Golf;

use strict;
use warnings;
use autodie;
use utf8;

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

    open my $fh, '<:encoding(utf-8)', $solution_fn;
    $verifier->process_solution( sub { my $l = <$fh>; chomp $l; return $l; } );
    print "Solution is OK.\n";
    exit(0);

=head1 METHODS

=head2 Games::Solitaire::Verify::Golf->new({board_string=>$str, variant =>"golf"|"all_in_a_row"|"black_hole"})

Construct a new validator / verifier for the variant and the initial board string.

For golf one can specify:

=over 4

=item * wrap_ranks

=item * queens_on_kings

=back

=head2 $obj->process_solution($line_iter_cb)

Process the solution with the line iterator. Throws an exception if there is an error in it.

=cut

use Carp       ();
use List::Util qw/ sum /;

use Games::Solitaire::Verify::Card      ();
use Games::Solitaire::Verify::Column    ();
use Games::Solitaire::Verify::Freecells ();

use parent 'Games::Solitaire::Verify::Base';

__PACKAGE__->mk_acc_ref(
    [
        qw(
            _columns
            _foundation
            _num_foundations
            _place_queens_on_kings
            _talon
            _variant
            _wrap_ranks
        )
    ]
);

my $MAX_RANK  = 13;
my $NUM_SUITS = 4;
my $CARD_RE   = qr/[A23456789TJQK][HCDS]/;

sub _is_binary_star
{
    my $self = shift;

    return $self->_variant eq 'binary_star';
}

sub _is_golf
{
    my $self = shift;

    return $self->_variant eq 'golf';
}

sub _init
{
    my ( $self, $args ) = @_;

    my $variant = $self->_variant( $args->{variant} );
    if (
        not exists {
            all_in_a_row => 1,
            binary_star  => 1,
            black_hole   => 1,
            golf         => 1,
        }->{$variant}
        )
    {
        Carp::confess("Unknown variant '$variant'!");
    }
    my $IS_BINARY_STAR = $self->_is_binary_star;
    $self->_place_queens_on_kings( $args->{queens_on_kings} // '' );
    $self->_wrap_ranks( $args->{wrap_ranks}                 // '' );
    my $num_foundations = ( $IS_BINARY_STAR ? 2 : 1 );
    $self->_num_foundations($num_foundations);
    $self->_foundation(
        Games::Solitaire::Verify::Freecells->new(
            { count => $num_foundations, }
        )
    );
    my $board_string = $args->{board_string};

    my @lines = split( /\n/, $board_string );

    my $_set_found_line = sub {
        my $foundation_str = shift;
        if ( my ($card_s) = $foundation_str =~
            m#\AFoundations:((?: $CARD_RE){$num_foundations})\z# )
        {
            $card_s =~ s/\A //ms or die;
            my @c = split( / /, $card_s );
            if ( @c != $num_foundations )
            {
                die;
            }
            for my $i ( keys @c )
            {
                my $s = $c[$i];
                $self->_set_found( $i,
                    Games::Solitaire::Verify::Card->new( { string => $s } ) );
            }
        }
        else
        {
            Carp::confess("Foundations str is '$foundation_str'");
        }
        return;
    };
    my $foundation_str = shift(@lines);
    if ( $self->_variant eq 'golf' )
    {
        if ( $foundation_str !~ s#\ATalon: ((?:$CARD_RE ){15}$CARD_RE)#$1# )
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
        $_set_found_line->($foundation_str);

    }
    else
    {
        $self->_talon( [] );
        if ( $self->_variant eq "all_in_a_row" )
        {
            if ( $foundation_str ne "Foundations: -" )
            {
                Carp::confess("Foundations str is '$foundation_str'");
            }
        }
        else
        {
            $_set_found_line->($foundation_str);
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
    my ( $self, $i, $card ) = @_;
    $self->_foundation->assign( $i, $card, );
    return;
}

sub process_solution
{
    my ( $self, $next_line_iter ) = @_;
    my $columns         = $self->_columns;
    my $NUM_COLUMNS     = @$columns;
    my $line_num        = 0;
    my $remaining_cards = sum( map { $_->len } @$columns );

    my $get_line = sub {
        my $ret = $next_line_iter->();
        return ( $ret, ++$line_num );
    };

    my $assert_empty_line = sub {
        my ( $s, $line_idx ) = $get_line->();

        if ( $s ne '' )
        {
            Carp::confess("Line '$line_idx' is not empty, but '$s'");
        }

        return;
    };

    my ( $l, $first_l ) = $get_line->();

    if ( $l ne "Solved!" )
    {
        die "First line is '$l' instead of 'Solved!'";
    }
    my $IS_BINARY_STAR     = $self->_is_binary_star;
    my $IS_GOLF            = $self->_is_golf;
    my $CHECK_EMPTY        = ( $IS_GOLF or $self->_variant eq "black_hole" );
    my $IS_DETAILED_MOVE   = $IS_BINARY_STAR;
    my $IS_DISPLAYED_BOARD = $IS_BINARY_STAR;
    my $num_decks          = $self->_num_foundations();

    # As many moves as the number of cards.
MOVES:
    for my $move_idx ( 0 .. ( $num_decks * $MAX_RANK * $NUM_SUITS - 1 ) )
    {
        my ( $move_line, $move_line_idx ) = $get_line->();

        my $card;
        my $col_idx;
        my $foundation_idx;
        my $moved_card_str;
        if (    $IS_GOLF
            and $move_line =~ m/\ADeal talon\z/ )
        {
            if ( !@{ $self->_talon } )
            {
                die "Talon is empty on line no. $move_line_idx";
            }
            $card           = shift @{ $self->_talon };
            $foundation_idx = 0;
        }
        else
        {
            if (
                $IS_DETAILED_MOVE
                ? ( ( $moved_card_str, $col_idx, $foundation_idx ) =
                        $move_line =~
m/\AMove ($CARD_RE) from stack ([0-9]+) to foundations ([0-9]+)\z/
                )
                : ( ($col_idx) =
                        $move_line =~
m/\AMove a card from stack ([0-9]+) to the foundations\z/
                )
                )
            {
                if ( not $IS_DETAILED_MOVE )
                {
                    $foundation_idx = 0;
                }
            }
            else
            {
                die
"Incorrect format for move line no. $move_line_idx - '$move_line'";
            }
        }

        if ( !defined $card )
        {
            if ( ( $col_idx < 0 ) or ( $col_idx >= $NUM_COLUMNS ) )
            {
                die "Invalid column index '$col_idx' at $move_line_idx";
            }
        }

        $assert_empty_line->();
        my ( $info_line, $info_line_idx );
        if ( not $IS_DETAILED_MOVE )
        {
            ( $info_line, $info_line_idx ) = $get_line->();
            if ( $info_line !~ m/\AInfo: Card moved is ($CARD_RE)\z/ )
            {
                die
"Invalid format for info line no. $info_line_idx - '$info_line'";
            }

            $moved_card_str = $1;

            $assert_empty_line->();
            $assert_empty_line->();

            my ( $sep_line, $sep_line_idx ) = $get_line->();

            if ( $sep_line !~ m/\A=+\z/ )
            {
                die
"Invalid format for separator line no. $sep_line_idx - '$sep_line'";
            }

            $assert_empty_line->();
        }

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
            my $col                = $columns->[$col_idx];
            my $top_card           = $col->top();
            my $top_card_moved_str = $top_card->to_string();

            if ( $top_card_moved_str ne $moved_card_str )
            {
                die
"Card moved should be '$top_card_moved_str', but the info says it is '$moved_card_str' at line $info_line_idx";
            }

            my $found_card = $self->_foundation->cell($foundation_idx);
            if ( defined($found_card) )
            {
                my $found_rank = $found_card->rank();
                my $src_rank   = $top_card->rank();

                my $delta = abs( $src_rank - $found_rank );
                if (
                    not( $delta == 1 or $delta == ( $MAX_RANK - 1 ) )
                    or (
                            $IS_GOLF
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
                    die
"Cannot put $top_card_moved_str in the foundations that contain "
                        . $found_card->to_string();
                }
                if ($IS_DISPLAYED_BOARD)
                {
                    my ( $line, $line_idx ) = $get_line->();
                    my $wanted_line = $self->_foundation->to_string();
                    $wanted_line =~ s#\AFreecells:#Foundations:#
                        or Carp::confess("Unimpl!");
                    $wanted_line =~ s#  # #g;
                    my $fstr = $found_card->to_string();
                    my $tstr = $top_card->to_string();
                    $wanted_line =~
s#\AFoundations:(?: $CARD_RE){$foundation_idx} \K(\Q$fstr\E)#my$c=$1;"[ $c → $tstr ]"#e
                        or Carp::confess(
"Failed substitute! foundation_idx=$foundation_idx wanted_line=$wanted_line fstr='$fstr'"
                        );
                    if ( $line ne $wanted_line )
                    {
                        Carp::confess(
                            "Foundations str is '$line' vs. '$wanted_line'");
                    }
                }
            }
            $card = $col->pop;
            --$remaining_cards;
        }
        if ( not defined $foundation_idx )
        {
            die "\$foundation_idx not set";
        }
        $self->_set_found( $foundation_idx, $card, );
        if ($CHECK_EMPTY)
        {
            if ( $remaining_cards == 0 )
            {
                last MOVES;
            }
        }
    }
    return;
}

1;
