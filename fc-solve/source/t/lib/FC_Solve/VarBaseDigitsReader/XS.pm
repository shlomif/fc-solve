package FC_Solve::VarBaseDigitsReader::XS;

use strict;
use warnings;

use FC_Solve::InlineWrap (
    C => <<'EOF',
#include "var_base_reader.h"
#include "inline.h"

SV* _proto_new(SV * data_proto) {

        fcs_var_base_reader_t * s;
        New(42, s, 1, fcs_var_base_reader_t);

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

static GCC_INLINE fcs_var_base_reader_t * _var_base_deref(SV * const obj) {
    return ((fcs_var_base_reader_t *)SvIV(SvRV(obj)));
}

int _var_base_reader__read(SV* obj, int base) {
    return fc_solve_var_base_reader_read(_var_base_deref(obj), base);
}

void DESTROY(SV* obj) {
    fcs_var_base_reader_t * const s = _var_base_deref(obj);
    fc_solve_var_base_reader_release(s);
    Safefree(s);
}

EOF
    LIBS => "-L" . $ENV{FCS_PATH} . ' -lgmp',
);

sub new
{
    my $class = shift;
    my $args = shift;
    return FC_Solve::VarBaseDigitsReader::XS::_proto_new($args->{data});
}

sub read
{
    my ($self, $base) = @_;

    return _var_base_reader__read($self, $base);
}

1;

=head1 COPYRIGHT AND LICENSE

Copyright (c) 2011 Shlomi Fish

Permission is hereby granted, free of charge, to any person
obtaining a copy of this software and associated documentation
files (the "Software"), to deal in the Software without
restriction, including without limitation the rights to use,
copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the
Software is furnished to do so, subject to the following
conditions:

The above copyright notice and this permission notice shall be
included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
OTHER DEALINGS IN THE SOFTWARE.

=cut

