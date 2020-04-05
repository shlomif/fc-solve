package FC_Solve::Test::CheckResults;

use strict;
use warnings;

use FC_Solve::Paths qw( data_file );
use FC_Solve::CheckResults ();

my $v = FC_Solve::CheckResults->new(
    {
        data_filename => data_file( ['digests-and-lens-storage.yml'] ),
    }
);

# Short for run.
sub r
{
    local $Test::Builder::Level = $Test::Builder::Level + 1;

    my ( $args, $msg ) = @_;

    return $v->vtest( $args, $msg );
}

1;

__END__

=head1 COPYRIGHT AND LICENSE

This file is part of Freecell Solver. It is subject to the license terms in
the COPYING.txt file found in the top-level directory of this distribution
and at http://fc-solve.shlomifish.org/docs/distro/COPYING.html . No part of
Freecell Solver, including this file, may be copied, modified, propagated,
or distributed except according to the terms contained in the COPYING file.

Copyright (c) 2009 Shlomi Fish

=cut
