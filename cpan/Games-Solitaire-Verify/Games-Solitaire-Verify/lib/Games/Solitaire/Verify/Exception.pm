package Games::Solitaire::Verify::Exception;

use strict;
use warnings;

=head1 NAME

Games::Solitaire::Verify::Exception - provides various exception
classes for G::S::Verify.

=cut

use vars qw($VERSION);

$VERSION = '0.1202';

use Exception::Class (
    'Games::Solitaire::Verify::Exception',
    'Games::Solitaire::Verify::Exception::Parse' =>
    { isa => "Games::Solitaire::Verify::Exception", },
    'Games::Solitaire::Verify::Exception::Parse::FCS' =>
    { isa => "Games::Solitaire::Verify::Exception::Parse", },
    'Games::Solitaire::Verify::Exception::Parse::Card' =>
    { isa => "Games::Solitaire::Verify::Exception::Parse", },
    'Games::Solitaire::Verify::Exception::Parse::Card::UnknownRank' =>
    { isa => "Games::Solitaire::Verify::Exception::Parse::Card", },
    'Games::Solitaire::Verify::Exception::Parse::Card::UnknownSuit' =>
    { isa => "Games::Solitaire::Verify::Exception::Parse::Card", },
    'Games::Solitaire::Verify::Exception::Parse::Column' =>
    { isa => "Games::Solitaire::Verify::Exception::Parse", },
    'Games::Solitaire::Verify::Exception::Parse::Column::Prefix' =>
    { isa => "Games::Solitaire::Verify::Exception::Parse::Column", },
    'Games::Solitaire::Verify::Exception::Parse::State' =>
    { isa => "Games::Solitaire::Verify::Exception::Parse", },
    "Games::Solitaire::Verify::Exception::Parse::State::Foundations" =>
    { isa => "Games::Solitaire::Verify::Exception::Parse::State", },
    "Games::Solitaire::Verify::Exception::Parse::State::Freecells" =>
    { isa => "Games::Solitaire::Verify::Exception::Parse::State", },
    "Games::Solitaire::Verify::Exception::Parse::State::Column" =>
    { isa => "Games::Solitaire::Verify::Exception::Parse::State",
      fields => ["index"],
    },

    "Games::Solitaire::Verify::Exception::VariantParams::Param" =>
    {
        isa => "Games::Solitaire::Verify::Exception",
        fields => ["value"],
    },
    "Games::Solitaire::Verify::Exception::VariantParams::Param::NumDecks" =>
    {
        isa => "Games::Solitaire::Verify::Exception::VariantParams::Param",
    },
    "Games::Solitaire::Verify::Exception::VariantParams::Param::EmptyStacksFill" =>
    {
        isa => "Games::Solitaire::Verify::Exception::VariantParams::Param",
    },
    "Games::Solitaire::Verify::Exception::VariantParams::Param::Stacks" =>
    {
        isa => "Games::Solitaire::Verify::Exception::VariantParams::Param",
    },
    "Games::Solitaire::Verify::Exception::VariantParams::Param::Freecells" =>
    {
        isa => "Games::Solitaire::Verify::Exception::VariantParams::Param",
    },
    "Games::Solitaire::Verify::Exception::VariantParams::Param::SeqMove" =>
    {
        isa => "Games::Solitaire::Verify::Exception::VariantParams::Param",
    },
    "Games::Solitaire::Verify::Exception::VariantParams::Param::SeqBuildBy" =>
    {
        isa => "Games::Solitaire::Verify::Exception::VariantParams::Param",
    },
    "Games::Solitaire::Verify::Exception::VariantParams::Param::Rules" =>
    {
        isa => "Games::Solitaire::Verify::Exception::VariantParams::Param",
    },

    'Games::Solitaire::Verify::Exception::Variant' =>
    { isa => "Games::Solitaire::Verify::Exception",
      fields => ["variant"],
    },
    'Games::Solitaire::Verify::Exception::Variant::Unknown' =>
    { isa => "Games::Solitaire::Verify::Exception::Variant", },
    'Games::Solitaire::Verify::Exception::VerifyMove' =>
    { isa => "Games::Solitaire::Verify::Exception",
        fields => ["problem"],
    },
    'Games::Solitaire::Verify::Exception::Move' =>
    { isa => "Games::Solitaire::Verify::Exception",
        fields => ["move"],
    },
   'Games::Solitaire::Verify::Exception::Move::Variant::Unsupported' =>
    { isa => "Games::Solitaire::Verify::Exception::Move", },
    'Games::Solitaire::Verify::Exception::Move::NotEnoughEmpties' =>
    { isa => "Games::Solitaire::Verify::Exception::Move", },
    'Games::Solitaire::Verify::Exception::Move::Src' =>
    { isa => "Games::Solitaire::Verify::Exception::Move", },
    'Games::Solitaire::Verify::Exception::Move::Src::Col' =>
    { isa => "Games::Solitaire::Verify::Exception::Move::Src", },
    'Games::Solitaire::Verify::Exception::Move::Src::Col::NoCards' =>
    { isa => "Games::Solitaire::Verify::Exception::Move::Src::Col", },
    'Games::Solitaire::Verify::Exception::Move::Src::Col::NonSequence' =>
    { isa => "Games::Solitaire::Verify::Exception::Move::Src::Col",
      fields => [qw(pos)],
    },
    'Games::Solitaire::Verify::Exception::Move::Src::Col::NotEnoughCards' =>
    { isa => "Games::Solitaire::Verify::Exception::Move::Src::Col", },
    'Games::Solitaire::Verify::Exception::Move::Src::Col::NotTrueSeq' =>
    { isa => "Games::Solitaire::Verify::Exception::Move::Src::Col", },
    'Games::Solitaire::Verify::Exception::Move::Src::Freecell::Empty' =>
    { isa => "Games::Solitaire::Verify::Exception::Move::Src", },
    'Games::Solitaire::Verify::Exception::Move::Dest' =>
    { isa => "Games::Solitaire::Verify::Exception::Move", },
    'Games::Solitaire::Verify::Exception::Move::Dest::Foundation' =>
    { isa => "Games::Solitaire::Verify::Exception::Move::Dest", },
    'Games::Solitaire::Verify::Exception::Move::Dest::Freecell' =>
    { isa => "Games::Solitaire::Verify::Exception::Move::Dest", },
    'Games::Solitaire::Verify::Exception::Move::Dest::Col' =>
    { isa => "Games::Solitaire::Verify::Exception::Move::Dest", },
    'Games::Solitaire::Verify::Exception::Move::Dest::Col::NonMatchSuits' =>
    { isa => "Games::Solitaire::Verify::Exception::Move::Dest::Col",
      fields => [qw(seq_build_by)],
      },
    'Games::Solitaire::Verify::Exception::Move::Dest::Col::OnlyKingsCanFillEmpty' =>
    { isa => "Games::Solitaire::Verify::Exception::Move::Dest::Col",
      },
    'Games::Solitaire::Verify::Exception::Move::Dest::Col::RankMismatch' =>
    { isa => "Games::Solitaire::Verify::Exception::Move::Dest::Col",
      },

      );

=head1 SYNOPSIS

    use Games::Solitaire::Verify::Exception;

=head1 DESCRIPTION

These are L<Exception:Class> exceptions for L<Games::Solitaire::Verify> .

=cut

=head1 FUNCTIONS

=head2 new($args)

The constructor. Blesses and calls _init() .

=cut

=head1 AUTHOR

Shlomi Fish, L<http://www.shlomifish.org/>.

=head1 BUGS

Please report any bugs or feature requests to C<bug-games-solitaire-verifysolution-move at rt.cpan.org>, or through
the web interface at L<http://rt.cpan.org/NoAuth/ReportBug.html?Queue=Games-Solitaire-Verify>.  I will be notified, and then you'll
automatically be notified of progress on your bug as I make changes.

=head1 SUPPORT

You can find documentation for this module with the perldoc command.

    perldoc Games::Solitaire::Verify


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
