package Games::Solitaire::VerifySolution::Exception;

use strict;
use warnings;

=head1 NAME

Games::Solitaire::VerifySolution::Exception - provides various exception 
classes for G::S::VerifySolution.

=cut

use vars qw($VERSION);

$VERSION = '0.01';

use Exception::Class (
    'Games::Solitaire::VerifySolution::Exception',
    'Games::Solitaire::VerifySolution::Exception::Parse' =>
    { isa => "Games::Solitaire::VerifySolution::Exception", },
    'Games::Solitaire::VerifySolution::Exception::Parse::FCS' =>
    { isa => "Games::Solitaire::VerifySolution::Exception::Parse", },
    'Games::Solitaire::VerifySolution::Exception::Parse::Card' =>
    { isa => "Games::Solitaire::VerifySolution::Exception::Parse", },
    'Games::Solitaire::VerifySolution::Exception::Parse::Card::UnknownRank' =>
    { isa => "Games::Solitaire::VerifySolution::Exception::Parse::Card", },
    'Games::Solitaire::VerifySolution::Exception::Parse::Card::UnknownSuit' =>
    { isa => "Games::Solitaire::VerifySolution::Exception::Parse::Card", },
    'Games::Solitaire::VerifySolution::Exception::Parse::State' =>
    { isa => "Games::Solitaire::VerifySolution::Exception::Parse", },
    "Games::Solitaire::VerifySolution::Exception::Parse::State::Foundations" =>
    { isa => "Games::Solitaire::VerifySolution::Exception::Parse::State", },
    'Games::Solitaire::VerifySolution::Exception::Variant' =>
    { isa => "Games::Solitaire::VerifySolution::Exception", 
      fields => ["variant"],
    },
    'Games::Solitaire::VerifySolution::Exception::Variant::Unknown' =>
    { isa => "Games::Solitaire::VerifySolution::Exception::Variant", },
      );

=head1 SYNOPSIS

    use Games::Solitaire::VerifySolution::Exception;

=head1 DESCRIPTION

These are L<Exception:Class> exceptions for L<Games::Solitaire::VerifySolution> .

=cut

=head1 FUNCTIONS

=head2 new($args)

The constructor. Blesses and calls _init() .

=cut

=head1 AUTHOR

Shlomi Fish, C<< <shlomif at iglu.org.il> >>

=head1 BUGS

Please report any bugs or feature requests to C<bug-games-solitaire-verifysolution-move at rt.cpan.org>, or through
the web interface at L<http://rt.cpan.org/NoAuth/ReportBug.html?Queue=Games-Solitaire-VerifySolution>.  I will be notified, and then you'll
automatically be notified of progress on your bug as I make changes.

=head1 SUPPORT

You can find documentation for this module with the perldoc command.

    perldoc Games::Solitaire::VerifySolution


You can also look for information at:

=over 4

=item * RT: CPAN's request tracker

L<http://rt.cpan.org/NoAuth/Bugs.html?Dist=Games-Solitaire-VerifySolution>

=item * AnnoCPAN: Annotated CPAN documentation

L<http://annocpan.org/dist/Games-Solitaire-VerifySolution>

=item * CPAN Ratings

L<http://cpanratings.perl.org/d/Games-Solitaire-VerifySolution>

=item * Search CPAN

L<http://search.cpan.org/dist/Games-Solitaire-VerifySolution>

=back


=head1 ACKNOWLEDGEMENTS


=head1 COPYRIGHT & LICENSE

Copyright 2008 Shlomi Fish, all rights reserved.

This program is released under the following license: MIT/X11
( L<http://www.opensource.org/licenses/mit-license.php> ).

=cut

1; # End of Games::Solitaire::VerifySolution::Move
