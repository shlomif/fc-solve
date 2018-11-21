package FC_Solve::SplitCmdLine;

use strict;
use warnings;

use FC_Solve::Paths qw( normalize_lf );

use FC_Solve::InlineWrap (
    C => <<"EOF",
#include "split_cmd_line.c"

SV * _internal_parse_args(char * input) {
    AV *const results = (AV *)sv_2mortal((SV *)newAV());
    fcs_args_man args = fc_solve_args_man_chop(input);

    for ( int i=0 ; i < args.argc ; i++)
    {
        av_push(results, newSVpv(args.argv[i], 0));
    }

    fc_solve_args_man_free(&args);
    return newRV((SV *)results);
}

EOF
);

sub split_cmd_line_string
{
    my ( $class, $input_string ) = @_;
    return _internal_parse_args( normalize_lf($input_string) );
}

1;

__END__

=head1 COPYRIGHT AND LICENSE

This file is part of Freecell Solver. It is subject to the license terms in
the COPYING.txt file found in the top-level directory of this distribution
and at http://fc-solve.shlomifish.org/docs/distro/COPYING.html . No part of
Freecell Solver, including this file, may be copied, modified, propagated,
or distributed except according to the terms contained in the COPYING file.

Copyright (c) 2013 Shlomi Fish

=cut
