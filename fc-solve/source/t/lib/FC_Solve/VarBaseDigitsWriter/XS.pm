package FC_Solve::VarBaseDigitsWriter::XS;

use strict;
use warnings;

use FC_Solve::InlineWrap (
    C => <<'EOF',
#include "var_base_writer.h"

SV* _proto_new() {
        fcs_var_base_writer * s;
        SV*      obj_ref = newSViv(0);
        SV*      obj = newSVrv(obj_ref, "FC_Solve::VarBaseDigitsWriter::XS");

        New(42, s, 1, fcs_var_base_writer);
        fc_solve_var_base_writer_init(s);
        fc_solve_var_base_writer_start(s);

        sv_setiv(obj, (IV)s);
        SvREADONLY_on(obj);
        return obj_ref;
}

static inline fcs_var_base_writer * deref(SV * const obj) {
    return (fcs_var_base_writer *)SvIV(SvRV(obj));
}

void _proto_write(SV* obj, int base, int item) {
    fc_solve_var_base_writer_write(deref(obj), base, item);
}

SV * _proto_get_data(SV* obj) {
    unsigned char exported[800];
    size_t count = fc_solve_var_base_writer_get_data(deref(obj), exported);

    return newSVpvn(exported, count);
}

void DESTROY(SV* obj) {
  fcs_var_base_writer * s = deref(obj);
  fc_solve_var_base_writer_release(s);
  Safefree(s);
}

EOF
    l => '-lgmp',
);

sub new
{
    my $class = shift;
    return FC_Solve::VarBaseDigitsWriter::XS::_proto_new();
}

sub write
{
    my ( $self, $args ) = @_;

    my $base = $args->{base};
    my $item = $args->{item};

    $self->_proto_write( $base, $item );

    return;
}

sub get_data
{
    my $self = shift;

    return $self->_proto_get_data();
}

1;

__END__

=head1 COPYRIGHT AND LICENSE

This file is part of Freecell Solver. It is subject to the license terms in
the COPYING.txt file found in the top-level directory of this distribution
and at http://fc-solve.shlomifish.org/docs/distro/COPYING.html . No part of
Freecell Solver, including this file, may be copied, modified, propagated,
or distributed except according to the terms contained in the COPYING file.

Copyright (c) 2000 Shlomi Fish

=cut
