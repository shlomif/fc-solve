package FC_Solve::Test::Valgrind;

use strict;
use warnings;

use Test::More ();
use Carp       ();
use File::Spec ();
use File::Temp qw( tempdir );
use FC_Solve::Paths
    qw( $IS_WIN bin_board bin_exe_raw is_without_dbm is_without_valgrind samp_board samp_preset );

use Test::RunValgrind ();

sub _expand_arg
{
    my $hash_ref = shift;

    my $type = $hash_ref->{type};

    if ( $type eq 'tempdir' )
    {
        return tempdir( CLEANUP => 1 ),;
    }
    elsif ( $type eq 'ENV' )
    {
        return $ENV{ $hash_ref->{arg} };
    }
    elsif ( $type eq 'bin_board' )
    {
        return bin_board( $hash_ref->{arg} );
    }
    elsif ( $type eq 'sample_board' )
    {
        return samp_board( $hash_ref->{arg} );
    }
    elsif ( $type eq 'sample_preset' )
    {
        return samp_preset( $hash_ref->{arg} );
    }
    elsif ( $type eq 'catfile' )
    {
        my $prefix = exists( $hash_ref->{prefix} ) ? $hash_ref->{prefix} : '';
        return $prefix
            . File::Spec->catfile(
            map { ( ref($_) eq 'HASH' ) ? _expand_arg($_) : $_ }
                @{ $hash_ref->{args} } );
    }
    else
    {
        Carp::confess("Unknown arg type '$type'!");
    }
}

# Short for run.
sub r
{
    my ( $args, $msg ) = @_;

SKIP:
    {
        if ($IS_WIN)
        {
            Test::More::skip( "valgrind is skipped due to running on windows.",
                1 );
        }
        elsif ( is_without_valgrind() )
        {
            Test::More::skip(
                "valgrind is skipped for google-dense/sparse-hash.", 1 );
        }
        elsif ( is_without_dbm() and $args->{dbm} )
        {
            Test::More::skip( "Skipping valgrind test for no_dbm solvers", 1 );
        }
        elsif (
            !Test::RunValgrind->new( { supress_stderr => 1, } )->run(
                {
                    log_fn => $args->{log_fn},
                    blurb  => $msg,
                    prog   => bin_exe_raw( [ $args->{prog} ] ),
                    argv   => [
                        map { ( ref($_) eq 'HASH' ) ? _expand_arg($_) : $_ }
                            @{ $args->{argv} }
                    ],
                }
            )
            )
        {
            die "Valgrind failed";
        }
    }
}

1;

__END__

sub _run_test
{
    return r(@_);
}

sub run_id
{
    local $Test::Builder::Level = $Test::Builder::Level + 1;

    my (undef, $args) = @_;

    return _run_test(@$args{qw(id data)});
}

__END__

=head1 COPYRIGHT AND LICENSE

This file is part of Freecell Solver. It is subject to the license terms in
the COPYING.txt file found in the top-level directory of this distribution
and at http://fc-solve.shlomifish.org/docs/distro/COPYING.html . No part of
Freecell Solver, including this file, may be copied, modified, propagated,
or distributed except according to the terms contained in the COPYING file.

Copyright (c) 2000 Shlomi Fish

=cut
