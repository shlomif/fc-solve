package Code::TidyAll::Plugin::ClangFormat;

use Moo;

use Path::Tiny qw/ path tempdir tempfile cwd /;

extends 'Code::TidyAll::Plugin';

my $FMT = ".clang-format";

sub transform_file
{
    my ( $self, $fn ) = @_;

    # print cwd(), "\n";
    path($fn)->parent->child($FMT)
        ->spew_raw( path( $self->tidyall->root_dir )->child($FMT)->slurp_raw );
    my @cmd = ( 'clang-format', '-style=file', '-i', $fn );
    if ( system(@cmd) )
    {
        die 'not valid';
    }
    return;
}

1;

=head1 NAME

Code::TidyAll::Plugin::Flake8 - run flake8 using Code::TidyAll

=head1 SYNOPSIS

In your C<.tidyallrc>:

    [Flake8]
    select = **/*.py

=head1 DESCRIPTION

This speeds up the flake8 python tool ( L<http://flake8.pycqa.org/en/latest/>
) checking by caching results using L<Code::TidyAll> .

It was originally written for use by PySolFC
( L<http://pysolfc.sourceforge.net/> ), an open suite of card solitaire
games.
