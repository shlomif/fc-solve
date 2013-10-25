package FC_Solve::Base64;

use strict;
use warnings;

use Inline (
    C => <<'EOF',
#include "fcs_base64.h"

#define MAX_SIZE 10*1024
#
SV * _fcs_base64_encode(SV * data_sv) {
    STRLEN len;
    char * data = SvPVbyte(data_sv, len);
    char result[MAX_SIZE];
    size_t out_len = 0;
    base64_encode((unsigned char *)data, len, result, &out_len);

    SV * ret = newSVpvn(result, out_len);

    return ret;
}

SV * _fcs_base64_decode(SV * enc_sv) {
    STRLEN len;
    char * enc = SvPVbyte(enc_sv, len);
    unsigned char result[MAX_SIZE];
    size_t out_len = 0;
    base64_decode((unsigned char *)enc, len, result, &out_len);

    SV * ret = newSVpvn(result, out_len);

    return ret;
}

void _fcs_base64_build_decoding_table() {
    build_decoding_table();
}

EOF
    NAME => "FC_Solve::Base64",
    CLEAN_AFTER_BUILD => 0,
    INC => "-I$ENV{FCS_PATH} -I$ENV{FCS_SRC_PATH}",
    LIBS => "-L" . $ENV{FCS_PATH} . ' -lgmp',
    OPTIMIZE => '-g -std=gnu99',
);


sub base64_encode
{
    return _fcs_base64_encode(shift);
}

sub base64_decode
{
    return _fcs_base64_decode(shift);
}

BEGIN
{
    _fcs_base64_build_decoding_table();
};

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

