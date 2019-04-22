package File::Find::CaseCollide;

use strict;
use warnings;
use 5.014;
use File::Find::Object ();
use Moo;

has '_results' => ( is => 'rw' );
has '_ffo'     => ( is => 'rw' );
has 'dir'      => ( is => 'ro' );

sub _iter
{
    my ( $self, $ffo ) = @_;
    if ( my $r = $ffo->next_obj() )
    {
        if ( $r->is_dir() )
        {
            my %found;
            my %pos;
            foreach my $fn ( @{ $ffo->get_current_node_files_list() } )
            {
                my $k = lc $fn;
                my $v = $found{$k} //= [];
                push @$v, $fn;
                $pos{$k} = $v if @$v == 2;
            }

            if (%pos)
            {
                $self->_results->{ $r->path() } = \%pos;
            }
        }
        return 1;
    }
    return;
}

sub find
{
    my ($self) = @_;

    $self->_results( {} );
    my $ffo = $self->_ffo( File::Find::Object->new( {}, $self->dir ) );

    while ( $self->_iter($ffo) )
    {
    }

    return $self->_results;
}

1;

__END__

=pod

=head1 NAME

File::Find::CaseCollide - find collisions in filenames, differing only in case

=head1 VERSION

version 0.0.1

=head1 SYNOPSIS

    use File::Find::CaseCollide ();

    my $obj = File::Find::CaseCollide->new( { dir => '.' } );
    my $results = $obj->find;

=head1 DESCRIPTION

This tests for filenames in the same directory which differ only in lowercase
vs uppercase letters which some filesystems do not support (e.g: "hello.txt" vs.
"Hello.txt").

=head1 VERSION

version 0.0.1

=head1 METHODS

=head2 dir

Pass it as a parameter with a path to the directory tree to traverse.

=head2 $obj->find()

Traverses the tree and returns the collisions as a hash reference - hopefully
empty.

=head1 SEE ALSO

L<https://www.jamendo.com/album/59248/sense> - Sense by LadyLau, a CC-licensed
album.

=head1 AUTHOR

Shlomi Fish <shlomif@cpan.org>

=head1 COPYRIGHT AND LICENSE

This software is Copyright (c) 2018 by Shlomi Fish.

This is free software, licensed under:

  The MIT (X11) License

=head1 BUGS

Please report any bugs or feature requests on the bugtracker website
L<https://github.com/shlomif/file-find-casecollide/issues>

When submitting a bug or request, please include a test-file or a
patch to an existing test-file that illustrates the bug or desired
feature.

=for :stopwords cpan testmatrix url annocpan anno bugtracker rt cpants kwalitee diff irc mailto metadata placeholders metacpan

=head1 SUPPORT

=head2 Perldoc

You can find documentation for this module with the perldoc command.

  perldoc File::Find::CaseCollide

=head2 Websites

The following websites have more information about this module, and may be of help to you. As always,
in addition to those websites please use your favorite search engine to discover more resources.

=over 4

=item *

MetaCPAN

A modern, open-source CPAN search engine, useful to view POD in HTML format.

L<https://metacpan.org/release/File-Find-CaseCollide>

=item *

Search CPAN

The default CPAN search engine, useful to view POD in HTML format.

L<http://search.cpan.org/dist/File-Find-CaseCollide>

=item *

RT: CPAN's Bug Tracker

The RT ( Request Tracker ) website is the default bug/issue tracking system for CPAN.

L<https://rt.cpan.org/Public/Dist/Display.html?Name=File-Find-CaseCollide>

=item *

AnnoCPAN

The AnnoCPAN is a website that allows community annotations of Perl module documentation.

L<http://annocpan.org/dist/File-Find-CaseCollide>

=item *

CPAN Ratings

The CPAN Ratings is a website that allows community ratings and reviews of Perl modules.

L<http://cpanratings.perl.org/d/File-Find-CaseCollide>

=item *

CPANTS

The CPANTS is a website that analyzes the Kwalitee ( code metrics ) of a distribution.

L<http://cpants.cpanauthors.org/dist/File-Find-CaseCollide>

=item *

CPAN Testers

The CPAN Testers is a network of smoke testers who run automated tests on uploaded CPAN distributions.

L<http://www.cpantesters.org/distro/F/File-Find-CaseCollide>

=item *

CPAN Testers Matrix

The CPAN Testers Matrix is a website that provides a visual overview of the test results for a distribution on various Perls/platforms.

L<http://matrix.cpantesters.org/?dist=File-Find-CaseCollide>

=item *

CPAN Testers Dependencies

The CPAN Testers Dependencies is a website that shows a chart of the test results of all dependencies for a distribution.

L<http://deps.cpantesters.org/?module=File::Find::CaseCollide>

=back

=head2 Bugs / Feature Requests

Please report any bugs or feature requests by email to C<bug-file-find-casecollide at rt.cpan.org>, or through
the web interface at L<https://rt.cpan.org/Public/Bug/Report.html?Queue=File-Find-CaseCollide>. You will be automatically notified of any
progress on the request by the system.

=head2 Source Code

The code is open to the world, and available for you to hack on. Please feel free to browse it and play
with it, or whatever. If you want to contribute patches, please send me a diff or prod me to pull
from your repository :)

L<https://github.com/shlomif/perl-File-Find-CaseCollide>

  git clone https://github.com/shlomif/perl-File-Find-CaseCollide.git

=cut
