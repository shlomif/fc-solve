package Games::Solitaire::FC_Solve::DeltaStater::VariableBaseDigitsReader::XS;

use strict;
use warnings;

use Inline (
    C => <<'EOF',
#include "gmp.h"

typedef struct
{
    mpz_t data;
} fcs_var_base_reader_t;

SV* _proto_new(SV * data_proto) {
        fcs_var_base_reader_t * s;
        unsigned char * data;
        STRLEN data_len;
        SV*      obj_ref = newSViv(0);
        SV*      obj = newSVrv(obj_ref, "Games::Solitaire::FC_Solve::DeltaStater::VariableBaseDigitsReader::XS");

        New(42, s, 1, fcs_var_base_reader_t);

        data = (unsigned char *)sv_2pvbyte(data_proto, &data_len);
        mpz_init(s->data);
        mpz_import(s->data, data_len, 1, sizeof(data[0]), 0, 0, data);

        sv_setiv(obj, (IV)s);
        SvREADONLY_on(obj);
        return obj_ref;
}

#define DEREF() ((fcs_var_base_reader_t *)SvIV(SvRV(obj)))

int _var_base_reader__read(SV* obj, int base) {
    fcs_var_base_reader_t * reader;
    mpz_t q, r;
    int ret;

    reader = DEREF();

    mpz_init(q);
    mpz_init(r);
    mpz_fdiv_qr_ui(q, r, reader->data, (unsigned long)base);

    ret = (int)mpz_get_ui(r);
    mpz_clear(r);

    mpz_set(reader->data, q);
    mpz_clear(q);

    return ret;
}

void DESTROY(SV* obj) {
    fcs_var_base_reader_t * s = DEREF();
    mpz_clear(s->data);
    Safefree(s);
}

EOF
    NAME => "Games::Solitaire::FC_Solve::DeltaStater::VariableBaseDigitsReader::XS",
    CLEAN_AFTER_BUILD => 0,
    INC => "-I" . $ENV{FCS_PATH},
    LIBS => "-L" . $ENV{FCS_PATH} . ' -lgmp',
    OPTIMIZE => '-g',
);

sub new
{
    my $class = shift;
    my $args = shift;
    return
Games::Solitaire::FC_Solve::DeltaStater::VariableBaseDigitsReader::XS::_proto_new($args->{data});
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

