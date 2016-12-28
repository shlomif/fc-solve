package Shlomif::Spelling::Iface;

use strict;
use warnings;

use MooX (qw( late ));

use Shlomif::Spelling::Check ();
use Shlomif::Spelling::FindFiles ();

sub run
{
    return Shlomif::Spelling::Check->new()->spell_check(
        {
            files => Shlomif::Spelling::FindFiles->new->list_htmls(),
        },
    );
}

1;
