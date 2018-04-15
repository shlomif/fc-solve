#!/usr/bin/perl

use strict;
use warnings;

package FC_Solve::StrUtils;

use FC_Solve::InlineWrap (
    C => <<"EOF",
#include "str_utils.h"

int c_string_starts_with( char * str, char * prefix, int end) {
    return string_starts_with(str, prefix, str+end);
}
EOF
);

package main;

use Test::More tests => 3;
use Test::Differences (qw( eq_or_diff ));

sub c_string_starts_with
{
    return FC_Solve::StrUtils::c_string_starts_with(@_);
}

# TEST
ok( scalar( c_string_starts_with( "Hello", "Hel", 3 ) ),
    "string_starts_with 1" );

# TEST
ok( scalar( !c_string_starts_with( "Hello", "Hel", 2 ) ),
    "string_starts_with wrong len" );

# TEST
ok( scalar( !c_string_starts_with( "Hello", "Hew", 3 ) ),
    "string_starts_with wrong prefix." );
