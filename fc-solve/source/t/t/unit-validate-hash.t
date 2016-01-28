#!/usr/bin/perl

use strict;
use warnings;

use lib './t/lib';

use Test::More tests => 1;

package TestHash;

my %test_hash;

sub get_hash
{
    return \%test_hash;
}

use parent 'FC_Solve::SplitTests::ValidateHash';

__PACKAGE__->populate(
    [
        'id24',
        { foo => 'bar', },
        'foo500',
        { test => 'toast', },
    ]
);

package main;

use Test::Differences qw/eq_or_diff/;

# TEST
eq_or_diff(
    (scalar TestHash::get_hash()),
    +{
        id24 => { foo => 'bar', },
        foo500 => { test => 'toast', },
    },
    'Test that FC_Solve::SplitTests::ValidateHash is doing its job.',
);

=head1 COPYRIGHT AND LICENSE

Copyright (c) 2008 Shlomi Fish

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

