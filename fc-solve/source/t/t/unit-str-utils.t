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
char *c_try_str_prefix(char * str, char * prefix) {
    return try_str_prefix(str, prefix);
}
EOF
);

package main;

use Test::More tests => 5;
use Test::Differences (qw( eq_or_diff ));

sub c_string_starts_with
{
    return FC_Solve::StrUtils::c_string_starts_with(@_);
}

sub try_str_prefix
{
    return FC_Solve::StrUtils::c_try_str_prefix(@_);
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

# TEST
eq_or_diff( try_str_prefix( "PrefixSuffix", "Prefix" ),
    "Suffix", "try_str_prefix success." );

# TEST
ok( !defined( try_str_prefix( "PrefixSuffix", "not" ) ),
    "try_str_prefix failure." );
