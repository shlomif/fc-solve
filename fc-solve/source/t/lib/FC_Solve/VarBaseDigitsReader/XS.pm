package FC_Solve::VarBaseDigitsReader::XS;

use strict;
use warnings;

use FC_Solve::InlineWrap (
    C => <<'EOF',
#include "var_base_reader.h"

SV* _proto_new(SV * data_proto) {

        fcs_var_base_reader * s;
        New(42, s, 1, fcs_var_base_reader);

        STRLEN data_len;
        unsigned char * const data = (unsigned char *)sv_2pvbyte(data_proto, &data_len);
        fc_solve_var_base_reader_init(s);
        fc_solve_var_base_reader_start(s, data, data_len);

        SV * const obj_ref = newSViv(0);
        SV * const obj = newSVrv(obj_ref, "FC_Solve::VarBaseDigitsReader::XS");
        sv_setiv(obj, (IV)s);
        SvREADONLY_on(obj);
        return obj_ref;
}

static inline fcs_var_base_reader * _var_base_deref(SV * const obj) {
    return ((fcs_var_base_reader *)SvIV(SvRV(obj)));
}

int _var_base_reader__read(SV* obj, int base) {
    return fc_solve_var_base_reader_read(_var_base_deref(obj), base);
}

void DESTROY(SV* obj) {
    fcs_var_base_reader * const s = _var_base_deref(obj);
    fc_solve_var_base_reader_release(s);
    Safefree(s);
}

EOF
    l => '-lgmp',
);

sub new
{
    my $class = shift;
    my $args  = shift;
    return FC_Solve::VarBaseDigitsReader::XS::_proto_new( $args->{data} );
}

sub read
{
    my ( $self, $base ) = @_;

    return _var_base_reader__read( $self, $base );
}

1;
__END__

=head1 COPYRIGHT AND LICENSE

This file is part of Freecell Solver. It is subject to the license terms in
the COPYING.txt file found in the top-level directory of this distribution
and at http://fc-solve.shlomifish.org/docs/distro/COPYING.html . No part of
Freecell Solver, including this file, may be copied, modified, propagated,
or distributed except according to the terms contained in the COPYING file.

Copyright (c) 2011 Shlomi Fish

=cut
