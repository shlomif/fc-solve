package FC_Solve::Base64;

use strict;
use warnings;

use FC_Solve::InlineWrap (
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
    l => '-lgmp',
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
