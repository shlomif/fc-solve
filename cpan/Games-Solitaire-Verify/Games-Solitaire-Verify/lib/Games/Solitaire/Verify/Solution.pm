package Games::Solitaire::Verify::Solution;

use warnings;
use strict;

use 5.008;

=head1 NAME

Games::Solitaire::Verify::Solution - verify an entire solution
of Freecell Solver (or a similar solve)

=head1 VERSION

Version 0.0101

=cut

our $VERSION = '0.1203';

use parent 'Games::Solitaire::Verify::Base';

use Games::Solitaire::Verify::Exception;
use Games::Solitaire::Verify::Card;
use Games::Solitaire::Verify::Column;
use Games::Solitaire::Verify::Move;
use Games::Solitaire::Verify::State;

__PACKAGE__->mk_acc_ref([qw(
    _input_fh
    _line_num
    _variant
    _variant_params
    _state
    _move
    _reached_end
    )]);

=head1 SYNOPSIS

    use Games::Solitaire::Verify::Solution;

    my $input_filename = "freecell-24-solution.txt";

    open (my $input_fh, "<", $input_filename)
        or die "Cannot open file $!";

    # Initialise a column
    my $solution = Games::Solitaire::Verify::Solution->new(
        {
            input_fh => $input_fh,
            variant => "freecell",
        },
    );

    my $ret = $solution->verify();

    close($input_fh);

    if ($ret)
    {
        die $ret;
    }
    else
    {
        print "Solution is OK";
    }

=head1 FUNCTIONS

=head2 Games::Solitaire::Verify::Solution->new({variant => $variant, input_fh => $input_fh})

Constructs a new solution verifier with the variant $variant (see
L<Games::Solitaire::Verify::VariantsMap> ), and the input file handle
$input_fh.

If $variant is C<"custom">, then the constructor also requires a
C<'variant_params'> key which should be a populated
L<Games::Solitaire::Verify::VariantParams> object.

=cut

sub _init
{
    my ($self, $args) = @_;

    $self->_variant($args->{variant});

    if ($self->_variant() eq "custom")
    {
        $self->_variant_params($args->{variant_params});
    }
    $self->_input_fh($args->{input_fh});
    $self->_state(undef);
    $self->_line_num(0);
    $self->_reached_end(0);

    return 0;
}

sub _calc_variant_args
{
    my $self = shift;

    my @ret;
    if ($self->_variant() eq "custom")
    {
        push @ret, ('variant_params' => $self->_variant_params());
    }
    push @ret, (variant => $self->_variant());
    return \@ret;
}

sub _read_state
{
    my $self = shift;

    my $line = $self->_get_line();

    if ($line ne "")
    {
        die "Non empty line before state";
    }

    my $str = "";

    while (($line = $self->_get_line()) && ($line ne ""))
    {
        $str .= $line . "\n";
    }

    my $new_state = Games::Solitaire::Verify::State->new(
            {
                string => $str,
                @{$self->_calc_variant_args()},
            }
        );

    if (!defined($self->_state()))
    {
        # Do nothing.

    }
    else
    {
        if ($self->_state()->to_string() ne $str)
        {
            die "States don't match";
        }
    }
    $self->_state($new_state);

    while (defined($line = $self->_get_line()) && ($line eq ""))
    {
    }

    if ($line !~ m{\A={3,}\z})
    {
        die "No ======== separator";
    }

    return;
}

sub _read_move
{
    my $self = shift;

    my $line = $self->_get_line();

    if ($line ne "")
    {
        die "No empty line before move";
    }

    $line = $self->_get_line();

    if ($line eq "This game is solveable.")
    {
        $self->_reached_end(1);

        return "END";
    }

    $self->_move(Games::Solitaire::Verify::Move->new(
            {
                fcs_string => $line,
                game => $self->_variant(),
            }
        )
    );

    return;
}

sub _apply_move
{
    my $self = shift;

    if (my $verdict = $self->_state()->verify_and_perform_move($self->_move()))
    {
        Games::Solitaire::Verify::Exception::VerifyMove->throw(
            error => "Wrong Move",
            problem => $verdict,
        );
    }

    return;
}

sub _get_line
{
    my $self = shift;

    $self->_line_num($self->_line_num()+1);

    my $ret = readline($self->_input_fh());

    chomp($ret);

    return $ret;
}

=head2 $solution->verify()

Traverse the solution verifying it.

=cut

sub verify
{
    my $self = shift;

    eval {

        my $line = $self->_get_line();

        if ($line !~ m{\A(-=)+-\z})
        {
            die "Incorrect start";
        }

        $self->_read_state();

        while (!defined(scalar($self->_read_move())))
        {
            $self->_apply_move();
            $self->_read_state();
        }
    };

    my $err;
    if (! $@)
    {
        # Do nothing - no exception was thrown.
    }
    elsif ($err =
        Exception::Class->caught('Games::Solitaire::Verify::Exception::VerifyMove'))
    {
        return { error => $err, line_num => $self->_line_num(), };
    }
    else
    {
        $err = Exception::Class->caught();
        ref $err ? $err->rethrow : die $err;
    }

    return;
}


=head1 AUTHOR

Shlomi Fish, L<http://www.shlomifish.org/>.

=head1 BUGS

Please report any bugs or feature requests to C<bug-games-solitaire-verifysolution-move at rt.cpan.org>, or through
the web interface at L<http://rt.cpan.org/NoAuth/ReportBug.html?Queue=Games-Solitaire-Verify>.  I will be notified, and then you'll
automatically be notified of progress on your bug as I make changes.

=head1 SUPPORT

You can find documentation for this module with the perldoc command.

    perldoc Games::Solitaire::Verify::Solution

You can also look for information at:

=over 4

=item * RT: CPAN's request tracker

L<http://rt.cpan.org/NoAuth/Bugs.html?Dist=Games-Solitaire-Verify>

=item * AnnoCPAN: Annotated CPAN documentation

L<http://annocpan.org/dist/Games-Solitaire-Verify>

=item * CPAN Ratings

L<http://cpanratings.perl.org/d/Games-Solitaire-Verify>

=item * Search CPAN

L<http://search.cpan.org/dist/Games-Solitaire-Verify>

=back


=head1 ACKNOWLEDGEMENTS


=head1 COPYRIGHT & LICENSE

Copyright 2008 Shlomi Fish.

This program is released under the following license: MIT/X11
( L<http://www.opensource.org/licenses/mit-license.php> ).

=cut

1; # End of Games::Solitaire::Verify::Move
