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
    eval { system(@cmd); };
    return;
}

1;

=head1 NAME

Code::TidyAll::Plugin::ClangFormat - run clang-format using Code::TidyAll

=head1 SYNOPSIS

In your C<.tidyallrc>:

    [ClangFormat]
    select = **/*.{c,cpp,h,hpp}

Also define a C<.clang-format> at the root_dir .

=head1 DESCRIPTION

This speeds up the clang-format tool ( L<https://clang.llvm.org/docs/ClangFormat.html>)
checking and reformatting, by caching results using L<Code::TidyAll> .

It was originally written for use by Freecell Solver
( L<https://fc-solve.shlomifish.org/> ), an open source automated solver
for some variants of Patience / card solitaire.
games.
