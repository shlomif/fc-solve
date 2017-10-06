package FC_Solve::Test::Verify;

use strict;
use warnings;

use FC_Solve::Paths
    qw( bin_file is_freecell_only is_without_patsolve samp_board );
use Path::Tiny qw( path );

# Short for run.
sub r
{
    local $Test::Builder::Level = $Test::Builder::Level + 1;

    my ( $args, $msg ) = @_;

    require FC_Solve::GetOutput;

    if ( exists $args->{variant} and is_freecell_only() )
    {
        return Test::More::ok( 1, 'skipped due to freecell-only' );
    }
    if ( exists( $args->{uses_patsolve} ) and is_without_patsolve() )
    {
        return Test::More::ok( 1,
            q#Test skipped because it uses patsolve on a build without it.# );
    }
    if ( defined( my $bb = delete( $args->{samp_board} ) ) )
    {
        $args->{board} = samp_board($bb);
    }
    my $id = delete $args->{id};

    my $cmd_line        = FC_Solve::GetOutput->new($args);
    my $fc_solve_output = $cmd_line->open_cmd_line->{fh};
    my $contents        = do { local $/; <$fc_solve_output>; };
    close($fc_solve_output);
    $contents =~ s/ +$//gm;
    require Digest::SHA;
    my $hasher = Digest::SHA->new(256);
    $hasher->add($contents);
    my $got_sum = $hasher->hexdigest;

    my $fn = bin_file( [ 't', 'verify-cache', "$id.sha" ] );
    if ( -e $fn )
    {
        return Test::More::is(
            $got_sum,
            path($fn)->slurp_raw,
            "Cached checksum matches."
        );
    }
    require Games::Solitaire::Verify::Solution;
    require Games::Solitaire::Verify::VariantParams;

    # Initialise a column
    open my $in, '<', \$contents;
    my $solution = Games::Solitaire::Verify::Solution->new(
        {
            input_fh => $in,
            variant  => $cmd_line->variant,
            (
                $cmd_line->is_custom
                ? (
                    variant_params =>
                        Games::Solitaire::Verify::VariantParams->new(
                        $args->{variant_params}
                        )
                    )
                : ()
            ),
        },
    );

    my $verdict = $solution->verify();
    my $test_verdict = Test::More::ok( !$verdict, $msg );

    if ( !$test_verdict )
    {
        require Data::Dumper;
        Test::More::diag( "Verdict == " . Data::Dumper::Dumper($verdict) );
    }
    else
    {
        path($fn)->spew_raw($got_sum);
    }

    return $test_verdict;
}

1;
__END__

sub _run_test
{
    local $Test::Builder::Level = $Test::Builder::Level + 1;
    return r(@_);
}

sub run_id
{
    local $Test::Builder::Level = $Test::Builder::Level + 1;

    my ($s, $args) = @_;

    return _run_test(@{ $args->{data} }{qw(args msg)});
}

__END__

=head1 COPYRIGHT AND LICENSE

This file is part of Freecell Solver. It is subject to the license terms in
the COPYING.txt file found in the top-level directory of this distribution
and at http://fc-solve.shlomifish.org/docs/distro/COPYING.html . No part of
Freecell Solver, including this file, may be copied, modified, propagated,
or distributed except according to the terms contained in the COPYING file.

Copyright (c) 2009 Shlomi Fish

=cut
